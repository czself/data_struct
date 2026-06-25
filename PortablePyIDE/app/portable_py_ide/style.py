from __future__ import annotations


APP_STYLESHEET = """
QMainWindow {
    background: #f3f7fb;
}
QWidget {
    color: #1d3345;
    font-family: "Microsoft YaHei", "Segoe UI", sans-serif;
    font-size: 14px;
}
QMenuBar {
    background: #ffffff;
    border-bottom: 1px solid #dde8f0;
    padding: 3px 8px;
}
QMenuBar::item {
    background: transparent;
    border-radius: 6px;
    padding: 6px 10px;
}
QMenuBar::item:selected {
    background: #e7f6f2;
    color: #0f6658;
}
QMenu {
    background: #ffffff;
    border: 1px solid #cadce8;
    padding: 6px;
}
QMenu::item {
    border-radius: 6px;
    padding: 7px 28px 7px 12px;
}
QMenu::item:selected {
    background: #e7f6f2;
    color: #0f6658;
}
QToolBar#MainToolbar {
    background: #ffffff;
    border: 0;
    border-bottom: 1px solid #d9e7f0;
    spacing: 4px;
    padding: 4px 8px;
}
QToolBar#MainToolbar::separator {
    background: #d7e4ed;
    width: 1px;
    margin: 6px 6px;
}
QFrame#ToolbarActionButton {
    background: #f8fbfd;
    border: 1px solid #cfe0ea;
    border-radius: 7px;
}
QFrame#ToolbarActionButton:hover {
    background: #edf8fb;
    border-color: #9ed2df;
}
QFrame#ToolbarActionButton:disabled {
    background: #f1f5f8;
    border-color: #dce6ed;
}
QFrame#ToolbarActionButton[role="run"] {
    background: #e7f7ee;
    border-color: #8fd4ad;
}
QFrame#ToolbarActionButton[role="run"]:hover {
    background: #d6f2e2;
}
QFrame#ToolbarActionButton[role="stop"] {
    background: #fff3f1;
    border-color: #efb1aa;
}
QFrame#ToolbarActionButton[role="warn"] {
    background: #fff7df;
    border-color: #efd179;
}
QFrame#ToolbarActionButton[role="hint"][checked="true"] {
    background: #e1f7eb;
    border-color: #73ce9a;
}
QFrame#PaneFrame {
    background: #f3f7fb;
    border: 0;
}
QLabel#PaneTitle {
    color: #31566d;
    font-size: 13px;
    font-weight: 800;
    letter-spacing: 0;
    padding: 2px 4px 4px 4px;
}
QLabel#SoftHint {
    color: #627587;
    padding: 2px 4px 6px 4px;
}
QTreeView#ProjectTree {
    background: #ffffff;
    border: 1px solid #d4e3ec;
    border-radius: 7px;
    padding: 6px 4px;
    selection-background-color: #d9f1f7;
    selection-color: #15384d;
    alternate-background-color: #f8fbfd;
}
QTreeView#ProjectTree::item {
    min-height: 24px;
    border-radius: 5px;
    padding: 2px 4px;
}
QTreeView#ProjectTree::item:hover {
    background: #eef8fb;
}
QTreeView#ProjectTree::item:selected {
    background: #d9f1f7;
    color: #12384d;
}
QTabWidget#EditorTabs::pane {
    background: #ffffff;
    border: 1px solid #cfe0ea;
    border-top: 0;
}
QTabWidget#BottomTabs::pane {
    background: #ffffff;
    border: 1px solid #d4e3ec;
    border-radius: 0;
    top: -1px;
}
QTabBar::tab {
    background: #e9f2f7;
    color: #2b4f64;
    border: 1px solid #c8dce8;
    border-bottom: 0;
    border-top-left-radius: 7px;
    border-top-right-radius: 7px;
    padding: 8px 13px;
    margin-right: 3px;
    min-width: 92px;
    font-weight: 700;
}
QTabBar::tab:selected {
    background: #ffffff;
    color: #0b6e84;
    border-color: #8ecbda;
}
QTabBar::tab:!selected:hover {
    background: #f5fbfd;
    color: #143c50;
}
QTabBar::close-button {
    subcontrol-position: right;
    margin-left: 7px;
}
QTabWidget#BottomTabs QTabBar::tab {
    min-width: 88px;
    padding: 7px 13px;
    background: #edf3f7;
    color: #456478;
}
QTabWidget#BottomTabs QTabBar::tab:selected {
    background: #ffffff;
    color: #0f6658;
    border-color: #9ad8c8;
}
QTextEdit, QLineEdit, QPlainTextEdit, QTableWidget {
    background: #ffffff;
    border: 1px solid #d4e3ec;
    border-radius: 7px;
    selection-background-color: #bfe8fb;
    selection-color: #102a3a;
}
QTextEdit {
    padding: 8px;
}
QLineEdit {
    padding: 8px;
}
QLineEdit#ProcessInput {
    background: #fafffc;
    border-color: #a8dbc2;
}
QLineEdit#ProcessInput:focus {
    border-color: #43a56f;
}
QHeaderView::section {
    background: #eef5f8;
    color: #36566a;
    border: 0;
    border-right: 1px solid #d4e3ec;
    border-bottom: 1px solid #d4e3ec;
    padding: 6px;
    font-weight: 700;
}
QTableWidget {
    gridline-color: #e4edf3;
}
QTableWidget::item {
    padding: 5px;
}
QTableWidget::item:selected {
    background: #d9f1f7;
    color: #12384d;
}
QPushButton {
    background: #f8fbfd;
    border: 1px solid #cfe0ea;
    border-radius: 8px;
    padding: 7px 12px;
    color: #18465c;
    font-weight: 700;
}
QPushButton:hover {
    background: #edf8fb;
    border-color: #9ed2df;
}
QPushButton:pressed {
    background: #d8f0f6;
}
QPushButton:disabled {
    background: #f1f5f8;
    border-color: #dce6ed;
    color: #9aa9b5;
}
QSplitter::handle {
    background: #dce8f0;
}
QSplitter#MainSplitter::handle {
    width: 4px;
}
QSplitter#VerticalSplitter::handle {
    height: 4px;
}
QStatusBar {
    background: #ffffff;
    border-top: 1px solid #d9e7f0;
    color: #486579;
}
QScrollBar:vertical {
    background: #f3f7fb;
    width: 12px;
    margin: 0;
}
QScrollBar::handle:vertical {
    background: #c8d9e4;
    border-radius: 6px;
    min-height: 28px;
}
QScrollBar::handle:vertical:hover {
    background: #9fb8c8;
}
QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
    height: 0;
}
QScrollBar:horizontal {
    background: #f3f7fb;
    height: 12px;
    margin: 0;
}
QScrollBar::handle:horizontal {
    background: #c8d9e4;
    border-radius: 6px;
    min-width: 28px;
}
QScrollBar::handle:horizontal:hover {
    background: #9fb8c8;
}
QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
    width: 0;
}
QDialog {
    background: #f3f7fb;
}
"""
