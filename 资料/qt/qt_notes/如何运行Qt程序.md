# 如何运行 Qt 程序

> 你的文件：`qt.cpp`（一个最基础的 QWidget 窗口）

---

## 方法一：直接用 qmake（推荐）

### 步骤

#### 1. 创建 .pro 文件

在 `qt.cpp` 同目录下，创建 `qt.pro`：

```pro
QT       += core gui widgets
TARGET   = qt
TEMPLATE = app
SOURCES += qt.cpp
```

#### 2. 运行 qmake

```bash
qmake
```

这会在当前目录生成 `Makefile`。

#### 3. 编译

```bash
make
```

#### 4. 运行

```bash
./qt
```

---

### .pro 文件字段详解

以上 `qt.pro` 各行的含义：

| 字段 | 含义 |
|------|------|
| `QT += core gui widgets` | 启用 Qt 模块。`core` = Qt 核心, `gui` = GUI 基础, `widgets` = QWidget 体系。**如果不加 `widgets`，`QWidget`、`QPushButton` 等都会报错** |
| `TARGET = qt` | 生成的可执行文件名字为 `qt`，最后运行 `./qt` |
| `TEMPLATE = app` | 告诉 qmake 这是应用程序。其他类型：`app` = 可执行程序, `lib` = 库 |
| `SOURCES += qt.cpp` | 列出源文件。多个 cpp 可写多行：`SOURCES += main.cpp` 再 `SOURCES += window.cpp` |

### qmake 干了什么

你执行 `qmake`，实际是 Qt 自动帮你生成 `Makefile`；
然后 `make`，真正调用 `g++` 编译。

**实际流程：**

```
.pro
  ↓ qmake
Makefile
  ↓ make
  g++
  ↓
可执行文件
```

---

### 完整操作（一条命令都不用记）

在你的 `qt.cpp` 目录下，按顺序执行：

```bash
# 第一步：写 pro 文件（只需一次）
echo 'QT += core gui widgets
TARGET = qt
TEMPLATE = app
SOURCES += qt.cpp' > qt.pro

# 第二步：生成 Makefile
qmake

# 第三步：编译
make

# 第四步：运行
./qt
```

---

## 方法二：直接 g++ 编译（不用 pro 文件）

如果你的 Qt 版本支持，也可以直接编译：

```bash
g++ -o qt qt.cpp -I/usr/include/qt5 -I/usr/include/qt5/QtWidgets \
    -I/usr/include/qt5/QtCore -I/usr/include/qt5/QtGui \
    -lQt5Widgets -lQt5Core -lQt5Gui -fPIC
```

> 不过更推荐方法一，因为它会自动处理所有依赖和路径。

---

## 方法三：如果你有 CMake

创建 `CMakeLists.txt`：

```cmake
cmake_minimum_required(VERSION 3.14)
project(qt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

add_executable(qt qt.cpp)

target_link_libraries(qt Qt5::Widgets)
```

然后：

```bash
mkdir build && cd build
cmake ..
make
./qt
```

---

## 快速运行你的 qt.cpp

你的当前目录是 `/run/media/sz/Data/coderepo/qt_try`，里面有 `qt.cpp`。

直接执行：

```bash
cd /run/media/sz/Data/coderepo/qt_try && \
echo 'QT += core gui widgets
TARGET = qt
TEMPLATE = app
SOURCES += qt.cpp' > qt.pro && \
qmake && \
make && \
./qt
```

这样就能看到你的 Qt 窗口弹出来了。

---

## 常见问题

### Q：提示 "找不到 QApplication" 或 "No such file or directory"

**原因：** 没有安装 Qt 开发库，或者 pro 文件缺少 `widgets`。

**解决：**
```bash
# Ubuntu/Debian 安装 Qt5
sudo apt install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools

# 如果还不行，确认 pro 文件有这一行：
QT += core gui widgets
```

### Q：运行后窗口闪一下就消失

**原因：** 用的是单文件，直接在 IDE 中运行的 Qt 代码需要事件循环。

**解决：** 确认 `main()` 最后有 `return app.exec();`，然后用终端运行 `./qt`。

### Q：make 时报错 "multiple definition of main"

**原因：** pro 文件中 SOURCES 列出了多个有 `main()` 的 cpp 文件。

**解决：** 检查 `qt.pro`，确保 `SOURCES +=` 只包含一个带 `main()` 的文件。

---

## 总结

| 步骤 | 命令 | 作用 |
|------|------|------|
| 1 | 创建 `xxx.pro` | 告诉 Qt 项目配置 |
| 2 | `qmake` | 生成 Makefile |
| 3 | `make` | 编译源代码 |
| 4 | `./qt` | 运行程序 |

**你的 `qt.cpp` 是最简单的 Qt 程序，按方法一操作，三步就能运行出来一个窗口。**