from __future__ import annotations

import json
import os
from pathlib import Path

from PyQt6.QtCore import QDir, QFile, QProcess, QRect, QSize, Qt, QThread, QTimer, pyqtSignal
from PyQt6.QtGui import (
    QAction,
    QCloseEvent,
    QColor,
    QDragEnterEvent,
    QDropEvent,
    QFileSystemModel,
    QFont,
    QIcon,
    QPainter,
    QTextCursor,
)
from PyQt6.QtWidgets import (
    QApplication,
    QCheckBox,
    QDialog,
    QFileDialog,
    QFormLayout,
    QFrame,
    QHBoxLayout,
    QInputDialog,
    QLabel,
    QLineEdit,
    QMainWindow,
    QMenu,
    QMessageBox,
    QPushButton,
    QSplitter,
    QStatusBar,
    QTabWidget,
    QTableWidget,
    QTableWidgetItem,
    QTextEdit,
    QToolBar,
    QTreeView,
    QVBoxLayout,
    QWidget,
)

from .ai_client import (
    AiClientError,
    AiMessage,
    OpenAICompatibleClient,
    explain_error_messages,
    explain_selection_messages,
    hint_messages,
)
from .constants import APP_DISPLAY_NAME, EXAMPLE_CODE, PROJECT_OR_RELEASE_ROOT
from .debug_runner import PROTOCOL_PREFIX
from .editor import CodeEditor
from .errors import format_friendly_error, parse_friendly_error
from .runner import check_command, debug_command, python_env, run_command
from .settings import AppSettings, load_settings, masked_api_key, save_settings


LEARNING_EXAMPLES = [
    ("第 1 课：打招呼", "01_hello_name.py"),
    ("第 2 课：数字和计算", "02_numbers_and_math.py"),
    ("第 3 课：如果……就……", "03_if_choices.py"),
    ("第 4 课：重复做事情", "04_for_loop_stars.py"),
    ("第 5 课：列表小背包", "05_list_backpack.py"),
    ("第 6 课：函数小机器", "06_function_machine.py"),
    ("第 7 课：猜数字游戏", "07_guess_number_steps.py"),
    ("第 8 课：多文件小项目", "08_multi_file_project/main.py"),
]


def icon(name: str, color: str = "#176b8a") -> QIcon:
    try:
        import qtawesome as qta

        return qta.icon(name, color=color)
    except Exception:
        return QIcon()


def pane(title: str, child: QWidget, hint: str = "", show_title: bool = True) -> QWidget:
    frame = QFrame()
    frame.setObjectName("PaneFrame")
    layout = QVBoxLayout(frame)
    layout.setContentsMargins(8, 8, 8, 8)
    layout.setSpacing(6)
    if show_title and title:
        label = QLabel(title)
        label.setObjectName("PaneTitle")
        layout.addWidget(label)
    if hint:
        hint_label = QLabel(hint)
        hint_label.setObjectName("SoftHint")
        hint_label.setWordWrap(True)
        layout.addWidget(hint_label)
    layout.addWidget(child, 1)
    return frame


class OutputPanel(QWidget):
    input_submitted = pyqtSignal(str)

    def __init__(self) -> None:
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(8, 8, 8, 8)
        layout.setSpacing(6)

        self.text = QTextEdit()
        self.text.setReadOnly(True)
        self.text.setPlaceholderText("运行代码后，结果会出现在这里。")
        self.input = QLineEdit()
        self.input.setObjectName("ProcessInput")
        self.input.setPlaceholderText("如果程序问你问题，请在这里输入，然后按 Enter")
        self.input.returnPressed.connect(self._submit_input)

        layout.addWidget(self.text, 1)
        layout.addWidget(self.input)

    def clear(self) -> None:
        self.text.clear()

    def append(self, message: str) -> None:
        self.text.moveCursor(QTextCursor.MoveOperation.End)
        self.text.insertPlainText(message)
        if message and not message.endswith("\n"):
            self.text.insertPlainText("\n")
        self.text.moveCursor(QTextCursor.MoveOperation.End)

    def show_message(self, message: str) -> None:
        self.text.setPlainText(message)

    def set_running(self, running: bool) -> None:
        if running:
            self.input.setPlaceholderText("程序如果问问题，就在这里输入，再按 Enter")
        else:
            self.input.setPlaceholderText("如果程序问你问题，请在这里输入，然后按 Enter")

    def _submit_input(self) -> None:
        value = self.input.text()
        if not value:
            return
        self.input.clear()
        self.append(f"> {value}")
        self.input_submitted.emit(value + "\n")


class DebugVariablesPanel(QWidget):
    continue_requested = pyqtSignal()
    step_requested = pyqtSignal()
    stop_requested = pyqtSignal()

    def __init__(self) -> None:
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(8, 8, 8, 8)
        layout.setSpacing(6)

        button_row = QHBoxLayout()
        self.continue_button = QPushButton("继续")
        self.step_button = QPushButton("下一步")
        self.stop_button = QPushButton("停下")
        self.continue_button.clicked.connect(self.continue_requested)
        self.step_button.clicked.connect(self.step_requested)
        self.stop_button.clicked.connect(self.stop_requested)
        for button in [self.continue_button, self.step_button, self.stop_button]:
            button_row.addWidget(button)
        button_row.addStretch(1)

        self.location = QLabel("还没有开始调试。点击行号加红点断点，再点“调试”。")
        self.location.setObjectName("SoftHint")
        self.table = QTableWidget(0, 2)
        self.table.setHorizontalHeaderLabels(["名字", "值"])
        self.table.horizontalHeader().setStretchLastSection(True)
        self.table.verticalHeader().setVisible(False)

        layout.addLayout(button_row)
        layout.addWidget(self.location)
        layout.addWidget(self.table, 1)
        self.set_debugging(False)

    def set_debugging(self, debugging: bool) -> None:
        self.continue_button.setEnabled(debugging)
        self.step_button.setEnabled(debugging)
        self.stop_button.setEnabled(debugging)

    def show_stop(self, file_name: str, line: int, variables: dict[str, str]) -> None:
        self.location.setText(f"小虫子停在：{Path(file_name).name} 第 {line} 行")
        self.table.setRowCount(len(variables))
        for row, (name, value) in enumerate(sorted(variables.items())):
            self.table.setItem(row, 0, QTableWidgetItem(name))
            self.table.setItem(row, 1, QTableWidgetItem(value))

    def clear(self) -> None:
        self.location.setText("还没有开始调试。点击行号加红点断点，再点“调试”。")
        self.table.setRowCount(0)


class LocationTablePanel(QWidget):
    location_activated = pyqtSignal(str, int)

    def __init__(self, empty_text: str) -> None:
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(8, 8, 8, 8)
        layout.setSpacing(6)

        self.hint = QLabel(empty_text)
        self.hint.setObjectName("SoftHint")
        self.table = QTableWidget(0, 4)
        self.table.setHorizontalHeaderLabels(["文件", "行", "内容", "路径"])
        self.table.horizontalHeader().setStretchLastSection(True)
        self.table.verticalHeader().setVisible(False)
        self.table.setSelectionBehavior(QTableWidget.SelectionBehavior.SelectRows)
        self.table.setEditTriggers(QTableWidget.EditTrigger.NoEditTriggers)
        self.table.cellDoubleClicked.connect(self._cell_double_clicked)
        self.table.hideColumn(3)

        layout.addWidget(self.hint)
        layout.addWidget(self.table, 1)

    def set_rows(self, rows: list[dict[str, object]], hint: str) -> None:
        self.hint.setText(hint)
        self.table.setRowCount(len(rows))
        for row, item in enumerate(rows):
            path = str(item.get("path", ""))
            line = int(item.get("line") or 1)
            message = str(item.get("message", ""))
            self.table.setItem(row, 0, QTableWidgetItem(Path(path).name if path else "新文件"))
            self.table.setItem(row, 1, QTableWidgetItem(str(line)))
            self.table.setItem(row, 2, QTableWidgetItem(message))
            self.table.setItem(row, 3, QTableWidgetItem(path))
        self.table.resizeColumnsToContents()
        self.table.hideColumn(3)
        self.table.horizontalHeader().setStretchLastSection(True)

    def clear(self, hint: str) -> None:
        self.hint.setText(hint)
        self.table.setRowCount(0)

    def _cell_double_clicked(self, row: int, _column: int) -> None:
        path_item = self.table.item(row, 3)
        line_item = self.table.item(row, 1)
        if not path_item or not line_item:
            return
        try:
            line = int(line_item.text())
        except ValueError:
            line = 1
        self.location_activated.emit(path_item.text(), line)


class AiPanel(QWidget):
    ask_requested = pyqtSignal(str)
    explain_selection_requested = pyqtSignal()
    explain_error_requested = pyqtSignal()
    hint_requested = pyqtSignal()
    collapse_requested = pyqtSignal()

    def __init__(self) -> None:
        super().__init__()
        layout = QVBoxLayout(self)
        layout.setContentsMargins(8, 8, 8, 8)
        layout.setSpacing(8)

        title_row = QHBoxLayout()
        title = QLabel("问问小老师")
        title.setObjectName("PaneTitle")
        collapse_button = QPushButton("收起")
        collapse_button.clicked.connect(self.collapse_requested)
        title_row.addWidget(title, 1)
        title_row.addWidget(collapse_button)

        hint = QLabel("可以问：这段代码什么意思？为什么错了？下一步怎么想？")
        hint.setObjectName("SoftHint")
        hint.setWordWrap(True)

        self.answer = QTextEdit()
        self.answer.setReadOnly(True)
        self.answer.setPlaceholderText("AI 小老师的回答会出现在这里。")
        self.input = QLineEdit()
        self.input.setPlaceholderText("在这里输入你的问题")
        self.input.returnPressed.connect(self._ask)

        button_row = QHBoxLayout()
        self.ask_button = QPushButton("问问题")
        self.explain_button = QPushButton("解释代码")
        self.error_button = QPushButton("哪里错了")
        self.hint_button = QPushButton("给点提示")
        self.ask_button.clicked.connect(self._ask)
        self.explain_button.clicked.connect(self.explain_selection_requested)
        self.error_button.clicked.connect(self.explain_error_requested)
        self.hint_button.clicked.connect(self.hint_requested)
        for button in [
            self.ask_button,
            self.explain_button,
            self.error_button,
            self.hint_button,
        ]:
            button_row.addWidget(button)

        layout.addLayout(title_row)
        layout.addWidget(hint)
        layout.addWidget(self.answer, 1)
        layout.addWidget(self.input)
        layout.addLayout(button_row)

    def append_answer(self, text: str) -> None:
        current = self.answer.toPlainText().strip()
        merged = f"{current}\n\n{text}".strip() if current else text
        self.answer.setPlainText(merged)
        self.answer.moveCursor(QTextCursor.MoveOperation.End)

    def set_busy(self, busy: bool) -> None:
        for widget in [
            self.ask_button,
            self.explain_button,
            self.error_button,
            self.hint_button,
            self.input,
        ]:
            widget.setEnabled(not busy)
        if busy:
            self.append_answer("小老师正在思考，请稍等...")

    def _ask(self) -> None:
        question = self.input.text().strip()
        if not question:
            return
        self.input.clear()
        self.append_answer(f"我：{question}")
        self.ask_requested.emit(question)


class AiSettingsDialog(QDialog):
    def __init__(self, settings: AppSettings, parent=None) -> None:
        super().__init__(parent)
        self.setWindowTitle("AI 小老师设置")
        self.setModal(True)
        self.resize(460, 220)
        self.base_url = QLineEdit(settings.base_url)
        self.model = QLineEdit(settings.model)
        self.api_key = QLineEdit(settings.api_key)
        self.api_key.setEchoMode(QLineEdit.EchoMode.Password)
        self.show_key = QCheckBox("显示 Key")
        self.show_key.toggled.connect(self._toggle_key)

        form = QFormLayout()
        form.addRow("Base URL", self.base_url)
        form.addRow("Model", self.model)
        form.addRow("API Key", self.api_key)
        form.addRow("", self.show_key)

        ok = QPushButton("保存")
        cancel = QPushButton("取消")
        ok.clicked.connect(self.accept)
        cancel.clicked.connect(self.reject)
        buttons = QHBoxLayout()
        buttons.addStretch(1)
        buttons.addWidget(ok)
        buttons.addWidget(cancel)

        layout = QVBoxLayout(self)
        layout.addLayout(form)
        layout.addLayout(buttons)

    def _toggle_key(self, checked: bool) -> None:
        mode = QLineEdit.EchoMode.Normal if checked else QLineEdit.EchoMode.Password
        self.api_key.setEchoMode(mode)

    def apply_to(self, settings: AppSettings) -> None:
        settings.base_url = self.base_url.text().strip()
        settings.model = self.model.text().strip()
        settings.api_key = self.api_key.text().strip()


class AiWorker(QThread):
    answer_ready = pyqtSignal(str)
    error_ready = pyqtSignal(str)

    def __init__(self, settings: AppSettings, messages) -> None:
        super().__init__()
        self.settings = settings
        self.messages = messages

    def run(self) -> None:
        try:
            answer = OpenAICompatibleClient(self.settings).chat(self.messages)
        except AiClientError as exc:
            self.error_ready.emit(str(exc))
        except Exception as exc:
            self.error_ready.emit(f"AI 小老师暂时出错了：{exc}")
        else:
            self.answer_ready.emit(answer)


class ToolbarActionButton(QFrame):
    def __init__(self, action: QAction, role: str = "", parent=None) -> None:
        super().__init__(parent)
        self.action = action
        self.setObjectName("ToolbarActionButton")
        self.setProperty("role", role)
        self.setFixedSize(74, 48)
        self.setCursor(Qt.CursorShape.PointingHandCursor)

        self.action.changed.connect(self.sync_from_action)
        self.sync_from_action()

    def sync_from_action(self) -> None:
        self.setEnabled(self.action.isEnabled())
        self.setProperty("checked", self.action.isChecked())
        self.style().unpolish(self)
        self.style().polish(self)
        self.update()

    def paintEvent(self, event) -> None:  # noqa: N802 - Qt API
        super().paintEvent(event)
        painter = QPainter(self)
        painter.setRenderHint(QPainter.RenderHint.TextAntialiasing)
        painter.setRenderHint(QPainter.RenderHint.SmoothPixmapTransform)

        mode = QIcon.Mode.Normal if self.isEnabled() else QIcon.Mode.Disabled
        pixmap = self.action.icon().pixmap(QSize(18, 18), mode)
        icon_x = (self.width() - pixmap.width()) // 2
        painter.drawPixmap(icon_x, 6, pixmap)

        font = QFont("Microsoft YaHei", 8)
        font.setBold(True)
        painter.setFont(font)
        painter.setPen(QColor(self._text_color()))
        painter.drawText(
            QRect(3, 28, self.width() - 6, 17),
            Qt.AlignmentFlag.AlignCenter,
            self.action.text(),
        )

    def _text_color(self) -> str:
        if not self.isEnabled():
            return "#96a6b2"
        role = self.property("role")
        if role == "run":
            return "#11643a"
        if role == "stop":
            return "#a23128"
        if role == "warn":
            return "#8a5a00"
        if role == "hint" and self.action.isChecked():
            return "#176b3a"
        return "#18465c"

    def mouseReleaseEvent(self, event) -> None:  # noqa: N802 - Qt API
        if (
            self.isEnabled()
            and event.button() == Qt.MouseButton.LeftButton
            and self.rect().contains(event.position().toPoint())
        ):
            self.action.trigger()
            event.accept()
            return
        super().mouseReleaseEvent(event)


class MainWindow(QMainWindow):
    def __init__(self) -> None:
        super().__init__()
        self.settings = load_settings()
        self.current_folder: Path = Path(self.settings.last_folder or Path.cwd())
        self.main_file: Path | None = None
        self.process: QProcess | None = None
        self.process_label = ""
        self.debugging = False
        self.stderr_buffer = ""
        self.last_error_text = ""
        self.last_find_text = ""
        self.process_cwd: Path = self.current_folder
        self.ai_worker: AiWorker | None = None
        self.main_splitter: QSplitter | None = None
        self.vertical_splitter: QSplitter | None = None
        self.project_pane: QWidget | None = None
        self.focus_mode = False
        self.ai_visible_before_focus = False

        self.setWindowTitle(APP_DISPLAY_NAME)
        self.resize(1400, 860)
        self.setAcceptDrops(True)
        self._setup_widgets()
        self._setup_actions()
        self._setup_menu()
        self._setup_toolbar()
        self._set_debug_controls_enabled(False)
        self.setStatusBar(QStatusBar())
        self.syntax_timer = QTimer(self)
        self.syntax_timer.setSingleShot(True)
        self.syntax_timer.setInterval(700)
        self.syntax_timer.timeout.connect(self.check_current_syntax)
        self.statusBar().showMessage("准备好了。写一点代码，然后按“运行”。")
        self.new_file()
        self._open_folder(self.current_folder)

    @property
    def editor(self) -> CodeEditor:
        current = self.editor_tabs.currentWidget()
        if isinstance(current, CodeEditor):
            return current
        return self._add_editor(EXAMPLE_CODE, None)

    @property
    def current_file(self) -> Path | None:
        return self.editor.current_path

    def _setup_widgets(self) -> None:
        self.file_model = QFileSystemModel(self)
        self.file_model.setNameFilters(["*.py", "*.txt", "*.md"])
        self.file_model.setNameFilterDisables(False)
        self.file_model.setFilter(QDir.Filter.AllDirs | QDir.Filter.Files | QDir.Filter.NoDotAndDotDot)

        self.tree = QTreeView()
        self.tree.setObjectName("ProjectTree")
        self.tree.setModel(self.file_model)
        self.tree.doubleClicked.connect(self._tree_double_clicked)
        self.tree.setContextMenuPolicy(Qt.ContextMenuPolicy.CustomContextMenu)
        self.tree.customContextMenuRequested.connect(self._show_tree_context_menu)
        self.tree.setHeaderHidden(True)
        self.tree.setIndentation(14)
        for column in range(1, 4):
            self.tree.hideColumn(column)

        self.editor_tabs = QTabWidget()
        self.editor_tabs.setObjectName("EditorTabs")
        self.editor_tabs.setTabsClosable(True)
        self.editor_tabs.tabCloseRequested.connect(self.close_tab)
        self.editor_tabs.currentChanged.connect(lambda _idx: self._update_current_status())

        self.ai_panel = AiPanel()
        self.output = OutputPanel()
        self.debug_panel = DebugVariablesPanel()
        self.problems_panel = LocationTablePanel("现在没有发现问题。")
        self.references_panel = LocationTablePanel("查找引用后，结果会出现在这里。")
        self.bottom_tabs = QTabWidget()
        self.bottom_tabs.setObjectName("BottomTabs")
        self.bottom_tabs.addTab(self.output, icon("fa5s.terminal", "#176b8a"), "运行结果")
        self.bottom_tabs.addTab(self.debug_panel, icon("fa5s.bug", "#d9822b"), "调试变量")
        self.bottom_tabs.addTab(self.problems_panel, icon("fa5s.exclamation-circle", "#b45309"), "问题")
        self.bottom_tabs.addTab(self.references_panel, icon("fa5s.search-location", "#176b8a"), "查找结果")
        self.output.input_submitted.connect(self._send_process_input)
        self.debug_panel.continue_requested.connect(lambda: self._send_debug_command("continue"))
        self.debug_panel.step_requested.connect(lambda: self._send_debug_command("step"))
        self.debug_panel.stop_requested.connect(self.stop_process)
        self.problems_panel.location_activated.connect(self.open_location)
        self.references_panel.location_activated.connect(self.open_location)

        self.ai_panel.ask_requested.connect(self._ask_ai)
        self.ai_panel.explain_selection_requested.connect(self._explain_selection)
        self.ai_panel.explain_error_requested.connect(self._explain_error)
        self.ai_panel.hint_requested.connect(self._ask_hint)
        self.ai_panel.collapse_requested.connect(self.hide_ai_panel)

        main_splitter = QSplitter(Qt.Orientation.Horizontal)
        main_splitter.setObjectName("MainSplitter")
        self.project_pane = pane("我的作品", self.tree)
        self.project_pane.setObjectName("ProjectPane")
        self.project_pane.setMinimumWidth(145)
        main_splitter.addWidget(self.project_pane)
        main_splitter.addWidget(pane("", self.editor_tabs, show_title=False))
        main_splitter.addWidget(self.ai_panel)
        main_splitter.setStretchFactor(0, 0)
        main_splitter.setStretchFactor(1, 1)
        main_splitter.setStretchFactor(2, 0)
        main_splitter.setCollapsible(0, False)
        main_splitter.setCollapsible(1, False)
        main_splitter.setSizes([175, 1200, 0])
        self.ai_panel.hide()
        self.main_splitter = main_splitter

        vertical = QSplitter(Qt.Orientation.Vertical)
        vertical.setObjectName("VerticalSplitter")
        vertical.addWidget(main_splitter)
        vertical.addWidget(self.bottom_tabs)
        vertical.setStretchFactor(0, 1)
        vertical.setStretchFactor(1, 0)
        vertical.setSizes([720, 135])
        self.vertical_splitter = vertical
        self.setCentralWidget(vertical)

    def _setup_actions(self) -> None:
        self.new_action = QAction(icon("fa5s.plus", "#2a9d55"), "新建", self)
        self.open_file_action = QAction(icon("fa5s.file-code", "#176b8a"), "打开文件", self)
        self.open_folder_action = QAction(icon("fa5s.folder-open", "#176b8a"), "打开文件夹", self)
        self.save_action = QAction(icon("fa5s.save", "#176b8a"), "保存", self)
        self.save_all_action = QAction("保存全部", self)
        self.set_main_action = QAction(icon("fa5s.flag", "#d9822b"), "设为运行入口", self)
        self.check_action = QAction(icon("fa5s.check-circle", "#2a9d55"), "检查", self)
        self.run_action = QAction(icon("fa5s.play", "#2a9d55"), "运行", self)
        self.stop_action = QAction(icon("fa5s.stop", "#b42318"), "停下", self)
        self.debug_action = QAction(icon("fa5s.bug", "#d9822b"), "调试", self)
        self.debug_continue_action = QAction(icon("fa5s.forward", "#2a9d55"), "继续", self)
        self.debug_step_action = QAction(icon("fa5s.shoe-prints", "#176b8a"), "下一步", self)
        self.ask_action = QAction(icon("fa5s.question-circle", "#176b8a"), "问问", self)
        self.error_help_action = QAction(icon("fa5s.life-ring", "#d9822b"), "哪里错了", self)
        self.hints_action = QAction(icon("fa5s.lightbulb", "#d9822b"), "代码提示", self)
        self.hints_action.setCheckable(True)
        self.hints_action.setChecked(self.settings.code_hints_enabled)
        self.find_action = QAction("查找文字", self)
        self.goto_definition_action = QAction("跳到定义", self)
        self.find_references_action = QAction("查找引用", self)
        self.manual_completion_action = QAction("叫出代码提示", self)
        self.comment_action = QAction("注释/取消注释", self)
        self.zoom_in_action = QAction("字变大", self)
        self.zoom_out_action = QAction("字变小", self)
        self.focus_mode_action = QAction("专心写代码", self)
        self.focus_mode_action.setCheckable(True)
        self.show_project_tree_action = QAction(icon("fa5s.folder-open", "#176b8a"), "显示我的作品", self)
        self.reset_layout_action = QAction(icon("fa5s.undo", "#176b8a"), "恢复默认布局", self)
        self.ai_settings_action = QAction("AI 小老师设置", self)
        self.open_learning_folder_action = QAction(icon("fa5s.book-open", "#176b8a"), "打开学习示例文件夹", self)
        self.learning_actions: list[QAction] = []
        for title, relative_path in LEARNING_EXAMPLES:
            action = QAction(icon("fa5s.graduation-cap", "#0f766e"), title, self)
            action.triggered.connect(
                lambda _checked=False, path=relative_path: self.open_learning_example(path)
            )
            self.learning_actions.append(action)

        self.new_action.triggered.connect(self.new_file)
        self.open_file_action.triggered.connect(self.open_file_dialog)
        self.open_folder_action.triggered.connect(self.open_folder_dialog)
        self.save_action.triggered.connect(self.save_file)
        self.save_all_action.triggered.connect(self.save_all_files)
        self.set_main_action.triggered.connect(self.set_current_as_main_file)
        self.check_action.triggered.connect(self.check_file)
        self.run_action.triggered.connect(self.run_file)
        self.stop_action.triggered.connect(self.stop_process)
        self.debug_action.triggered.connect(self.debug_file)
        self.debug_continue_action.triggered.connect(lambda: self._send_debug_command("continue"))
        self.debug_step_action.triggered.connect(lambda: self._send_debug_command("step"))
        self.ask_action.triggered.connect(self.show_ai_panel)
        self.error_help_action.triggered.connect(self.show_local_error_help)
        self.hints_action.toggled.connect(self._set_hints_enabled)
        self.find_action.triggered.connect(self.find_text_dialog)
        self.goto_definition_action.triggered.connect(self.goto_definition)
        self.find_references_action.triggered.connect(self.find_references)
        self.manual_completion_action.triggered.connect(lambda: self.editor.show_jedi_completions())
        self.comment_action.triggered.connect(lambda: self.editor.toggle_comment())
        self.zoom_in_action.triggered.connect(lambda: self.editor.increase_font_size())
        self.zoom_out_action.triggered.connect(lambda: self.editor.decrease_font_size())
        self.focus_mode_action.toggled.connect(self.set_focus_mode)
        self.show_project_tree_action.triggered.connect(self.show_project_tree)
        self.reset_layout_action.triggered.connect(self.reset_layout)
        self.ai_settings_action.triggered.connect(self.open_ai_settings)
        self.open_learning_folder_action.triggered.connect(self.open_learning_folder)

        self.save_action.setShortcut("Ctrl+S")
        self.save_all_action.setShortcut("Ctrl+Shift+S")
        self.open_file_action.setShortcut("Ctrl+O")
        self.find_action.setShortcut("Ctrl+F")
        self.goto_definition_action.setShortcut("F12")
        self.find_references_action.setShortcut("Shift+F12")
        self.manual_completion_action.setShortcut("Ctrl+Space")
        self.comment_action.setShortcut("Ctrl+/")
        self.zoom_in_action.setShortcut("Ctrl+=")
        self.zoom_out_action.setShortcut("Ctrl+-")
        self.show_project_tree_action.setShortcut("Ctrl+Alt+1")
        self.reset_layout_action.setShortcut("Ctrl+Alt+0")
        self.check_action.setShortcut("F5")
        self.run_action.setShortcut("F6")
        self.debug_action.setShortcut("F7")
        self.debug_continue_action.setShortcut("F8")
        self.debug_step_action.setShortcut("F10")
        self.new_action.setShortcut("Ctrl+N")
        self.focus_mode_action.setShortcut("Ctrl+Shift+F")
        self.stop_action.setEnabled(False)

    def _setup_menu(self) -> None:
        file_menu = self.menuBar().addMenu("文件")
        file_menu.addAction(self.new_action)
        file_menu.addAction(self.open_file_action)
        file_menu.addAction(self.open_folder_action)
        file_menu.addAction(self.save_action)
        file_menu.addAction(self.save_all_action)

        edit_menu = self.menuBar().addMenu("编辑")
        edit_menu.addAction(self.find_action)
        edit_menu.addAction(self.goto_definition_action)
        edit_menu.addAction(self.find_references_action)
        edit_menu.addAction(self.manual_completion_action)
        edit_menu.addSeparator()
        edit_menu.addAction(self.comment_action)
        edit_menu.addAction(self.zoom_in_action)
        edit_menu.addAction(self.zoom_out_action)
        edit_menu.addSeparator()
        edit_menu.addAction(self.hints_action)

        learning_menu = self.menuBar().addMenu("学习")
        for action in self.learning_actions:
            learning_menu.addAction(action)
        learning_menu.addSeparator()
        learning_menu.addAction(self.open_learning_folder_action)

        run_menu = self.menuBar().addMenu("运行")
        run_menu.addAction(self.check_action)
        run_menu.addAction(self.run_action)
        run_menu.addAction(self.set_main_action)
        run_menu.addAction(self.stop_action)
        run_menu.addAction(self.debug_action)
        run_menu.addAction(self.debug_continue_action)
        run_menu.addAction(self.debug_step_action)

        ai_menu = self.menuBar().addMenu("AI")
        ai_menu.addAction(self.ask_action)
        ai_menu.addAction(self.error_help_action)
        ai_menu.addAction(self.ai_settings_action)

        view_menu = self.menuBar().addMenu("视图")
        view_menu.addAction(self.show_project_tree_action)
        view_menu.addAction(self.reset_layout_action)
        view_menu.addSeparator()
        view_menu.addAction(self.focus_mode_action)

        help_menu = self.menuBar().addMenu("帮助")
        about = QAction("关于", self)
        about.triggered.connect(self._show_about)
        help_menu.addAction(about)

    def _setup_toolbar(self) -> None:
        toolbar = QToolBar("常用工具")
        toolbar.setObjectName("MainToolbar")
        toolbar.setMovable(False)
        toolbar.setIconSize(QSize(22, 22))
        self.addToolBar(toolbar)

        def add_button(action: QAction, role: str = "") -> None:
            toolbar.addWidget(self._toolbar_button(action, role))

        for action in [
            self.new_action,
            self.open_file_action,
            self.open_folder_action,
            self.save_action,
        ]:
            add_button(action)
        toolbar.addSeparator()
        add_button(self.run_action, "run")
        add_button(self.stop_action, "stop")
        add_button(self.debug_action, "warn")
        toolbar.addSeparator()
        add_button(self.ask_action)
        add_button(self.error_help_action, "warn")
        add_button(self.hints_action, "hint")

    def _toolbar_button(self, action: QAction, role: str = "") -> ToolbarActionButton:
        button = ToolbarActionButton(action, role, self)
        shortcut = action.shortcut().toString()
        button.setToolTip(f"{action.text()}  {shortcut}".strip())
        return button

    def _add_editor(self, text: str, path: Path | None) -> CodeEditor:
        editor = CodeEditor()
        editor.set_code_hints_enabled(self.settings.code_hints_enabled)
        editor.set_current_path(path)
        editor.setText(text)
        editor.setModified(False)
        editor.modificationChanged.connect(lambda _modified, e=editor: self._update_tab_title(e))
        editor.textChanged.connect(lambda e=editor: self._editor_text_changed(e))
        editor.cursorPositionChanged.connect(lambda _line, _col: self._update_current_status())
        editor.breakpoints_changed.connect(lambda _lines, e=editor: self._update_tab_title(e))
        title = self._tab_title(editor)
        self.editor_tabs.addTab(editor, self._tab_icon(editor), title)
        self.editor_tabs.setCurrentWidget(editor)
        return editor

    def _tab_title(self, editor: CodeEditor) -> str:
        name = editor.current_path.name if editor.current_path else "新文件"
        if self.main_file and editor.current_path and editor.current_path.resolve() == self.main_file.resolve():
            name = "★ " + name
        if editor.isModified():
            name += " *"
        return name

    def _update_tab_title(self, editor: CodeEditor) -> None:
        idx = self.editor_tabs.indexOf(editor)
        if idx >= 0:
            self.editor_tabs.setTabText(idx, self._tab_title(editor))
            self.editor_tabs.setTabIcon(idx, self._tab_icon(editor))

    def _tab_icon(self, editor: CodeEditor) -> QIcon:
        if editor.current_path is None:
            return icon("fa5s.file", "#5b778a")
        suffix = editor.current_path.suffix.lower()
        if suffix == ".py":
            return icon("fa5s.file-code", "#176b8a")
        if suffix == ".md":
            return icon("fa5s.book-open", "#7c3aed")
        return icon("fa5s.file-alt", "#5b778a")

    def _update_all_tab_titles(self) -> None:
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor):
                self._update_tab_title(editor)

    def _update_current_status(self) -> None:
        editor = self.editor
        line, col = editor.getCursorPosition()
        file_name = self.current_file.name if self.current_file else "新文件"
        state = self.process_label + "中" if self.process else "准备好了"
        parts = [state, file_name, f"第 {line + 1} 行，第 {col + 1} 列"]
        if self.main_file:
            parts.append(f"运行入口：{self.main_file.name}")
        self.statusBar().showMessage(" | ".join(parts))

    def _editor_text_changed(self, editor: CodeEditor) -> None:
        self._update_tab_title(editor)
        if editor is self.editor:
            self.syntax_timer.start()

    def show_ai_panel(self) -> None:
        if self.ai_panel.isHidden():
            self.ai_panel.show()
            if self.main_splitter is not None:
                self.main_splitter.setSizes([180, 980, 320])
        self.ai_panel.input.setFocus()

    def hide_ai_panel(self) -> None:
        self.ai_panel.hide()
        if self.main_splitter is not None:
            self.main_splitter.setSizes([190, 1160, 0])
        self.editor.setFocus()

    def show_project_tree(self) -> None:
        if self.focus_mode_action.isChecked():
            self.focus_mode_action.setChecked(False)
        elif self.focus_mode:
            self.set_focus_mode(False)
        self._restore_project_tree()
        self.statusBar().showMessage("“我的作品”已经回到左边。")

    def reset_layout(self) -> None:
        if self.focus_mode_action.isChecked():
            self.focus_mode_action.setChecked(False)
        elif self.focus_mode:
            self.set_focus_mode(False)
        self._restore_project_tree()
        self.bottom_tabs.show()
        self.ai_panel.hide()
        if self.main_splitter is not None:
            self.main_splitter.setSizes([175, 1200, 0])
        if self.vertical_splitter is not None:
            self.vertical_splitter.setSizes([720, 135])
        self.editor.setFocus()
        self.statusBar().showMessage("布局已经恢复默认。")

    def _restore_project_tree(self) -> None:
        if self.project_pane is not None:
            self.project_pane.show()
        self.tree.show()
        if self.main_splitter is None:
            return
        sizes = self.main_splitter.sizes()
        total = sum(sizes) if sizes else self.width()
        ai_width = 320 if not self.ai_panel.isHidden() else 0
        left_width = 175
        center_width = max(700, total - left_width - ai_width)
        self.main_splitter.setSizes([left_width, center_width, ai_width])
        self.editor.setFocus()

    def _open_folder(self, folder: Path) -> None:
        folder = folder if folder.exists() else Path.cwd()
        self.current_folder = folder
        self.process_cwd = folder
        root = self.file_model.setRootPath(str(folder))
        self.tree.setRootIndex(root)
        self.settings.last_folder = str(folder)
        save_settings(self.settings)

    def open_folder_dialog(self) -> None:
        folder = QFileDialog.getExistingDirectory(self, "打开你的作品文件夹", str(self.current_folder))
        if folder:
            self._open_folder(Path(folder))

    def open_file_dialog(self) -> None:
        path_text, _ = QFileDialog.getOpenFileName(
            self,
            "打开文件",
            str(self.current_folder),
            "Python/文本文件 (*.py *.txt *.md);;所有文件 (*.*)",
        )
        if not path_text:
            return
        path = Path(path_text)
        self._open_folder(path.parent)
        self.open_file(path)

    def open_learning_folder(self) -> None:
        folder = self._learning_examples_root()
        if not folder.exists():
            QMessageBox.warning(self, "没有找到学习示例", f"学习示例文件夹不见了：\n{folder}")
            return
        self._open_folder(folder)
        self.statusBar().showMessage("学习示例已经放在左边。可以从第 1 课开始。")

    def open_learning_example(self, relative_path: str) -> None:
        path = (self._learning_examples_root() / relative_path).resolve()
        root = self._learning_examples_root().resolve()
        try:
            path.relative_to(root)
        except ValueError:
            QMessageBox.warning(self, "打不开示例", "这个示例位置不太对。")
            return
        if not path.exists():
            QMessageBox.warning(self, "没有找到示例", f"这个学习示例不见了：\n{path}")
            return
        self._open_folder(path.parent)
        self.open_file(path)
        self.statusBar().showMessage(f"打开学习示例：{path.name}。可以先点“运行”看看。")

    def _learning_examples_root(self) -> Path:
        return PROJECT_OR_RELEASE_ROOT / "examples" / "learning"

    def _tree_double_clicked(self, index) -> None:
        path = Path(self.file_model.filePath(index))
        if path.is_file() and path.suffix.lower() in {".py", ".txt", ".md"}:
            self.open_file(path)

    def _show_tree_context_menu(self, position) -> None:
        index = self.tree.indexAt(position)
        selected = Path(self.file_model.filePath(index)) if index.isValid() else self.current_folder
        folder = selected if selected.is_dir() else selected.parent

        menu = QMenu(self)
        new_file_action = menu.addAction("新建 Python 文件")
        rename_action = menu.addAction("重命名")
        delete_action = menu.addAction("删除到回收站")
        rename_action.setEnabled(index.isValid())
        delete_action.setEnabled(index.isValid() and selected.exists())

        chosen = menu.exec(self.tree.viewport().mapToGlobal(position))
        if chosen == new_file_action:
            self._create_project_file(folder)
        elif chosen == rename_action:
            self._rename_project_item(selected)
        elif chosen == delete_action:
            self._trash_project_item(selected)

    def _create_project_file(self, folder: Path) -> None:
        folder = folder if folder.exists() else self.current_folder
        name, ok = QInputDialog.getText(self, "新建文件", "文件名：", text="new_file.py")
        if not ok:
            return
        name = name.strip()
        if not name:
            return
        path = folder / name
        if path.suffix == "":
            path = path.with_suffix(".py")
        if path.exists():
            QMessageBox.information(self, "已经有这个文件", "换一个名字试试。")
            return
        try:
            path.write_text("# 新作品\n\n", encoding="utf-8")
        except OSError as exc:
            QMessageBox.warning(self, "新建失败", f"没有新建成功：\n{exc}")
            return
        self.open_file(path)

    def _rename_project_item(self, path: Path) -> None:
        if not path.exists():
            return
        new_name, ok = QInputDialog.getText(self, "重命名", "新名字：", text=path.name)
        if not ok:
            return
        new_name = new_name.strip()
        if not new_name or new_name == path.name:
            return
        target = path.with_name(new_name)
        if target.exists():
            QMessageBox.information(self, "名字重复了", "这里已经有同名文件。")
            return
        try:
            path.rename(target)
        except OSError as exc:
            QMessageBox.warning(self, "重命名失败", f"没有改名成功：\n{exc}")
            return
        self._replace_open_path(path, target)
        self.statusBar().showMessage(f"已经改名为：{target.name}")

    def _trash_project_item(self, path: Path) -> None:
        if not path.exists():
            return
        answer = QMessageBox.question(
            self,
            "删除到回收站？",
            f"要把 {path.name} 放进回收站吗？",
            QMessageBox.StandardButton.Yes | QMessageBox.StandardButton.No,
        )
        if answer != QMessageBox.StandardButton.Yes:
            return
        result = QFile.moveToTrash(str(path))
        ok = result[0] if isinstance(result, tuple) else bool(result)
        if not ok:
            QMessageBox.warning(self, "删除失败", "没有放进回收站。可以请老师或家长帮忙。")
            return
        self._close_deleted_paths(path)
        self.statusBar().showMessage(f"已经放进回收站：{path.name}")

    def _replace_open_path(self, old: Path, new: Path) -> None:
        old = old.resolve()
        new = new.resolve()
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor) and editor.current_path:
                try:
                    relative = editor.current_path.resolve().relative_to(old)
                except ValueError:
                    if editor.current_path.resolve() == old:
                        editor.set_current_path(new)
                else:
                    editor.set_current_path(new / relative)
                self._update_tab_title(editor)
        if self.main_file and self.main_file.resolve() == old:
            self.main_file = new
        self._update_all_tab_titles()

    def _close_deleted_paths(self, deleted: Path) -> None:
        deleted = deleted.resolve()
        for idx in reversed(range(self.editor_tabs.count())):
            editor = self.editor_tabs.widget(idx)
            if not isinstance(editor, CodeEditor) or not editor.current_path:
                continue
            path = editor.current_path.resolve()
            is_deleted = path == deleted
            if deleted.is_dir():
                try:
                    path.relative_to(deleted)
                    is_deleted = True
                except ValueError:
                    pass
            if is_deleted:
                self.editor_tabs.removeTab(idx)
                editor.deleteLater()
        if self.main_file:
            main = self.main_file.resolve()
            clear_main = main == deleted
            if deleted.is_dir():
                try:
                    main.relative_to(deleted)
                    clear_main = True
                except ValueError:
                    pass
            if clear_main:
                self.main_file = None
        if self.editor_tabs.count() == 0:
            self.new_file()
        self._update_all_tab_titles()

    def new_file(self) -> None:
        self._add_editor(EXAMPLE_CODE, None)
        self.output.clear()
        self.statusBar().showMessage("新文件已经准备好。记得保存成 .py 文件。")

    def open_file(self, path: Path) -> None:
        path = path.resolve()
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor) and editor.current_path and editor.current_path.resolve() == path:
                self.editor_tabs.setCurrentIndex(idx)
                return
        try:
            text = path.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            text = path.read_text(encoding="gbk", errors="replace")
        except OSError as exc:
            QMessageBox.warning(self, "打不开文件", f"这个文件暂时打不开：\n{exc}")
            return
        editor = self._add_editor(text, path)
        editor.clear_error_markers()
        self.statusBar().showMessage(f"打开了：{path.name}")

    def close_tab(self, index: int) -> None:
        editor = self.editor_tabs.widget(index)
        if not isinstance(editor, CodeEditor):
            return
        if not self._maybe_save_editor(editor):
            return
        closed_path = editor.current_path.resolve() if editor.current_path else None
        self.editor_tabs.removeTab(index)
        editor.deleteLater()
        if self.main_file and closed_path and self.main_file.resolve() == closed_path:
            self.main_file = None
            self.statusBar().showMessage("运行入口标签已经关闭，需要时可以重新设置。")
        if self.editor_tabs.count() == 0:
            self.new_file()
        self._update_all_tab_titles()

    def save_file(self) -> bool:
        return self._save_editor(self.editor)

    def _save_editor(self, editor: CodeEditor) -> bool:
        if editor.current_path is None:
            path_text, _ = QFileDialog.getSaveFileName(
                self,
                "保存你的 Python 作品",
                str(self.current_folder / "my_program.py"),
                "Python 文件 (*.py);;文本文件 (*.txt *.md);;所有文件 (*.*)",
            )
            if not path_text:
                return False
            editor.set_current_path(Path(path_text).resolve())
        try:
            editor.current_path.write_text(editor.text(), encoding="utf-8")
        except OSError as exc:
            QMessageBox.warning(self, "保存失败", f"没有保存成功：\n{exc}")
            return False
        editor.setModified(False)
        self._update_tab_title(editor)
        self.statusBar().showMessage(f"保存好了：{editor.current_path.name}")
        return True

    def _maybe_save_editor(self, editor: CodeEditor) -> bool:
        if not editor.isModified():
            return True
        answer = QMessageBox.question(
            self,
            "要保存吗？",
            f"{self._tab_title(editor).replace(' *', '')} 还没有保存。要先保存吗？",
            QMessageBox.StandardButton.Save
            | QMessageBox.StandardButton.Discard
            | QMessageBox.StandardButton.Cancel,
        )
        if answer == QMessageBox.StandardButton.Save:
            return self._save_editor(editor)
        if answer == QMessageBox.StandardButton.Cancel:
            return False
        return True

    def _maybe_save_all(self) -> bool:
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor) and not self._maybe_save_editor(editor):
                self.editor_tabs.setCurrentIndex(idx)
                return False
        return True

    def save_all_files(self) -> bool:
        if not self._save_modified_editors():
            return False
        self.statusBar().showMessage("全部保存好了。")
        return True

    def set_current_as_main_file(self) -> None:
        target = self._ensure_current_saved_python_file()
        if target is None:
            return
        self.main_file = target.resolve()
        self._update_all_tab_titles()
        self.statusBar().showMessage(f"运行入口设为：{self.main_file.name}。以后会从它开始运行。")

    def _run_target_file(self) -> Path | None:
        if self.main_file and self.main_file.exists():
            return self.main_file
        if self.current_file is not None:
            return self.current_file
        return None

    def _project_root(self, file: Path) -> Path:
        if self.current_folder.exists():
            return self.current_folder.resolve()
        return file.resolve().parent

    def check_file(self) -> None:
        target = self._ensure_current_saved_python_file()
        if target is None:
            return
        self._start_python_process(
            check_command(target, cwd=self._project_root(target)),
            "检查",
        )

    def run_file(self) -> None:
        target = self._ensure_run_target()
        if target is None:
            return
        self._start_python_process(run_command(target, cwd=self._project_root(target)), "运行")

    def debug_file(self) -> None:
        target = self._ensure_run_target()
        if target is None:
            return
        self._start_python_process(
            debug_command(target, self._project_root(target), self._breakpoints_payload()),
            "调试",
        )

    def _ensure_current_saved_python_file(self) -> Path | None:
        if not self.save_file():
            return None
        if self.current_file is None or self.current_file.suffix.lower() != ".py":
            QMessageBox.information(self, "需要 Python 文件", "请把作品保存成 .py 文件后再运行。")
            return None
        return self.current_file.resolve()

    def _ensure_run_target(self) -> Path | None:
        if not self._save_modified_editors():
            return None
        target = self._run_target_file()
        if target is None:
            return self._ensure_current_saved_python_file()
        target = target.resolve()
        if not target.exists():
            QMessageBox.information(self, "找不到运行入口", "运行入口文件不见了。请重新打开它，或者重新设置。")
            self.main_file = None
            self._update_all_tab_titles()
            return None
        if target.suffix.lower() != ".py":
            QMessageBox.information(self, "需要 Python 文件", "运行和调试需要选择 .py 文件。")
            return None
        return target

    def _save_modified_editors(self) -> bool:
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor) and editor.isModified():
                self.editor_tabs.setCurrentIndex(idx)
                if not self._save_editor(editor):
                    return False
        return True

    def _breakpoints_payload(self) -> dict[str, list[int]]:
        payload: dict[str, list[int]] = {}
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor) and editor.current_path and editor.breakpoints:
                payload[str(editor.current_path.resolve())] = sorted(editor.breakpoints)
        return payload

    def _start_python_process(self, command, label: str) -> None:
        self.stop_process()
        self.process_label = label
        self.process_cwd = command.cwd
        self.debugging = label == "调试"
        self.stderr_buffer = ""
        self.last_error_text = ""
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor):
                editor.clear_error_markers()
                editor.clear_debug_position()
        self.output.clear()
        self.output.set_running(True)
        self.stop_action.setEnabled(True)
        self.debug_panel.clear()
        self.problems_panel.clear("正在检查运行结果。")
        self._set_debug_controls_enabled(self.debugging)
        self.output.append(f"{label}开始：{command.display}\n")
        if self.debugging:
            self.bottom_tabs.setCurrentWidget(self.debug_panel)
            self.output.append("小虫子启动了。遇到红点断点会停下来。")
        else:
            self.bottom_tabs.setCurrentWidget(self.output)

        process = QProcess(self)
        env = process.processEnvironment()
        for key, value in python_env().items():
            env.insert(key, value)
        process.setProcessEnvironment(env)
        process.setWorkingDirectory(str(command.cwd))
        process.readyReadStandardOutput.connect(self._read_stdout)
        process.readyReadStandardError.connect(self._read_stderr)
        process.finished.connect(self._process_finished)
        process.start(str(command.program), command.args)
        if not process.waitForStarted(3000):
            self.output.append("没有启动成功。请把这个窗口截图给老师或家长。")
            self.debugging = False
            self.stop_action.setEnabled(False)
            self._set_debug_controls_enabled(False)
            return
        self.process = process
        self.statusBar().showMessage(f"{label}中...")

    def _read_stdout(self) -> None:
        if self.process:
            data = bytes(self.process.readAllStandardOutput()).decode("utf-8", errors="replace")
            self.output.append(data)

    def _read_stderr(self) -> None:
        if not self.process:
            return
        data = bytes(self.process.readAllStandardError()).decode("utf-8", errors="replace")
        self.stderr_buffer += data
        while "\n" in self.stderr_buffer:
            line, self.stderr_buffer = self.stderr_buffer.split("\n", 1)
            self._handle_stderr_line(line + "\n")

    def _handle_stderr_line(self, line: str) -> None:
        if line.startswith(PROTOCOL_PREFIX):
            self._handle_debug_event(line[len(PROTOCOL_PREFIX) :].strip())
            return
        self.last_error_text += line
        self.output.append(line)

    def _handle_debug_event(self, raw: str) -> None:
        try:
            event = json.loads(raw)
        except json.JSONDecodeError:
            self.output.append(raw)
            return
        event_type = event.get("type")
        if event_type == "started":
            self.output.append("调试开始。")
        elif event_type == "message":
            self.output.append(str(event.get("message", "")))
        elif event_type in {"stop", "exception"}:
            file_name = str(event.get("file", ""))
            line = int(event.get("line") or 0)
            variables = event.get("locals") if isinstance(event.get("locals"), dict) else {}
            self._show_debug_position(file_name, line)
            self.debug_panel.show_stop(file_name, line, variables)
            if event_type == "exception":
                self.output.append(f"程序在这里出错：{event.get('name')}: {event.get('message')}")
            self.statusBar().showMessage("小虫子停住了，可以点“继续”或“下一步”。")
        elif event_type == "finished":
            self.output.append("调试结束。")
        elif event_type == "crash":
            self.output.append(f"调试时遇到错误：{event.get('name')}: {event.get('message')}")

    def _show_debug_position(self, file_name: str, line: int) -> None:
        path = self.open_location(file_name, line)
        self.editor.set_debug_position(line)

    def _send_process_input(self, text: str) -> None:
        if self.process and self.process.state() == QProcess.ProcessState.Running:
            self.process.write(text.encode("utf-8"))

    def _send_debug_command(self, command: str) -> None:
        if not self.debugging or not self.process:
            return
        self.process.write((command + "\n").encode("utf-8"))

    def _process_finished(self, exit_code: int, _exit_status=None) -> None:
        if self.stderr_buffer:
            self._handle_stderr_line(self.stderr_buffer)
            self.stderr_buffer = ""
        self.process = None
        was_debugging = self.debugging
        self.debugging = False
        self.output.set_running(False)
        self.stop_action.setEnabled(False)
        self._set_debug_controls_enabled(False)
        self._clear_debug_positions()
        if exit_code == 0:
            self.statusBar().showMessage("完成了。")
            self.output.append("\n调试完成。" if was_debugging else "\n完成了。")
            self.problems_panel.clear("运行完成，没有发现会让程序停下来的错误。")
            return
        friendly = parse_friendly_error(self.last_error_text)
        if friendly:
            self._show_problem(friendly, switch_to_tab=True)
            self.output.show_message(format_friendly_error(friendly))
        self.statusBar().showMessage("程序停下来了，可以按“哪里错了”看看提示。")

    def stop_process(self) -> None:
        if self.process and self.process.state() != QProcess.ProcessState.NotRunning:
            if self.debugging:
                self.process.write(b"stop\n")
            self.process.kill()
            self.process.waitForFinished(1500)
            self.output.append("\n已经停下。")
        self.process = None
        self.debugging = False
        self.stderr_buffer = ""
        self.output.set_running(False)
        self.stop_action.setEnabled(False)
        self._set_debug_controls_enabled(False)
        self._clear_debug_positions()

    def show_local_error_help(self) -> None:
        friendly = parse_friendly_error(self.last_error_text)
        if not friendly:
            self.output.show_message("现在还没有发现错误。可以先按“运行”，如果程序停下来了，再点这里。")
            return
        self._show_problem(friendly, switch_to_tab=True)
        self.output.show_message(format_friendly_error(friendly))

    def _set_debug_controls_enabled(self, enabled: bool) -> None:
        self.debug_continue_action.setEnabled(enabled)
        self.debug_step_action.setEnabled(enabled)
        self.debug_panel.set_debugging(enabled)

    def _clear_debug_positions(self) -> None:
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor):
                editor.clear_debug_position()

    def find_text_dialog(self) -> None:
        query, ok = QInputDialog.getText(
            self,
            "查找文字",
            "想找哪几个字？",
            text=self.last_find_text,
        )
        if not ok:
            return
        self.last_find_text = query
        if not self.editor.find_text(query):
            self.statusBar().showMessage("没有找到这几个字。")
            return
        self.statusBar().showMessage("找到了。再按 Ctrl+F 可以继续查。")

    def goto_definition(self) -> None:
        editor = self.editor
        if editor.current_path is None:
            self.statusBar().showMessage("先保存成 .py 文件，再跳到定义。")
            return
        script = self._jedi_script(editor)
        if script is None:
            return
        line, col = editor.getCursorPosition()
        try:
            targets = script.goto(line + 1, col, follow_imports=True)
        except Exception:
            self.statusBar().showMessage("这次没有找到定义。")
            return
        target = self._first_project_name(targets)
        if target is None:
            self.statusBar().showMessage("这次没有找到定义。")
            return
        self.open_location(str(target.module_path), int(target.line or 1), int(target.column or 0))

    def find_references(self) -> None:
        editor = self.editor
        if editor.current_path is None:
            self.statusBar().showMessage("先保存成 .py 文件，再查找引用。")
            return
        script = self._jedi_script(editor)
        if script is None:
            return
        line, col = editor.getCursorPosition()
        try:
            names = script.get_references(line + 1, col, include_builtins=False)
        except Exception:
            self.statusBar().showMessage("这次没有找到引用。")
            return
        rows = []
        seen: set[tuple[str, int, int]] = set()
        for name in names:
            if not name.module_path or not name.line:
                continue
            path = Path(name.module_path).resolve()
            if not self._inside_current_folder(path):
                continue
            key = (str(path), int(name.line), int(name.column or 0))
            if key in seen:
                continue
            seen.add(key)
            rows.append(
                {
                    "path": str(path),
                    "line": int(name.line),
                    "message": self._line_preview(path, int(name.line)),
                }
            )
        if not rows:
            self.references_panel.clear("没有在这个项目里找到引用。")
            self.bottom_tabs.setCurrentWidget(self.references_panel)
            return
        self.references_panel.set_rows(rows, f"找到 {len(rows)} 个位置，双击可以跳过去。")
        self.bottom_tabs.setCurrentWidget(self.references_panel)

    def _jedi_script(self, editor: CodeEditor):
        try:
            import jedi
        except Exception:
            self.statusBar().showMessage("代码跳转需要 Jedi。请确认依赖已经安装。")
            return None
        try:
            project = jedi.Project(str(self.current_folder.resolve()))
            return jedi.Script(
                editor.text(),
                path=str(editor.current_path) if editor.current_path else None,
                project=project,
            )
        except Exception:
            self.statusBar().showMessage("代码分析暂时没有准备好。")
            return None

    def _first_project_name(self, names):
        for name in names:
            if not name.module_path or not name.line:
                continue
            path = Path(name.module_path).resolve()
            if self._inside_current_folder(path):
                return name
        return None

    def _inside_current_folder(self, path: Path) -> bool:
        try:
            path.resolve().relative_to(self.current_folder.resolve())
            return True
        except ValueError:
            return False

    def _line_preview(self, path: Path, line: int) -> str:
        try:
            lines = path.read_text(encoding="utf-8", errors="replace").splitlines()
        except OSError:
            return ""
        if 1 <= line <= len(lines):
            return lines[line - 1].strip()
        return ""

    def open_location(self, path_text: str, line: int, column: int = 0) -> Path:
        path = self._resolve_location_path(path_text)
        if path.exists():
            self.open_file(path)
        elif self.editor.current_path is not None:
            QMessageBox.information(self, "找不到文件", f"这个位置的文件暂时找不到：\n{path}")
        zero_line = max(0, line - 1)
        self.editor.setCursorPosition(zero_line, max(0, column))
        self.editor.ensureLineVisible(zero_line)
        self.editor.setFocus()
        return path

    def _resolve_location_path(self, path_text: str) -> Path:
        path = Path(path_text)
        if not path.is_absolute():
            path = self.process_cwd / path
        return path.resolve()

    def check_current_syntax(self) -> None:
        editor = self.editor
        editor.clear_error_markers()
        if editor.current_path and editor.current_path.suffix.lower() != ".py":
            self.problems_panel.clear("这个文件不是 Python 文件，不做语法提醒。")
            return
        source = editor.text()
        if not source.strip():
            self.problems_panel.clear("现在没有发现问题。")
            return
        filename = str(editor.current_path or Path("新文件.py"))
        try:
            compile(source, filename, "exec")
        except SyntaxError as exc:
            line = exc.lineno or 1
            editor.mark_error_line(line)
            message = exc.msg or "这里的写法还没拼完整"
            self.problems_panel.set_rows(
                [{"path": filename, "line": line, "message": message}],
                "发现 1 个小问题，双击可以跳到那一行。",
            )
            self.statusBar().showMessage(f"报错提醒：第 {line} 行可能需要看看。")
            return
        self.problems_panel.clear("现在没有发现问题。")
        self._update_current_status()

    def _show_problem(self, friendly, switch_to_tab: bool = False) -> None:
        path_text = friendly.file or str(self.current_file or Path("新文件.py"))
        line = friendly.line or 1
        path = self._resolve_location_path(path_text)
        if path.exists() or self.current_file is None:
            self.open_location(str(path), line)
        self.editor.mark_error_line(line)
        self.problems_panel.set_rows(
            [{"path": str(path), "line": line, "message": friendly.title}],
            "发现 1 个让程序停下来的问题，双击可以跳到那一行。",
        )
        if switch_to_tab:
            self.bottom_tabs.setCurrentWidget(self.problems_panel)

    def set_focus_mode(self, enabled: bool) -> None:
        if enabled == self.focus_mode:
            return
        if enabled:
            self.ai_visible_before_focus = not self.ai_panel.isHidden()
        self.focus_mode = enabled
        if self.project_pane is not None:
            self.project_pane.setVisible(not enabled)
        self.tree.setVisible(not enabled)
        self.bottom_tabs.setVisible(not enabled)
        self.ai_panel.setVisible(False if enabled else self.ai_visible_before_focus)
        if self.main_splitter is not None:
            ai_width = 320 if self.ai_visible_before_focus and not enabled else 0
            self.main_splitter.setSizes([0, 1300, 0] if enabled else [190, 1160, ai_width])
        self.editor.setFocus()

    def _set_hints_enabled(self, enabled: bool) -> None:
        self.settings.code_hints_enabled = enabled
        for idx in range(self.editor_tabs.count()):
            editor = self.editor_tabs.widget(idx)
            if isinstance(editor, CodeEditor):
                editor.set_code_hints_enabled(enabled)
        save_settings(self.settings)
        state = "打开" if enabled else "关闭"
        self.statusBar().showMessage(f"代码提示已{state}。")

    def open_ai_settings(self) -> None:
        dialog = AiSettingsDialog(self.settings, self)
        if dialog.exec() == QDialog.DialogCode.Accepted:
            dialog.apply_to(self.settings)
            save_settings(self.settings)
            self.statusBar().showMessage(f"AI 设置已保存，Key：{masked_api_key(self.settings.api_key)}")

    def _ask_ai(self, question: str) -> None:
        code = self.editor.text()
        self._run_ai(
            [
                AiMessage(
                    "user",
                    f"孩子的问题：{question}\n\n当前代码：\n```python\n{code}\n```",
                )
            ]
        )

    def _explain_selection(self) -> None:
        code = self.editor.selected_or_all_text()
        self._run_ai(explain_selection_messages(code))

    def _explain_error(self) -> None:
        if not self.last_error_text:
            self.ai_panel.append_answer("现在还没有错误。先运行一次，如果停下来了我再帮你看。")
            return
        self._run_ai(explain_error_messages(self.editor.text(), self.last_error_text))

    def _ask_hint(self) -> None:
        self._run_ai(hint_messages(self.editor.text()))

    def _run_ai(self, messages) -> None:
        if self.ai_worker and self.ai_worker.isRunning():
            return
        self.ai_panel.set_busy(True)
        self.ai_worker = AiWorker(self.settings, messages)
        self.ai_worker.answer_ready.connect(self._ai_answer_ready)
        self.ai_worker.error_ready.connect(self._ai_error_ready)
        self.ai_worker.finished.connect(lambda: self.ai_panel.set_busy(False))
        self.ai_worker.start()

    def _ai_answer_ready(self, answer: str) -> None:
        self.ai_panel.append_answer(f"小老师：{answer}")

    def _ai_error_ready(self, error: str) -> None:
        self.ai_panel.append_answer(error)

    def dragEnterEvent(self, event: QDragEnterEvent) -> None:
        if event.mimeData().hasUrls():
            event.acceptProposedAction()

    def dropEvent(self, event: QDropEvent) -> None:
        urls = event.mimeData().urls()
        if not urls:
            return
        path = Path(urls[0].toLocalFile())
        if path.is_dir():
            self._open_folder(path)
        elif path.is_file():
            self._open_folder(path.parent)
            if path.suffix.lower() in {".py", ".txt", ".md"}:
                self.open_file(path)

    def closeEvent(self, event: QCloseEvent) -> None:
        if self._maybe_save_all():
            self.stop_process()
            event.accept()
        else:
            event.ignore()

    def _show_about(self) -> None:
        QMessageBox.information(
            self,
            "关于",
            f"{APP_DISPLAY_NAME}\n\n给孩子使用的 Python 创作工具。\nAI Key 只保存在这台电脑的用户设置里。",
        )


def create_window() -> MainWindow:
    app = QApplication.instance()
    if app is None:
        raise RuntimeError("QApplication must be created before MainWindow")
    return MainWindow()
