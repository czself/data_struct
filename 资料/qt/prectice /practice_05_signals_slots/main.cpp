// 第5章 信号槽机制
// 笔记中各信号槽的独立代码片段

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDebug>
#include <QVBoxLayout>
#include <QHBoxLayout>

// ── 5.6 自定义槽函数（含Q_OBJECT，放在cpp中） ──
// 笔记原文：
//   class Widget : public QWidget
//   {
//       Q_OBJECT
//   public:
//       Widget(QWidget *parent = nullptr);
//   private slots:
//       void onLoginClicked();
//       void onTextChanged(const QString &text);
//   private:
//       QPushButton *loginBtn;
//       QLineEdit *usernameEdit;
//   };

class DemoWidget : public QWidget
{
    Q_OBJECT
public:
    DemoWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        loginBtn = new QPushButton("登录", this);
        usernameEdit = new QLineEdit(this);

        connect(loginBtn, &QPushButton::clicked, this, &DemoWidget::onLoginClicked);
        connect(usernameEdit, &QLineEdit::textChanged, this, &DemoWidget::onTextChanged);
    }

private slots:
    void onLoginClicked()
    {
        qDebug() << "登录按钮被点击了";
    }

    void onTextChanged(const QString &text)
    {
        qDebug() << "输入框内容变了:" << text;
    }

private:
    QPushButton *loginBtn;
    QLineEdit *usernameEdit;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("第5章 - 信号槽机制");
    window.resize(500, 400);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    // ── 5.3 标准信号槽：按钮点击关闭窗口 ──
    // 笔记原文：
    //   connect(button, &QPushButton::clicked,   // 信号：按钮被点击
    //           this,  &QWidget::close);         // 槽：关闭窗口

    QPushButton *closeBtn = new QPushButton("点击关闭窗口（5.3 标准信号槽）");
    QObject::connect(closeBtn, &QPushButton::clicked, &window, &QWidget::close);
    layout->addWidget(closeBtn);

    // ── 5.5 Lambda 表达式 ──
    // 笔记原文：
    //   QPushButton *btn = new QPushButton("点我", this);
    //   QLabel *label = new QLabel("隐藏的文本", this);
    //   label->hide();
    //   connect(btn, &QPushButton::clicked, this, [=]() {
    //       label->show();
    //       label->setText("按钮被点击了！");
    //   });

    QPushButton *showBtn = new QPushButton("点击显示文本（5.5 Lambda）");
    QLabel *hiddenLabel = new QLabel("隐藏的文本 — 按钮被点击了！");
    hiddenLabel->hide();
    QObject::connect(showBtn, &QPushButton::clicked, [=]() {
        hiddenLabel->show();
        hiddenLabel->setText("按钮被点击了！");
    });
    layout->addWidget(showBtn);
    layout->addWidget(hiddenLabel);

    // ── 5.6 自定义槽函数 ──
    // 笔记原文中给出了完整的 Widget 类（含 Q_OBJECT + 槽函数）
    // 在 main.cpp 顶部定义了 DemoWidget 类

    DemoWidget *demo = new DemoWidget;
    demo->setWindowTitle("5.6 自定义槽函数演示");
    demo->resize(300, 100);
    layout->addWidget(demo);

    // ── 5.7 多对一连接 ──
    // 笔记原文：
    //   connect(btn1, &QPushButton::clicked, this, &Widget::onButtonClicked);
    //   connect(btn2, &QPushButton::clicked, this, &Widget::onButtonClicked);
    //   connect(btn3, &QPushButton::clicked, this, &Widget::onButtonClicked);
    //   void Widget::onButtonClicked()
    //   {
    //       QPushButton *btn = qobject_cast<QPushButton*>(sender());
    //       if (btn) qDebug() << "点击了:" << btn->text();
    //   }

    QPushButton *btn1 = new QPushButton("按钮1");
    QPushButton *btn2 = new QPushButton("按钮2");
    QPushButton *btn3 = new QPushButton("按钮3");
    QLabel *senderLabel = new QLabel("点击按钮，用 sender() 识别来源");
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(btn1);
    hLayout->addWidget(btn2);
    hLayout->addWidget(btn3);
    layout->addLayout(hLayout);
    layout->addWidget(senderLabel);

    for (QPushButton *btn : {btn1, btn2, btn3}) {
        QObject::connect(btn, &QPushButton::clicked, [senderLabel, btn]() {
            senderLabel->setText("点击了: " + btn->text());
            qDebug() << "点击了:" << btn->text();
        });
    }

    window.setLayout(layout);
    window.show();
    return app.exec();
}

#include "main.moc"