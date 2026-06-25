# qt.pro 写法详解

> 基于 `qt_try` 目录下的两个实际例子：`qt.pro`（单文件）和 `qt_notepad/notepad.pro`（多文件）

---

## 目录结构参考

```
qt_try/
├── qt.cpp               # 单文件
├── qt.pro               # 单文件的 pro
└── qt_notepad/          # 多文件项目
    ├── main.cpp
    ├── notepad.cpp
    ├── notepad.h
    └── notepad.pro
```

---

## 核心变量

### `QT` —— 启用 Qt 模块

```pro
QT += core gui widgets
```

| 模块 | 作用 | 不加会怎样 |
|------|------|-----------|
| `core` | Qt 核心（默认就有） | 基本必加 |
| `gui` | 绘图、事件等 GUI 基础 | GUI 功能失效 |
| `widgets` | QWidget, QPushButton 等控件 | `QWidget` 都报错 |

`notepad.pro` 中的兼容写法：

```pro
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
```

意思：如果 Qt 主版本 > 4（即 Qt5/Qt6），就加上 `widgets` 模块。用于兼容 Qt4。

---

### `TARGET` —— 生成的文件名

```pro
TARGET = qt        # 最后运行 ./qt
TARGET = notepad   # 最后运行 ./notepad
```

不写则默认等于项目目录名。

---

### `TEMPLATE` —— 项目类型

| 值 | 用途 |
|----|------|
| `app` | 应用程序（可执行文件） |
| `lib` | 库（静态 `.a` 或动态 `.so`） |
| `subdirs` | 子目录项目（组合多个子项目） |

---

### `SOURCES` + `HEADERS` —— 源文件和头文件

**单文件**（`qt.pro`）：

```pro
SOURCES += qt.cpp
```

**多文件**（`notepad.pro`）：

```pro
SOURCES += \
    main.cpp \
    notepad.cpp

HEADERS += \
    notepad.h
```

- `\` 是换行续行符，也可以写在一行用空格隔开
- `HEADERS` 不写也能编译，但写了可以让 moc（元对象编译器）自动处理 `Q_OBJECT` 宏
- 建议多文件项目按 `notepad.pro` 的格式分行写，清晰易维护

---

### `DEFINES` —— 全局宏定义

```pro
DEFINES += QT_DEPRECATED_WARNINGS
```

等价于 `#define QT_DEPRECATED_WARNINGS`，编译时加 `-D` 参数。

---

### `CONFIG` —— 编译配置

```pro
CONFIG += c++17          # 启用 C++17
CONFIG += release        # 发布模式
CONFIG += debug          # 调试模式
CONFIG += console        # 控制台程序（有终端输出）
```

---

## 其他常用变量

```pro
# 包含路径（头文件搜索目录）
INCLUDEPATH += /usr/include/mylib

# 链接库
LIBS += -lmylib                    # 链接动态库
LIBS += -L/path/to/lib -lmylib     # 指定库路径

# 资源文件
RESOURCES += resources.qrc

# UI 文件（Qt Designer 设计）
FORMS += mainwindow.ui

# 子项目
TEMPLATE = subdirs
SUBDIRS += core plugin app
```

---

## 两个例子的对比

| | `qt.pro` | `notepad.pro` |
|---|---|---|
| 行数 | 4 行 | 15 行 |
| SOURCES | 1 个 cpp | 多个 cpp，分行 |
| HEADERS | 无 | `notepad.h` |
| 条件判断 | 无 | `greaterThan(...)` |
| DEFINES | 无 | `QT_DEPRECATED_WARNINGS` |
| 适用场景 | 快速测试/练手 | 正式项目 |

---

## 附录：各关键字的完整含义

| 关键字 | 含义 |
|--------|------|
| `QT += core gui widgets` | 启用 Qt 模块。`+=` 表示追加 |
| `TARGET = qt` | 生成可执行文件名为 `qt` |
| `TEMPLATE = app` | 告诉 qmake 这是应用程序 |
| `SOURCES += qt.cpp` | 列出要编译的源文件 |
| `HEADERS += notepad.h` | 列出头文件（用于 moc 处理） |
| `DEFINES += XXX` | 定义预处理器宏 |
| `greaterThan(a, b): X` | 条件判断：如果 a > b，执行 X |
| `CONFIG += c++17` | 启用 C++17 标准 |
