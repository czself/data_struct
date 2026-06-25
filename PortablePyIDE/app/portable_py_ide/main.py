from __future__ import annotations

import sys

from PyQt6.QtWidgets import QApplication

from .constants import APP_DISPLAY_NAME, ORG_NAME
from .main_window import MainWindow
from .style import APP_STYLESHEET


def main() -> int:
    app = QApplication(sys.argv)
    app.setApplicationName(APP_DISPLAY_NAME)
    app.setOrganizationName(ORG_NAME)
    app.setStyleSheet(APP_STYLESHEET)
    window = MainWindow()
    window.show()
    return app.exec()
