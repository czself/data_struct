# Qt QWidget 体系完整学习笔记

> 这是 Qt 最传统、最稳定、最适合课程设计的开发体系。
> 核心思想：**一切都是控件（Widget）**

---

## 目录

1. [QWidget 体系概述](#一qwidget-体系概述)
2. [最基础窗口](#二最基础窗口)
3. [常用控件详解](#三常用控件详解)
4. [布局系统（超重要）](#四布局系统超重要)
5. [信号槽机制（最关键）](#五信号槽机制最关键)
6. [综合实战：登录窗口](#六综合实战登录窗口)
7. [进阶实战：简易计算器](#七进阶实战简易计算器)
8. [常用控件速查表](#八常用控件速查表)
9. [课程设计推荐项目](#九课程设计推荐项目)
10. [避坑指南](#十避坑指南)

---

## 一、QWidget 体系概述

### 什么是 QWidget？

`QWidget` 是所有用户界面控件的**基类**。任何一个可以显示在屏幕上的东西，本质上都是一个 `QWidget`。

### 控件继承关系

```
QObject
  └── QWidget                  ← 所有控件的基类
        ├── QPushButton        ← 按钮
        ├── QLabel             ← 文本标签
        ├── QLineEdit          ← 单行输入框
        ├── QTextEdit          ← 多行文本框
        ├── QCheckBox          ← 复选框
        ├── QComboBox          ← 下拉框
        ├── QListWidget        ← 列表
        ├── QTableWidget       ← 表格
        └── ...其他控件...
```

### 为什么学 QWidget 体系？

| 特点 | 说明 |
|------|------|
| 逻辑直观 | 按钮就是按钮，窗口就是窗口，所见即所得 |
| 事件驱动 | 点击按钮 → 执行函数，思维模型简单 |
| Qt Designer 支持 | 可视化拖拽设计界面 |
| 教程最多 | 网上绝大多数 Qt 教程都是 QWidget |
| 课程设计首选 | 足够完成学生管理、图书管理、记事本等常见项目 |

---

## 二、最基础窗口

### 2.1 最小 Qt 程序

```cpp
#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   // 1. 创建 Qt 应用对象

    QWidget window;                 // 2. 创建窗口
    window.resize(400, 300);        // 3. 设置窗口大小（宽400，高300）
    window.setWindowTitle("我的第一个Qt窗口"); // 设置标题
    window.show();                  // 4. 显示窗口

    return app.exec();              // 5. 进入事件循环
}
```

### 2.2 关键代码解析

| 代码 | 含义 | 说明 |
|------|------|------|
| `QApplication app(argc, argv)` | Qt 应用对象 | 每个 Qt 程序有且仅有一个，管理全局资源 |
| `QWidget window` | 创建窗口 | 创建一个空白窗口 |
| `window.resize(w, h)` | 设置大小 | 参数为宽度和高度（像素） |
| `window.show()` | 显示窗口 | 默认创建的窗口是隐藏的，必须调用 show() |
| `app.exec()` | 事件循环 | 程序进入等待用户操作的状态，直到窗口关闭才返回 |

### 2.3 窗口常用设置

```cpp
window.resize(800, 600);                // 设置大小
window.setWindowTitle("标题");           // 设置标题
window.setFixedSize(400, 300);          // 设置固定大小（不可缩放）
window.setMinimumSize(300, 200);        // 设置最小尺寸
window.setMaximumSize(1000, 800);       // 设置最大尺寸
window.move(100, 50);                   // 移动窗口到屏幕坐标 (100, 50)
```

### 2.4 pro 文件配置

```pro
# notepad.pro
QT       += core gui widgets    # 启用 widgets 模块
TARGET   = 你的程序名           # 生成的可执行文件名
TEMPLATE = app                  # 应用程序模板
SOURCES += main.cpp             # 源文件
HEADERS +=                      # 头文件（如有）
```

> **注意**：Qt5 以后，`QT += widgets` 是必须的。如果没有这一行，`QWidget`、`QPushButton` 等控件类无法使用。

---

## 三、常用控件详解

### 3.1 QPushButton（按钮）

最常用的交互控件，用户点击触发操作。

#### 基本用法

```cpp
#include <QPushButton>

// 方式一：直接创建
QPushButton button("点我");
button.show();

// 方式二：指定父对象（推荐，由父窗口管理内存）
QPushButton *button = new QPushButton("登录", this);  // this 为父窗口
```

#### 完整示例

```cpp
#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QPushButton button("Hello Qt");
    button.resize(300, 100);         // 按钮大小
    button.setFont(QFont("微软雅黑", 16));  // 设置字体
    button.show();

    return app.exec();
}
```

#### 常用方法

```cpp
button->setText("新文本");           // 设置按钮文字
button->text();                      // 获取按钮文字
button->setEnabled(false);           // 禁用按钮（灰化不可点击）
button->setEnabled(true);            // 启用按钮
button->setFont(QFont("Arial", 14)); // 设置字体
button->setStyleSheet("background-color: lightblue;"); // 设置样式
```

### 3.2 QLabel（文本标签）

用于显示文本、图片或富文本，**不可交互**（默认不可点击）。

#### 基本用法

```cpp
#include <QLabel>

QLabel *label = new QLabel("你好，Qt！", this);
```

#### 常用功能

```cpp
label->setText("新文本");            // 设置文本
label->text();                       // 获取文本

// 显示数字
label->setNum(42);                   // 显示 "42"
label->setNum(3.14159);              // 显示 "3.14159"

// 显示图片
QPixmap pix("image.png");
label->setPixmap(pix);

// 富文本（HTML）
label->setText("<h2 style='color:red;'>警告</h2>");

// 文本对齐
label->setAlignment(Qt::AlignCenter);            // 居中
label->setAlignment(Qt::AlignRight | Qt::AlignVCenter); // 右居中

// 自动换行
label->setWordWrap(true);
```

#### 常见用途

| 用途 | 说明 |
|------|------|
| 提示文字 | "用户名："、"密码：" |
| 显示状态 | "登录成功"、"加载中..." |
| 显示结果 | 计算器的输出、查询结果 |
| 图片显示 | 头像、图标、背景图 |

### 3.3 QLineEdit（单行输入框）

用于接收用户输入的单行文本。

#### 基本用法

```cpp
#include <QLineEdit>

QLineEdit *edit = new QLineEdit(this);
```

#### 常用设置

```cpp
edit->setPlaceholderText("请输入用户名");   // 占位提示文字（灰色字）
edit->setText("默认文本");                  // 设置默认文本
edit->text();                               // 获取用户输入的文本
edit->clear();                              // 清空内容
edit->setEchoMode(QLineEdit::Password);     // 密码模式（显示••••）
edit->setEchoMode(QLineEdit::Normal);       // 正常模式（默认）
edit->setEchoMode(QLineEdit::NoEcho);       // 不显示任何字符
edit->setMaxLength(20);                     // 最大输入长度
edit->setReadOnly(true);                    // 只读模式
```

#### 输入验证

```cpp
// 限制只能输入数字
#include <QValidator>
edit->setValidator(new QIntValidator(0, 999, this));     // 整数 0-999
edit->setValidator(new QDoubleValidator(0.0, 100.0, 2, this)); // 浮点数

// 限制输入格式（如电话号码：XXX-XXXXXXX）
edit->setInputMask("000-0000000;_");
```

### 3.4 QTextEdit（多行文本框）

用于显示和编辑多行文本，支持富文本（HTML）。

```cpp
#include <QTextEdit>

QTextEdit *editor = new QTextEdit(this);
editor->setPlainText("普通文本内容");         // 设置纯文本
editor->setHtml("<h1>富文本</h1><p>内容</p>"); // 设置 HTML
editor->toPlainText();                        // 获取纯文本
editor->append("追加一行");                   // 末尾追加
editor->clear();                              // 清空
```

### 3.5 QCheckBox（复选框）

用于多选场景。

```cpp
#include <QCheckBox>

QCheckBox *check = new QCheckBox("记住密码", this);
check->isChecked();          // 是否选中（返回 true/false）
check->setChecked(true);     // 设为选中状态
check->setText("同意协议");  // 设置文本
```

### 3.6 QComboBox（下拉框）

用于从多个选项中选择一个。

```cpp
#include <QComboBox>

QComboBox *combo = new QComboBox(this);
combo->addItem("选项1");            // 添加选项
combo->addItem("选项2");
combo->addItems({"男", "女", "保密"}); // 批量添加
combo->currentText();               // 获取当前选中的文本
combo->currentIndex();              // 获取当前选中的索引（从0开始）
combo->setCurrentIndex(0);          // 设置当前选中项
```

### 3.7 QListWidget（列表）

用于显示列表数据。

```cpp
#include <QListWidget>

QListWidget *list = new QListWidget(this);
list->addItem("第一项");
list->addItem("第二项");
list->addItem("第三项");

// 获取当前选中项
QString text = list->currentItem()->text();
int row = list->currentRow();

// 删除选中项
delete list->takeItem(list->currentRow());
```

### 3.8 QTableWidget（表格）

用于显示表格数据。

```cpp
#include <QTableWidget>

QTableWidget *table = new QTableWidget(3, 4, this); // 3行4列

// 设置表头
table->setHorizontalHeaderLabels({"姓名", "年龄", "性别", "成绩"});

// 填入数据
table->setItem(0, 0, new QTableWidgetItem("张三"));
table->setItem(0, 1, new QTableWidgetItem("20"));
table->setItem(0, 2, new QTableWidgetItem("男"));
table->setItem(0, 3, new QTableWidgetItem("95"));

// 获取选中行
int row = table->currentRow();
QString name = table->item(row, 0)->text();
```

### 3.9 QMessageBox（消息弹窗）

用于提示信息、警告、错误、提问等。

```cpp
#include <QMessageBox>

// 信息提示
QMessageBox::information(this, "标题", "操作成功！");

// 警告
QMessageBox::warning(this, "警告", "文件未保存！");

// 错误
QMessageBox::critical(this, "错误", "无法连接到数据库！");

// 提问（返回 Yes 或 No）
int ret = QMessageBox::question(this, "确认",
                                "确定要删除吗？",
                                QMessageBox::Yes | QMessageBox::No);
if (ret == QMessageBox::Yes) {
    // 用户点击了"是"
}

// 关于对话框
QMessageBox::about(this, "关于", "记事本 v1.0\n作者：xxx");
```

### 3.10 QTimer（定时器）

用于定时执行某些操作。

```cpp
#include <QTimer>

QTimer *timer = new QTimer(this);
timer->start(1000);                     // 每1000毫秒（1秒）触发一次
// timer->setSingleShot(true);          // 只触发一次

// 连接信号槽
connect(timer, &QTimer::timeout, this, [=]() {
    qDebug() << "定时器触发了！";
});

// 停止定时器
timer->stop();
```

### 3.11 QFile（文件操作）

用于读写文件。

```cpp
#include <QFile>
#include <QTextStream>

// 写文件
QFile file("data.txt");
if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    QTextStream out(&file);
    out << "要写入的内容" << "\n";
    file.close();
}

// 读文件
if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QTextStream in(&file);
    QString content = in.readAll();  // 读取全部内容
    file.close();
}
```

---

## 四、布局系统（超重要）

### 4.1 新手最常见的错误

```cpp
// ❌ 错误：手动写坐标
button1->move(100, 50);
button2->move(200, 100);
// 窗口缩放时按钮位置不会变，会乱掉甚至重叠
```

### 4.2 布局系统的作用

布局自动管理控件的位置和大小，当窗口缩放时控件会自动调整。

### 4.3 四种布局

| 布局 | 类名 | 作用 |
|------|------|------|
| **垂直布局** | `QVBoxLayout` | 控件从上到下垂直排列 |
| **水平布局** | `QHBoxLayout` | 控件从左到右水平排列 |
| **网格布局** | `QGridLayout` | 控件按行/列网格排列 |
| **表单布局** | `QFormLayout` | 两列形式（标签+控件），适合表单 |

### 4.4 QVBoxLayout（垂直布局）

```cpp
#include <QVBoxLayout>

QVBoxLayout *layout = new QVBoxLayout;

layout->addWidget(button1);
layout->addWidget(button2);
layout->addWidget(button3);

window.setLayout(layout);
```

效果：
```
┌──────────────┐
│  [button1]   │
│  [button2]   │
│  [button3]   │
└──────────────┘
```

### 4.5 QHBoxLayout（水平布局）

```cpp
#include <QHBoxLayout>

QHBoxLayout *layout = new QHBoxLayout;
layout->addWidget(button1);
layout->addWidget(button2);
layout->addWidget(button3);

window.setLayout(layout);
```

效果：
```
┌────────────────────┐
│ [button1] [button2] [button3]│
└────────────────────┘
```

### 4.6 QGridLayout（网格布局）

```cpp
#include <QGridLayout>

QGridLayout *layout = new QGridLayout;

// addWidget(控件, 行, 列, 行跨度, 列跨度)
layout->addWidget(button1, 0, 0);       // 第0行第0列
layout->addWidget(button2, 0, 1);       // 第0行第1列
layout->addWidget(button3, 1, 0, 1, 2); // 第1行第0列，占1行2列
```

效果：
```
┌────────────────────┐
│ [button1] [button2] │
│  [button3 (跨两列)] │
└────────────────────┘
```

### 4.7 QFormLayout（表单布局）

```cpp
#include <QFormLayout>

QFormLayout *layout = new QFormLayout;
layout->addRow("用户名:", usernameEdit);
layout->addRow("密  码:", passwordEdit);
layout->addRow("", loginButton);  // 不显示标签
```

效果：
```
┌────────────────────┐
│ 用户名: [________] │
│ 密  码: [________] │
│       [登录]       │
└────────────────────┘
```

### 4.8 布局嵌套（核心技巧）

实际开发中几乎都是**布局嵌套**：

```cpp
// 外层：垂直布局
QVBoxLayout *mainLayout = new QVBoxLayout;

// 内层：水平布局（放按钮行）
QHBoxLayout *buttonLayout = new QHBoxLayout;
buttonLayout->addWidget(okButton);
buttonLayout->addWidget(cancelButton);

// 组装
mainLayout->addWidget(label);
mainLayout->addLayout(buttonLayout);  // 嵌入子布局

window.setLayout(mainLayout);
```

```
┌────────────────────┐
│    [这是一个标签]    │  ← QVBoxLayout
│ [确定]   [取消]     │  ← QHBoxLayout 嵌入
└────────────────────┘
```

### 4.9 布局设置

```cpp
layout->setSpacing(10);        // 控件之间的间距
layout->setContentsMargins(20, 20, 20, 20); // 外边距（左上右下）
layout->addStretch();          // 添加弹簧（把控件推向一边）
```

### 4.10 布局 + 弹簧示例

```cpp
// 把按钮推到右边
QHBoxLayout *layout = new QHBoxLayout;
layout->addStretch();           // ← 弹性空间
layout->addWidget(button);      // 按钮被推到右侧
```

```
┌──────────────────────┐
│              [button] │
└──────────────────────┘
```

```cpp
// 两个按钮左右分开
QHBoxLayout *layout = new QHBoxLayout;
layout->addWidget(button1);     // 左侧
layout->addStretch();           // 中间弹性空间
layout->addWidget(button2);     // 右侧
```

```
┌──────────────────────┐
│ [button1]    [button2] │
└──────────────────────┘
```

---

## 五、信号槽机制（最关键）

### 5.1 什么是信号槽？

信号槽是 Qt 的核心机制，用于**对象之间的通信**。

### 5.2 基本概念

```
用户点击按钮 → 按钮发出 clicked 信号 → 连接到槽函数 → 执行操作
```

### 5.3 标准信号槽连接

```cpp
connect(发送信号的对象, 信号,
        接收信号的对象, 槽函数);
```

#### 示例：按钮点击关闭窗口

```cpp
connect(button, &QPushButton::clicked,   // 信号：按钮被点击
        this,  &QWidget::close);         // 槽：关闭窗口
```

### 5.4 常用信号

```cpp
// 按钮点击
connect(button, &QPushButton::clicked, ...);

// 输入框文本变化
connect(edit, &QLineEdit::textChanged, ...);

// 下拉框选项变化
connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged), ...);

// 复选框状态变化
connect(check, &QCheckBox::stateChanged, ...);
```

### 5.5 使用 Lambda 表达式（推荐）

对于简单操作，直接写 Lambda 更简洁：

```cpp
#include <QPushButton>
#include <QLabel>

QPushButton *btn = new QPushButton("点我", this);
QLabel *label = new QLabel("隐藏的文本", this);
label->hide();  // 初始隐藏

connect(btn, &QPushButton::clicked, this, [=]() {
    label->show();   // 点击按钮显示文本
    label->setText("按钮被点击了！");
});
```

### 5.6 自定义槽函数（推荐方式）

```cpp
// 在头文件中声明槽函数
// widget.h
class Widget : public QWidget
{
    Q_OBJECT  // 必须包含这个宏

public:
    Widget(QWidget *parent = nullptr);

private slots:  // 槽函数声明区
    void onLoginClicked();
    void onTextChanged(const QString &text);

private:
    QPushButton *loginBtn;
    QLineEdit *usernameEdit;
};
```

```cpp
// widget.cpp 实现
Widget::Widget(QWidget *parent) : QWidget(parent)
{
    loginBtn = new QPushButton("登录", this);
    usernameEdit = new QLineEdit(this);

    // 连接信号槽
    connect(loginBtn, &QPushButton::clicked,
            this, &Widget::onLoginClicked);

    connect(usernameEdit, &QLineEdit::textChanged,
            this, &Widget::onTextChanged);
}

void Widget::onLoginClicked()
{
    qDebug() << "登录按钮被点击了";
    // 执行登录逻辑...
}

void Widget::onTextChanged(const QString &text)
{
    qDebug() << "输入框内容变了:" << text;
}
```

### 5.7 多对一连接

多个信号可以连接到同一个槽函数：

```cpp
connect(btn1, &QPushButton::clicked, this, &Widget::onButtonClicked);
connect(btn2, &QPushButton::clicked, this, &Widget::onButtonClicked);
connect(btn3, &QPushButton::clicked, this, &Widget::onButtonClicked);

// 使用 sender() 区分是哪个按钮触发的
void Widget::onButtonClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (btn) {
        qDebug() << "点击了:" << btn->text();
    }
}
```

### 5.8 信号槽注意事项

| 注意事项 | 说明 |
|----------|------|
| 信号函数的参数类型和数量必须与槽函数兼容 | 槽函数的参数可以少于信号，但不能多于信号 |
| `Q_OBJECT` 宏不能少 | 自定义类如果使用信号槽，必须在类定义中加入 `Q_OBJECT` |
| 信号和槽的参数类型必须匹配 | 类型不同会导致连接失败 |
| Lambda 捕获注意生命周期 | 捕获 `this`、`[=]` 时要确保对象生命周期 |

---

## 六、综合实战：登录窗口

这是 Qt 入门**最好的练习**，包含了几乎所有基础内容：

- ✅ 标签（QLabel）
- ✅ 输入框（QLineEdit）
- ✅ 按钮（QPushButton）
- ✅ 布局（Layout）
- ✅ 信号槽（connect）
- ✅ 弹窗（QMessageBox）

### 6.1 项目结构

```
login_demo/
├── login_demo.pro
├── main.cpp
├── loginwidget.h
└── loginwidget.cpp
```

### 6.2 login_demo.pro

```pro
QT       += core gui widgets
TARGET   = login_demo
TEMPLATE = app
SOURCES += main.cpp loginwidget.cpp
HEADERS += loginwidget.h
```

### 6.3 main.cpp

```cpp
#include <QApplication>
#include "loginwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LoginWidget login;
    login.show();

    return app.exec();
}
```

### 6.4 loginwidget.h

```cpp
#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>

class LoginWidget : public QWidget
{
    Q_OBJECT  // 信号槽必须

public:
    LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget() = default;

private slots:
    void onLoginClicked();
    void onClearClicked();

private:
    QLabel *titleLabel;       // 标题
    QLabel *usernameLabel;    // 用户名标签
    QLabel *passwordLabel;    // 密码标签
    QLineEdit *usernameEdit;  // 用户名输入框
    QLineEdit *passwordEdit;  // 密码输入框
    QCheckBox *rememberCheck; // 记住密码复选框
    QPushButton *loginBtn;    // 登录按钮
    QPushButton *clearBtn;    // 清空按钮
};

#endif // LOGINWIDGET_H
```

### 6.5 loginwidget.cpp

```cpp
#include "loginwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFont>

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
{
    // 窗口设置
    setWindowTitle("用户登录");
    setFixedSize(400, 300);  // 固定大小

    // ── 1. 创建控件 ──

    // 标题
    titleLabel = new QLabel("欢迎登录系统");
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setFont(QFont("微软雅黑", 18, QFont::Bold));

    // 用户名
    usernameLabel = new QLabel("用户名:");
    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("请输入用户名");

    // 密码
    passwordLabel = new QLabel("密  码:");
    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("请输入密码");
    passwordEdit->setEchoMode(QLineEdit::Password);

    // 记住密码复选框
    rememberCheck = new QCheckBox("记住密码");

    // 按钮
    loginBtn = new QPushButton("登  录");
    clearBtn = new QPushButton("清  空");

    // 设置按钮大小
    loginBtn->setMinimumHeight(35);
    clearBtn->setMinimumHeight(35);

    // ── 2. 布局 ──

    // 表单布局（用户名、密码）
    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(usernameLabel, usernameEdit);
    formLayout->addRow(passwordLabel, passwordEdit);
    formLayout->addRow("", rememberCheck);  // 不显示标签

    // 按钮行水平布局
    QHBoxLayout *btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(loginBtn);
    btnLayout->addSpacing(20);             // 按钮间间距
    btnLayout->addWidget(clearBtn);
    btnLayout->addStretch();

    // 主布局（垂直）
    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addSpacing(20);
    mainLayout->addWidget(titleLabel);
    mainLayout->addSpacing(20);
    mainLayout->addLayout(formLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(btnLayout);
    mainLayout->addStretch();

    setLayout(mainLayout);

    // ── 3. 信号槽连接 ──

    connect(loginBtn, &QPushButton::clicked,
            this, &LoginWidget::onLoginClicked);

    connect(clearBtn, &QPushButton::clicked,
            this, &LoginWidget::onClearClicked);

    // 按回车触发的登录
    connect(passwordEdit, &QLineEdit::returnPressed,
            this, &LoginWidget::onLoginClicked);
}

void LoginWidget::onLoginClicked()
{
    QString username = usernameEdit->text().trimmed();  // 去除首尾空格
    QString password = passwordEdit->text();

    // 输入验证
    if (username.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入用户名！");
        usernameEdit->setFocus();  // 光标定位到用户名输入框
        return;
    }

    if (password.isEmpty()) {
        QMessageBox::warning(this, "提示", "请输入密码！");
        passwordEdit->setFocus();
        return;
    }

    // 演示账号验证
    if (username == "admin" && password == "123456") {
        QMessageBox::information(this, "登录成功",
                                 "欢迎回来，管理员！");
        accept();
        // 这里可以关闭登录窗口，打开主界面
    } else {
        QMessageBox::critical(this, "登录失败",
                              "用户名或密码错误！");
        passwordEdit->clear();     // 清空密码
        passwordEdit->setFocus();  // 焦点回到密码框
    }
}

void LoginWidget::onClearClicked()
{
    usernameEdit->clear();
    passwordEdit->clear();
    usernameEdit->setFocus();
}
```

### 6.6 运行效果

```
┌──────────────────────────┐
│      欢迎登录系统          │
│                          │
│ 用户名: [____________]    │
│ 密  码: [____________]    │
│        ☐ 记住密码         │
│                          │
│    [登  录]  [清  空]     │
│                          │
└──────────────────────────┘
```

---

## 七、进阶实战：简易计算器

更复杂的练习，展示了网格布局和多个信号槽连接。

### 7.1 calculator.h

```cpp
#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>

class Calculator : public QWidget
{
    Q_OBJECT

public:
    Calculator(QWidget *parent = nullptr);

private slots:
    void onNumberClicked();    // 数字按钮点击
    void onOperatorClicked();  // 运算符按钮点击
    void onEqualClicked();     // 等号
    void onClearClicked();     // 清空

private:
    QLineEdit *display;       // 显示区域
    double leftOperand;       // 左操作数
    QString currentOperator;  // 当前运算符
    bool isNewInput;          // 是否是新一轮输入
};

#endif // CALCULATOR_H
```

### 7.2 calculator.cpp

```cpp
#include "calculator.h"
#include <QGridLayout>
#include <QPushButton>
#include <QFont>

Calculator::Calculator(QWidget *parent)
    : QWidget(parent), leftOperand(0), isNewInput(true)
{
    setWindowTitle("计算器");
    setFixedSize(300, 350);

    // 显示框
    display = new QLineEdit("0");
    display->setReadOnly(true);               // 只读
    display->setAlignment(Qt::AlignRight);    // 右对齐
    display->setFont(QFont("Consolas", 20));
    display->setMinimumHeight(50);

    // 按钮文本
    QStringList btnTexts = {
        "7", "8", "9", "+",
        "4", "5", "6", "-",
        "1", "2", "3", "*",
        "0", "C", "=", "/"
    };

    // 网格布局
    QGridLayout *grid = new QGridLayout;
    grid->addWidget(display, 0, 0, 1, 4);  // 显示框占一整行

    int row = 1, col = 0;
    for (const QString &text : btnTexts) {
        QPushButton *btn = new QPushButton(text);
        btn->setMinimumSize(60, 50);
        btn->setFont(QFont("Arial", 14));

        grid->addWidget(btn, row, col);

        // 连接信号
        if (text == "C") {
            connect(btn, &QPushButton::clicked,
                    this, &Calculator::onClearClicked);
        } else if (text == "=") {
            connect(btn, &QPushButton::clicked,
                    this, &Calculator::onEqualClicked);
        } else if (text == "+" || text == "-" ||
                   text == "*" || text == "/") {
            connect(btn, &QPushButton::clicked,
                    this, &Calculator::onOperatorClicked);
        } else {
            connect(btn, &QPushButton::clicked,
                    this, &Calculator::onNumberClicked);
        }

        col++;
        if (col > 3) {
            col = 0;
            row++;
        }
    }

    setLayout(grid);
}

void Calculator::onNumberClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    QString digit = btn->text();

    if (isNewInput) {
        display->clear();
        isNewInput = false;
    }

    display->setText(display->text() + digit);
}

void Calculator::onOperatorClicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>(sender());
    if (!btn) return;

    leftOperand = display->text().toDouble();
    currentOperator = btn->text();
    isNewInput = true;
}

void Calculator::onEqualClicked()
{
    double rightOperand = display->text().toDouble();
    double result = 0;

    if (currentOperator == "+") {
        result = leftOperand + rightOperand;
    } else if (currentOperator == "-") {
        result = leftOperand - rightOperand;
    } else if (currentOperator == "*") {
        result = leftOperand * rightOperand;
    } else if (currentOperator == "/") {
        if (rightOperand != 0) {
            result = leftOperand / rightOperand;
        } else {
            display->setText("错误");
            isNewInput = true;
            return;
        }
    }

    display->setText(QString::number(result));
    isNewInput = true;
}

void Calculator::onClearClicked()
{
    display->setText("0");
    leftOperand = 0;
    currentOperator.clear();
    isNewInput = true;
}
```

---

## 八、常用控件速查表

### 8.1 基本控件

| 类名 | 用途 | 头文件 | 常用信号 |
|------|------|--------|----------|
| `QWidget` | 基础窗口/容器 | `QWidget` | — |
| `QPushButton` | 按钮 | `QPushButton` | `clicked()` |
| `QLabel` | 文本/图片显示 | `QLabel` | — |
| `QLineEdit` | 单行输入 | `QLineEdit` | `textChanged()`, `returnPressed()` |
| `QTextEdit` | 多行文本 | `QTextEdit` | `textChanged()` |
| `QCheckBox` | 复选框 | `QCheckBox` | `stateChanged()` |
| `QComboBox` | 下拉框 | `QComboBox` | `currentIndexChanged()` |
| `QListWidget` | 列表 | `QListWidget` | `currentItemChanged()` |
| `QTableWidget` | 表格 | `QTableWidget` | `cellClicked()` |

### 8.2 布局

| 类名 | 用途 | 说明 |
|------|------|------|
| `QVBoxLayout` | 垂直布局 | 控件纵向排列 |
| `QHBoxLayout` | 水平布局 | 控件横向排列 |
| `QGridLayout` | 网格布局 | 行列排列，适合计算器/键盘 |
| `QFormLayout` | 表单布局 | 标签+控件，适合登录/设置 |

### 8.3 其他常用类

| 类名 | 用途 | 头文件 |
|------|------|--------|
| `QMessageBox` | 消息弹窗 | `QMessageBox` |
| `QTimer` | 定时器 | `QTimer` |
| `QFile` | 文件读写 | `QFile` |
| `QFont` | 字体设置 | `QFont` |

---

## 九、课程设计推荐项目

### 9.1 学生管理系统

```
功能：
  - 添加学生（姓名、学号、成绩）
  - 显示学生列表（QTableWidget）
  - 删除/修改学生信息
  - 成绩统计（最高分、最低分、平均分）
  - 数据保存到文件（QFile）

涉及控件：
  QTableWidget, QPushButton, QLineEdit, QLabel
  QVBoxLayout, QHBoxLayout, QGridLayout
  QMessageBox, QFile
```

### 9.2 简易记事本

```
功能：
  - 新建、打开、保存文件
  - 编辑文本（QTextEdit）
  - 字体设置
  - 查找替换

涉及控件：
  QTextEdit, QPushButton, QMenuBar, QMessageBox
  QFile, QFont
```

### 9.3 番茄钟

```
功能：
  - 倒计时显示（QLabel）
  - 开始、暂停、重置（QPushButton）
  - 时间到弹窗提醒（QMessageBox）
  - 自定义时间

涉及控件：
  QLabel, QPushButton, QTimer, QMessageBox
  QLineEdit, QSpinBox
```

### 9.4 计算器

见 [第七节](#七进阶实战简易计算器)

### 9.5 图书管理系统

```
功能：
  - 增删改查图书信息
  - 按书名/作者搜索
  - 借阅/归还记录

涉及控件：
  QTableWidget, QLineEdit, QPushButton, QComboBox
  QMessageBox, QFile
```

---

## 十、避坑指南

### ❌ 常见错误及正确写法

#### 错误 1：忘记加 widgets 模块

```pro
# ❌ 错误
QT += core gui

# ✅ 正确（Qt5/6 必须加 widgets）
QT += core gui widgets
```

#### 错误 2：手动写坐标

```cpp
// ❌ 错误：窗口缩放了就乱掉
button->move(100, 50);

// ✅ 正确：使用布局
QVBoxLayout *layout = new QVBoxLayout;
layout->addWidget(button);
window.setLayout(layout);
```

#### 错误 3：忘记 Q_OBJECT 宏

```cpp
// ❌ 错误：信号槽连接会失败
class MyWidget : public QWidget
{
    // 缺少 Q_OBJECT
    void onButtonClicked();
};

// ✅ 正确
class MyWidget : public QWidget
{
    Q_OBJECT
    // ...
};
```

#### 错误 4：在构造函数中 connect Lambda 时生命周期问题

```cpp
// ✅ 推荐：使用 [=] 捕获
connect(btn, &QPushButton::clicked, this, [=]() {
    // this 和 btn 都被捕获，安全
    label->setText("点击了");
});
```

#### 错误 5：new 了控件没有指定父对象

```cpp
// ❌ 错误：内存泄漏，窗口关闭后控件不会被销毁
QPushButton *btn = new QPushButton("按钮");

// ✅ 正确：指定父对象，由父窗口管理内存
QPushButton *btn = new QPushButton("按钮", this);
```

### ✅ 推荐编码习惯

1. **所有 new 出来的控件都指定父对象** `this`
2. **始终使用布局**，不要用 `move()` 和 `resize()` 手动控制位置
3. **用 `setFixedSize()` 固定窗口大小** 或使用布局让窗口自适应
4. **输入验证在前端就做**，减少不必要的后端请求
5. **使用 Lambda 连接简单信号槽**，代码更简洁
6. **控件命名规范**：`xxxBtn`、`xxxEdit`、`xxxLabel`、`xxxLayout`

---

## 附录：学习路线图

```
第一阶段（入门）               第二阶段（基础）             第三阶段（提升）
┌───────────────┐            ┌───────────────┐           ┌───────────────┐
│ QWidget       │            │ QGridLayout   │           │ QFile         │
│ QPushButton   │  ──────►   │ QFormLayout   │  ──────►  │ QTimer        │
│ QLabel        │            │ 布局嵌套       │           │ QListWidget   │
│ QLineEdit     │            │ 信号槽深入      │           │ QTableWidget  │
│ QVBoxLayout   │            │ 自定义槽函数    │           │ 数据持久化     │
│ QHBoxLayout   │            │ Lambda         │           │               │
│ connect信号槽  │            │ QMessageBox    │           │               │
└───────────────┘            └───────────────┘           └───────────────┘
        │                            │                           │
        ▼                            ▼                           ▼
   登录窗口练习                  计算器实战                 课程设计项目
```

---

> **最后建议**：不要急着学多线程、网络、QML、OpenGL。
>
> 先把 `QWidget`、`QPushButton`、`QLabel`、`QLineEdit`、布局、信号槽这六个核心吃透，就足够做出 90% 的课程设计项目了。
>
> **动手写一个登录窗口，胜过读十遍教程。**