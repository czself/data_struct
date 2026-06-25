from __future__ import annotations

from pathlib import Path

from PyQt6.QtCore import QSize, Qt, pyqtSignal
from PyQt6.QtGui import QColor, QFont
from PyQt6.Qsci import QsciLexerPython, QsciScintilla
from PyQt6.QtWidgets import QAbstractItemView, QListWidget


class CodeEditor(QsciScintilla):
    ERROR_MARKER = 1
    BREAKPOINT_MARKER = 2
    CURRENT_LINE_MARKER = 3
    MIN_FONT_SIZE = 11
    MAX_FONT_SIZE = 26
    breakpoints_changed = pyqtSignal(object)

    def __init__(self, parent=None) -> None:
        super().__init__(parent)
        self.current_path: Path | None = None
        self.code_hints_enabled = True
        self.font_size = 16
        self.breakpoints: set[int] = set()
        self._completion_prefix = ""
        self._completion_popup: QListWidget | None = None
        self._lexer: QsciLexerPython | None = None
        self._setup_editor()

    def _setup_editor(self) -> None:
        font = self._code_font()
        font.setFixedPitch(True)
        font.setStyleStrategy(QFont.StyleStrategy.PreferAntialias)
        self.setFont(font)
        self.setMarginsFont(font)
        self.setUtf8(True)
        self.setIndentationsUseTabs(False)
        self.setIndentationWidth(4)
        self.setTabWidth(4)
        self.setAutoIndent(True)
        self.setBraceMatching(QsciScintilla.BraceMatch.SloppyBraceMatch)
        self.setCaretLineVisible(True)
        self.setCaretLineBackgroundColor(QColor("#fff7d6"))
        self.setSelectionBackgroundColor(QColor("#9edcff"))
        self.setPaper(QColor("#ffffff"))
        self.setColor(QColor("#111827"))
        self.setCaretForegroundColor(QColor("#111827"))
        self.setMargins(8)

        self.setMarginType(0, QsciScintilla.MarginType.NumberMargin)
        self.setMarginWidth(0, "00000")
        self.setMarginSensitivity(0, True)
        self.setMarginsForegroundColor(QColor("#334155"))
        self.setMarginsBackgroundColor(QColor("#eaf6ff"))
        self.marginClicked.connect(self._margin_clicked)

        self.markerDefine(QsciScintilla.MarkerSymbol.RightTriangle, self.ERROR_MARKER)
        self.setMarkerBackgroundColor(QColor("#ff6b62"), self.ERROR_MARKER)
        self.markerDefine(QsciScintilla.MarkerSymbol.Circle, self.BREAKPOINT_MARKER)
        self.setMarkerBackgroundColor(QColor("#dc2626"), self.BREAKPOINT_MARKER)
        self.markerDefine(QsciScintilla.MarkerSymbol.RightArrow, self.CURRENT_LINE_MARKER)
        self.setMarkerBackgroundColor(QColor("#facc15"), self.CURRENT_LINE_MARKER)

        lexer = QsciLexerPython(self)
        lexer.setDefaultFont(font)
        lexer.setDefaultColor(QColor("#111827"))
        lexer.setDefaultPaper(QColor("#ffffff"))
        for style in range(128):
            lexer.setFont(font, style)
            lexer.setPaper(QColor("#ffffff"), style)
        lexer.setColor(QColor("#111827"), QsciLexerPython.Default)
        lexer.setColor(QColor("#5b21b6"), QsciLexerPython.Keyword)
        lexer.setColor(QColor("#166534"), QsciLexerPython.Comment)
        lexer.setColor(QColor("#9a3412"), QsciLexerPython.DoubleQuotedString)
        lexer.setColor(QColor("#9a3412"), QsciLexerPython.SingleQuotedString)
        lexer.setColor(QColor("#075985"), QsciLexerPython.FunctionMethodName)
        lexer.setColor(QColor("#7e22ce"), QsciLexerPython.ClassName)
        lexer.setColor(QColor("#be123c"), QsciLexerPython.Number)
        lexer.setColor(QColor("#334155"), QsciLexerPython.Operator)
        lexer.setColor(QColor("#111827"), QsciLexerPython.Identifier)
        self.setLexer(lexer)
        self._lexer = lexer

        self.setAutoCompletionThreshold(1)
        self.setAutoCompletionCaseSensitivity(False)
        self.setAutoCompletionReplaceWord(False)
        self._setup_completion_popup()

    def _setup_completion_popup(self) -> None:
        popup = QListWidget(self)
        popup.setObjectName("CompletionPopup")
        popup.setFocusPolicy(Qt.FocusPolicy.NoFocus)
        popup.setMouseTracking(True)
        popup.setUniformItemSizes(True)
        popup.setHorizontalScrollBarPolicy(Qt.ScrollBarPolicy.ScrollBarAlwaysOff)
        popup.setVerticalScrollMode(QAbstractItemView.ScrollMode.ScrollPerPixel)
        popup.setSelectionMode(QAbstractItemView.SelectionMode.SingleSelection)
        popup.itemClicked.connect(lambda item: self._insert_completion_text(item.text()))
        popup.setStyleSheet(
            """
            QListWidget#CompletionPopup {
                background: #ffffff;
                color: #0f172a;
                border: 2px solid #7dd3fc;
                border-radius: 8px;
                padding: 4px;
                font-size: 15px;
                outline: 0;
            }
            QListWidget#CompletionPopup::item {
                color: #0f172a;
                min-height: 28px;
                padding: 4px 10px;
                border-radius: 6px;
            }
            QListWidget#CompletionPopup::item:selected {
                background: #d9f99d;
                color: #0f172a;
            }
            QListWidget#CompletionPopup::item:hover {
                background: #e0f2fe;
                color: #0f172a;
            }
            """
        )
        popup.hide()
        self._completion_popup = popup

    def _code_font(self) -> QFont:
        font = QFont("Consolas", self.font_size)
        font.setFixedPitch(True)
        font.setStyleStrategy(QFont.StyleStrategy.PreferAntialias)
        return font

    def set_font_size(self, size: int) -> None:
        self.font_size = max(self.MIN_FONT_SIZE, min(self.MAX_FONT_SIZE, size))
        font = self._code_font()
        self.setFont(font)
        self.setMarginsFont(font)
        if self._lexer is not None:
            self._lexer.setDefaultFont(font)
            for style in range(128):
                self._lexer.setFont(font, style)
        self.setMarginWidth(0, "00000")

    def increase_font_size(self) -> None:
        self.set_font_size(self.font_size + 1)

    def decrease_font_size(self) -> None:
        self.set_font_size(self.font_size - 1)

    def set_code_hints_enabled(self, enabled: bool) -> None:
        self.code_hints_enabled = enabled

    def set_current_path(self, path: Path | None) -> None:
        self.current_path = path

    def clear_error_markers(self) -> None:
        self.markerDeleteAll(self.ERROR_MARKER)

    def mark_error_line(self, line: int | None) -> None:
        self.clear_error_markers()
        if line and line > 0:
            self.markerAdd(line - 1, self.ERROR_MARKER)
            self.ensureLineVisible(line - 1)
            self.setCursorPosition(line - 1, 0)

    def clear_debug_position(self) -> None:
        self.markerDeleteAll(self.CURRENT_LINE_MARKER)

    def set_debug_position(self, line: int | None) -> None:
        self.clear_debug_position()
        if line and line > 0:
            zero_line = line - 1
            self.markerAdd(zero_line, self.CURRENT_LINE_MARKER)
            self.ensureLineVisible(zero_line)
            self.setCursorPosition(zero_line, 0)

    def toggle_breakpoint(self, line: int) -> None:
        if line < 1:
            return
        zero_line = line - 1
        if line in self.breakpoints:
            self.breakpoints.remove(line)
            self.markerDelete(zero_line, self.BREAKPOINT_MARKER)
        else:
            self.breakpoints.add(line)
            self.markerAdd(zero_line, self.BREAKPOINT_MARKER)
        self.breakpoints_changed.emit(sorted(self.breakpoints))

    def set_breakpoints(self, lines: set[int]) -> None:
        self.markerDeleteAll(self.BREAKPOINT_MARKER)
        self.breakpoints = {line for line in lines if line > 0}
        for line in sorted(self.breakpoints):
            self.markerAdd(line - 1, self.BREAKPOINT_MARKER)
        self.breakpoints_changed.emit(sorted(self.breakpoints))

    def _margin_clicked(self, margin: int, line: int, _state) -> None:
        if margin == 0:
            self.toggle_breakpoint(line + 1)

    def selected_or_all_text(self) -> str:
        selected = self.selectedText()
        if selected:
            return selected.replace("\u2029", "\n")
        return self.text()

    def keyPressEvent(self, event) -> None:  # noqa: N802 - Qt API
        if self._handle_completion_popup_key(event):
            return
        if (
            event.key() == Qt.Key.Key_Space
            and event.modifiers() & Qt.KeyboardModifier.ControlModifier
        ):
            self.show_jedi_completions()
            return
        is_return = event.key() in {Qt.Key.Key_Return, Qt.Key.Key_Enter}
        if self._handle_auto_pair_key(event):
            return
        super().keyPressEvent(event)
        if is_return:
            self._indent_after_colon()
            return
        if not self.code_hints_enabled:
            return
        if event.modifiers() & Qt.KeyboardModifier.ControlModifier:
            return
        text = event.text()
        if text and (text.isalnum() or text in "._"):
            self.show_jedi_completions()
        elif text:
            self.hide_completion_popup()

    def focusOutEvent(self, event) -> None:  # noqa: N802 - Qt API
        self.hide_completion_popup()
        super().focusOutEvent(event)

    def mousePressEvent(self, event) -> None:  # noqa: N802 - Qt API
        self.hide_completion_popup()
        super().mousePressEvent(event)

    def _handle_auto_pair_key(self, event) -> bool:
        if event.modifiers() & (
            Qt.KeyboardModifier.ControlModifier | Qt.KeyboardModifier.AltModifier
        ):
            return False
        text = event.text()
        pairs = {"(": ")", "[": "]", "{": "}", '"': '"', "'": "'"}
        if text in pairs:
            close = pairs[text]
            if self.hasSelectedText():
                selected = self.selectedText().replace("\u2029", "\n")
                self.replaceSelectedText(f"{text}{selected}{close}")
                return True
            line, col = self.getCursorPosition()
            self.insert(text + close)
            self.setCursorPosition(line, col + 1)
            return True
        if text in {
            ")",
            "]",
            "}",
            '"',
            "'",
        } and self._cursor_char() == text:
            line, col = self.getCursorPosition()
            self.setCursorPosition(line, col + 1)
            return True
        return False

    def _cursor_char(self) -> str:
        line, col = self.getCursorPosition()
        line_text = self.text(line)
        if 0 <= col < len(line_text):
            return line_text[col]
        return ""

    def _indent_after_colon(self) -> None:
        line, col = self.getCursorPosition()
        if line <= 0:
            return
        previous = self.text(line - 1).rstrip("\r\n")
        if not previous.rstrip().endswith(":"):
            return
        current = self.text(line).rstrip("\r\n")
        if current.strip():
            return
        self.insert(" " * self.indentationWidth())
        self.setCursorPosition(line, col + self.indentationWidth())

    def toggle_comment(self) -> None:
        start_line, end_line = self._selected_line_range()
        lines = [self.text(line).rstrip("\r\n") for line in range(start_line, end_line + 1)]
        non_empty = [line for line in lines if line.strip()]
        should_uncomment = bool(non_empty) and all(
            line[line.find(line.lstrip()) :].startswith("#") for line in non_empty
        )

        for line in range(start_line, end_line + 1):
            text = self.text(line).rstrip("\r\n")
            if should_uncomment:
                self._uncomment_line(line, text)
            else:
                self._comment_line(line, text)

    def _selected_line_range(self) -> tuple[int, int]:
        if not self.hasSelectedText():
            line, _col = self.getCursorPosition()
            return line, line
        start_line, _start_col, end_line, end_col = self.getSelection()
        if end_col == 0 and end_line > start_line:
            end_line -= 1
        return start_line, end_line

    def _comment_line(self, line: int, text: str) -> None:
        indent = len(text) - len(text.lstrip(" "))
        self.setSelection(line, indent, line, indent)
        self.replaceSelectedText("# ")

    def _uncomment_line(self, line: int, text: str) -> None:
        indent = len(text) - len(text.lstrip(" "))
        if text[indent : indent + 2] == "# ":
            end = indent + 2
        elif text[indent : indent + 1] == "#":
            end = indent + 1
        else:
            return
        self.setSelection(line, indent, line, end)
        self.replaceSelectedText("")

    def find_text(self, query: str, forward: bool = True) -> bool:
        if not query:
            return False
        return self.findFirst(query, False, False, False, True, forward)

    def find_next_text(self, query: str, forward: bool = True) -> bool:
        if not query:
            return False
        if self.findNext():
            return True
        return self.find_text(query, forward)

    def show_jedi_completions(self) -> None:
        try:
            import jedi
        except Exception:
            return

        line, index = self.getCursorPosition()
        try:
            script = jedi.Script(
                self.text(),
                path=str(self.current_path) if self.current_path else None,
            )
            completions = script.complete(line + 1, index)
        except Exception:
            return

        names = []
        for item in completions[:20]:
            label = item.name
            if item.type == "function":
                label = f"{item.name}()"
            elif item.type == "class":
                label = f"{item.name} 类"
            elif item.type == "module":
                label = f"{item.name} 模块"
            if label not in names:
                names.append(label)
        if not names:
            self.hide_completion_popup()
            return

        self._completion_prefix = self._prefix_before_cursor()
        self._show_completion_popup(names)

    def _show_completion_popup(self, names: list[str]) -> None:
        popup = self._completion_popup
        if popup is None:
            return

        popup.clear()
        font = self._code_font()
        popup.setFont(font)
        metrics = popup.fontMetrics()
        row_height = max(30, metrics.height() + 10)
        max_text_width = max(metrics.horizontalAdvance(name) for name in names)
        width = min(420, max(260, max_text_width + 42))
        visible_rows = min(8, len(names))
        height = visible_rows * row_height + 12

        for name in names:
            popup.addItem(name)
            item = popup.item(popup.count() - 1)
            if item is not None:
                item.setSizeHint(QSize(width - 16, row_height))

        popup.setCurrentRow(0)
        popup.resize(width, height)
        popup.move(*self._completion_popup_position(width, height))
        popup.raise_()
        popup.show()

    def _completion_popup_position(self, width: int, height: int) -> tuple[int, int]:
        line, index = self.getCursorPosition()
        position = self.positionFromLineIndex(line, index)
        x = int(self.SendScintilla(QsciScintilla.SCI_POINTXFROMPOSITION, 0, position))
        y = int(self.SendScintilla(QsciScintilla.SCI_POINTYFROMPOSITION, 0, position))
        y += self.fontMetrics().height() + 8

        max_x = max(8, self.width() - width - 12)
        max_y = max(8, self.height() - height - 12)
        return min(max(8, x), max_x), min(max(8, y), max_y)

    def hide_completion_popup(self) -> None:
        if self._completion_popup is not None:
            self._completion_popup.hide()

    def _handle_completion_popup_key(self, event) -> bool:
        popup = self._completion_popup
        if popup is None or popup.isHidden():
            return False

        key = event.key()
        if key == Qt.Key.Key_Escape:
            self.hide_completion_popup()
            return True
        if key in {Qt.Key.Key_Return, Qt.Key.Key_Enter, Qt.Key.Key_Tab}:
            item = popup.currentItem()
            if item is not None:
                self._insert_completion_text(item.text())
            return True
        if key == Qt.Key.Key_Down:
            popup.setCurrentRow(min(popup.count() - 1, popup.currentRow() + 1))
            return True
        if key == Qt.Key.Key_Up:
            popup.setCurrentRow(max(0, popup.currentRow() - 1))
            return True
        if key == Qt.Key.Key_PageDown:
            popup.setCurrentRow(min(popup.count() - 1, popup.currentRow() + 6))
            return True
        if key == Qt.Key.Key_PageUp:
            popup.setCurrentRow(max(0, popup.currentRow() - 6))
            return True
        return False

    def _prefix_before_cursor(self) -> str:
        line, index = self.getCursorPosition()
        text = self.text(line)[:index]
        end = len(text)
        start = end
        while start > 0 and (text[start - 1].isalnum() or text[start - 1] == "_"):
            start -= 1
        return text[start:end]

    def _insert_user_completion(self, _list_id: int, raw_text: str) -> None:
        self._insert_completion_text(raw_text)

    def _insert_completion_text(self, raw_text: str) -> None:
        name = raw_text.split()[0]
        prefix = self._completion_prefix
        if prefix and name.startswith(prefix):
            self.insert(name[len(prefix) :])
        else:
            self.insert(name)
        self.hide_completion_popup()
