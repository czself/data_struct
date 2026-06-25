# PortablePyIDE

给小学 3-6 年级孩子使用的零配置 Python 图形化 IDE。

## 目标

- 双击就能写 Python，不要求孩子安装 Python、配置环境变量或理解命令行。
- 默认中文界面，按钮少而清楚：新建、打开、保存、设主文件、运行、停下、调试、问问、哪里错了。
- 顶部工具栏包含课程要求的图标：新建空白文件、保存文件、打开文件、打开文件夹、运行、停止、调试小虫子。
- 中间代码区默认最大，AI 小老师默认收起，不挡住写代码。
- 支持多个 Python 文件标签页，适合 `main.py`、`helper.py` 这样的多文件作品。
- 错误提示先用孩子能看懂的话解释，再保留原始信息给老师或家长查看。
- AI 小老师只给解释、提示和建议，不会自动改孩子的代码。

## 写代码

- 双击左边“我的作品”里的 `.py`、`.txt`、`.md` 文件，会在中间打开一个新标签。
- 标签名后面出现 `*`，表示这个文件还没有保存。
- Python 代码有语法高亮、行号、当前行高亮、括号配对、自动缩进和代码提示。
- 菜单 `编辑` 里有查找文字、注释/取消注释、字变大、字变小。
- 点顶部“代码提示”可以打开或关闭自动补全。

## 多文件运行

- 默认点击“运行”会运行当前 `.py` 文件。
- 多文件项目建议把入口文件保存成 `main.py`，然后点“设为主文件”。
- 设好主文件后，从任意标签点“运行”或“调试”，都会运行这个主文件。
- 运行时工作目录是左侧打开的项目文件夹，所以 `main.py` 可以正常 `import helper`。

## 断点调试

- 点代码左边的行号，可以加一个红点断点；再点一次会取消。
- 点“调试”，小虫子会在红点那一行停住。
- 停住后可以点“继续”跑到下一个红点，也可以点“下一步”一行一行看。
- 底部“调试变量”会显示简单变量，例如数字、文字、列表、字典。
- 点“停下”可以结束正在运行或调试的程序。

## 开发运行

```powershell
cd c:\aaa\portable_py_ide
powershell -ExecutionPolicy Bypass -File .\scripts\setup_dev.ps1
.\.venv\Scripts\python.exe -m portable_py_ide
```

如果你不想创建虚拟环境，也可以设置 `PYTHONPATH` 后用已有 Python 启动：

```powershell
$env:PYTHONPATH="$PWD\app"
python -m portable_py_ide
```

## Windows 便携包

```powershell
cd c:\aaa\portable_py_ide
powershell -ExecutionPolicy Bypass -File .\scripts\build_windows_release.ps1
```

生成目录：

```text
dist\PortablePyIDE-win-x64\
```

孩子只需要双击里面的 `start.bat`。发布包会包含官方 Python 3.13.13 Windows embeddable runtime 和应用依赖。

脚本会优先生成 `小小Python创作室.exe` 作为启动入口；如果构建机缺少 C# 编译能力，会保留 `start.bat` 作为备用入口。

## AI 小老师

AI 默认预设为 DeepSeek 的 OpenAI-compatible 接口。第一次使用时在菜单 `AI -> AI 小老师设置` 填写：

- Base URL，默认 `https://api.deepseek.com`
- Model，默认 `deepseek-v4-flash`
- API Key

没有 API Key 时，写代码、运行代码、本地错误解释都能正常使用。

## 许可证说明

本项目按 GPL 组件路线规划，使用 PyQt6 / QScintilla。发布前请按实际分发方式确认许可证合规。
