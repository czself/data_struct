from __future__ import annotations

from pathlib import Path

APP_NAME = "PortablePyIDE"
APP_DISPLAY_NAME = "小小 Python 创作室"
ORG_NAME = "PortablePyIDE"

PACKAGE_DIR = Path(__file__).resolve().parent
APP_DIR = PACKAGE_DIR.parent
PROJECT_OR_RELEASE_ROOT = APP_DIR.parent
RUNTIME_PYTHON_DIR = PROJECT_OR_RELEASE_ROOT / "runtime" / "python"

DEFAULT_BASE_URL = "https://api.deepseek.com"
DEFAULT_MODEL = "deepseek-v4-flash"
LEGACY_DEFAULT_BASE_URL = "https://api.openai.com"
LEGACY_DEFAULT_MODEL = "gpt-4.1-mini"

EXAMPLE_CODE = """# 欢迎来到小小 Python 创作室
# 按上面的“运行”看看会发生什么。

name = input("你叫什么名字？")
print("你好，" + name + "！")
print("今天也可以写出很棒的程序。")
"""
