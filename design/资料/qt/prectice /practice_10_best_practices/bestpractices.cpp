#include "bestpractices.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QFont>
#include <QGroupBox>

BestPractices::BestPractices(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("第10章 - 避坑指南（最常见的错误和正确做法）");
    setGeometry(50, 50, 1000, 800);

    QVBoxLayout *mainLayout = new QVBoxLayout;

    // 标题
    QLabel *titleLabel = new QLabel("Qt 避坑指南 - 常见错误与最佳实践");
    titleLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #C0392B;");
    mainLayout->addWidget(titleLabel);

    // 文本显示区域
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setReadOnly(true);
    textEdit->setFont(QFont("Courier New", 10));

    QString content = R"(
╔══════════════════════════════════════════════════════════════════════════════╗
║                          Qt 避坑指南：最常见的错误                            ║
╚══════════════════════════════════════════════════════════════════════════════╝

────────────────────────────────────────────────────────────────────────────────
【错误 1】忘记在 pro 文件中加 widgets 模块
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  QT += core gui

✅ 正确做法：
  QT += core gui widgets    # Qt5/Qt6 必须加 widgets

📌 后果：无法使用 QWidget、QPushButton、QLabel 等控件，编译会出错


────────────────────────────────────────────────────────────────────────────────
【错误 2】手动使用 move() 定位控件
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  button1->move(100, 50);
  button2->move(200, 100);
  button3->move(300, 150);
  // 窗口缩放时控件位置会乱掉！

✅ 正确做法：
  QVBoxLayout *layout = new QVBoxLayout;
  layout->addWidget(button1);
  layout->addWidget(button2);
  layout->addWidget(button3);
  window.setLayout(layout);
  // 窗口缩放时控件会自动调整

📌 后果：窗口缩放后界面混乱，控件重叠或消失


────────────────────────────────────────────────────────────────────────────────
【错误 3】忘记加 Q_OBJECT 宏
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  class MyWidget : public QWidget
  {
      // 缺少 Q_OBJECT
  public:
      MyWidget();
  private slots:
      void onButtonClicked();
  };

✅ 正确做法：
  class MyWidget : public QWidget
  {
      Q_OBJECT    // 必须加这一行
  public:
      MyWidget();
  private slots:
      void onButtonClicked();
  };

📌 后果：信号槽连接失败，槽函数不会被调用


────────────────────────────────────────────────────────────────────────────────
【错误 4】new 的控件没有指定父对象
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  QPushButton *btn = new QPushButton("按钮");
  // 窗口关闭时，btn 不会被自动销毁 → 内存泄漏

✅ 正确做法：
  QPushButton *btn = new QPushButton("按钮", this);  // this 是父对象
  // 窗口关闭时，btn 会自动被 this 销毁

📌 后果：内存泄漏，长期运行程序会越来越慢


────────────────────────────────────────────────────────────────────────────────
【错误 5】Lambda 捕获生命周期问题
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  QPushButton *btn = new QPushButton;
  QLabel *label = nullptr;  // 这个对象被销毁了
  
  connect(btn, &QPushButton::clicked, this, [label]() {
      label->setText("clicked");  // 访问已销毁的对象！
  });

✅ 正确做法：
  QPushButton *btn = new QPushButton("按钮", this);
  QLabel *label = new QLabel("标签", this);
  
  connect(btn, &QPushButton::clicked, this, [label]() {
      label->setText("点击了按钮");  // label 仍然存在
  });

📌 后果：程序崩溃，提示访问无效的内存


────────────────────────────────────────────────────────────────────────────────
【错误 6】信号槽的参数类型不匹配
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  // QLineEdit::textChanged() 信号携带 QString 参数
  // 但槽函数不接收参数
  void onTextChanged(int value) { }  // 参数类型错误
  
  connect(edit, &QLineEdit::textChanged, this, &MyWidget::onTextChanged);
  // 连接会失败！

✅ 正确做法：
  void onTextChanged(const QString &text) { }  // 参数类型匹配
  
  connect(edit, &QLineEdit::textChanged, this, &MyWidget::onTextChanged);
  // 连接成功

📌 后果：信号槽连接失败，槽函数不会被触发


────────────────────────────────────────────────────────────────────────────────
【错误 7】在循环中 new 对象但没有保存指针
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  for (int i = 0; i < 5; i++) {
      new QPushButton(QString::number(i), this);
      // 只 new 了，没有保存指针，无法修改或响应这个按钮
  }

✅ 正确做法：
  QList<QPushButton*> buttons;
  for (int i = 0; i < 5; i++) {
      QPushButton *btn = new QPushButton(QString::number(i), this);
      buttons.append(btn);
      connect(btn, &QPushButton::clicked, this, [btn]() {
          qDebug() << "点击了按钮" << btn->text();
      });
  }

📌 后果：无法对创建的控件进行操作


────────────────────────────────────────────────────────────────────────────────
【错误 8】忘记调用 show()
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  QWidget window;
  // 忘记调用 show()
  return app.exec();
  // 窗口不显示！

✅ 正确做法：
  QWidget window;
  window.show();  // 必须调用
  return app.exec();

📌 后果：程序运行但看不到任何窗口


────────────────────────────────────────────────────────────────────────────────
【错误 9】布局设置后又用 move() 和 resize()
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  layout->addWidget(button);
  window.setLayout(layout);
  button->move(100, 50);  // 与布局冲突，结果不可预测！
  button->resize(200, 100);

✅ 正确做法：
  layout->addWidget(button);
  window.setLayout(layout);
  // 让布局管理大小和位置，不要手动干预

📌 后果：布局管理混乱，界面显示错误


────────────────────────────────────────────────────────────────────────────────
【错误 10】使用全局指针管理 UI 控件
────────────────────────────────────────────────────────────────────────────────

❌ 错误做法：
  // global.h
  extern QPushButton *globalBtn;
  extern QLabel *globalLabel;
  // 全局指针容易出 bug，难以维护

✅ 正确做法：
  class MyWindow : public QWidget
  {
  private:
      QPushButton *btn;  // 成员变量
      QLabel *label;
  };
  // 封装在类中，易于管理

📌 后果：代码难以维护，容易出现指针悬空的问题


════════════════════════════════════════════════════════════════════════════════
                              【推荐编码习惯】
════════════════════════════════════════════════════════════════════════════════

1. ✅ 所有 new 出来的控件都指定父对象 this
   例：new QPushButton("按钮", this);

2. ✅ 始终使用布局管理控件，不要用 move()/resize()
   例：layout->addWidget(button);

3. ✅ 输入验证在前端完成
   例：检查空字符串、范围检查等

4. ✅ 使用 Lambda 连接简单信号槽
   例：connect(btn, &QPushButton::clicked, this, [this]() { ... });

5. ✅ 复杂逻辑使用自定义槽函数
   例：在 private slots: 中定义槽函数

6. ✅ 控件命名规范
   xxxBtn（按钮）、xxxEdit（编辑框）、xxxLabel（标签）、xxxLayout（布局）

7. ✅ 信号槽参数类型必须匹配
   例：信号为 valueChanged(int)，槽为 void onChanged(int)

8. ✅ 避免在栈上创建需要长期保存的对象
   例：不要 QWidget w; 然后在函数外使用


════════════════════════════════════════════════════════════════════════════════
)";

    textEdit->setText(content);
    mainLayout->addWidget(textEdit);

    // 底部提示
    QLabel *tipLabel = new QLabel(
        "💡 提示：这些是 Qt 初学者最常犯的错误。牢记这些避坑指南，能让你少走很多弯路！"
    );
    tipLabel->setStyleSheet("background-color: #D5F4E6; padding: 10px; border-radius: 3px; color: #27AE60; font-weight: bold;");
    tipLabel->setWordWrap(true);
    mainLayout->addWidget(tipLabel);

    setLayout(mainLayout);
}
