from __future__ import annotations

import re
from dataclasses import dataclass

FILE_LINE_RE = re.compile(r'File "([^"]+)", line (\d+)')
ERROR_NAME_RE = re.compile(r"^\s*([A-Za-z_][A-Za-z0-9_]*(?:Error|Exception)):\s*(.*)$")


@dataclass(frozen=True)
class FriendlyError:
    line: int | None
    title: str
    why: str
    try_this: str
    original: str
    file: str | None = None


ERROR_HINTS: dict[str, tuple[str, str, str]] = {
    "SyntaxError": (
        "这一行的写法还没拼完整",
        "Python 读到这里时发现格式不对，常见原因是少了括号、引号、逗号或冒号。",
        "先检查这一行和上一行：括号有没有成对出现？字符串两边有没有引号？if/for/def 后面有没有冒号？",
    ),
    "IndentationError": (
        "缩进有点不整齐",
        "Python 用空格判断代码属于哪一层。这里的空格数量可能和前后不一致。",
        "看看这一行前面的空格，尽量每一层都用 4 个空格，不要混用 Tab 和空格。",
    ),
    "NameError": (
        "这个名字还没有准备好",
        "程序用到了一个变量或函数名，但 Python 还不知道它是什么。",
        "检查名字有没有拼错，或者是不是忘了先写赋值，例如 name = \"小明\"。",
    ),
    "TypeError": (
        "这里的东西类型不太合适",
        "程序把数字、文字、列表等不同类型混在一起用了。",
        "看看报错行附近，是不是把数字和文字直接相加了。可以试试 str()、int() 或 float()。",
    ),
    "ValueError": (
        "这个值不适合放进去",
        "函数收到了一个它看不懂的值，比如把不是数字的文字交给 int()。",
        "检查输入内容，或者先用 if 判断它是不是符合要求。",
    ),
    "ZeroDivisionError": (
        "不能除以 0",
        "数学里除数不能是 0，Python 也会停下来提醒你。",
        "运行除法前，先判断除数是不是 0。",
    ),
    "ModuleNotFoundError": (
        "没有找到这个工具包",
        "代码里 import 了一个当前 Python 里没有的模块。",
        "检查模块名有没有拼错。发布包内置的是基础 Python，额外工具包需要老师或家长帮忙加入。",
    ),
    "EOFError": (
        "程序在等输入，但没有等到",
        "input() 想让你输入内容，可是输入被提前结束了。",
        "运行时在底部输入框里打字，再按 Enter 发送给程序。",
    ),
}


def parse_friendly_error(output: str) -> FriendlyError | None:
    output = output.strip()
    if not output:
        return None

    line = None
    file_name = None
    for match in FILE_LINE_RE.finditer(output):
        file_name = match.group(1)
        line = int(match.group(2))

    error_name = ""
    message = ""
    for raw_line in reversed(output.splitlines()):
        match = ERROR_NAME_RE.match(raw_line)
        if match:
            error_name = match.group(1)
            message = match.group(2).strip()
            break

    if not error_name and "SyntaxError" in output:
        error_name = "SyntaxError"

    title, why, try_this = ERROR_HINTS.get(
        error_name,
        (
            "程序停在这里了",
            "Python 遇到了一个它不能继续处理的问题。",
            "从标记的行开始读，再看上一行。通常问题就在这两行附近。",
        ),
    )
    if message:
        why = f"{why}\nPython 的原话是：{message}"

    return FriendlyError(
        line=line,
        title=title,
        why=why,
        try_this=try_this,
        original=output,
        file=file_name,
    )


def format_friendly_error(error: FriendlyError) -> str:
    if error.file and error.line:
        line_text = f"{error.file} 第 {error.line} 行"
    else:
        line_text = f"第 {error.line} 行" if error.line else "某一行"
    return (
        f"哪里错了：{line_text}\n"
        f"{error.title}\n\n"
        f"为什么：\n{error.why}\n\n"
        f"可以这样试试：\n{error.try_this}\n\n"
        "原始信息（给老师或家长看）：\n"
        f"{error.original}"
    )
