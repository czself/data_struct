// 第4章 布局系统
// 笔记中各布局的独立代码片段

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QLineEdit>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("第4章 - 布局系统");
    window.resize(500, 600);

    QVBoxLayout *mainLayout = new QVBoxLayout(&window);

    // ── 4.4 QVBoxLayout（垂直布局） ──
    // 笔记原文：
    //   QVBoxLayout *layout = new QVBoxLayout;
    //   layout->addWidget(button1);
    //   layout->addWidget(button2);
    //   layout->addWidget(button3);
    //   window.setLayout(layout);

    QVBoxLayout *vLayout = new QVBoxLayout;
    vLayout->addWidget(new QPushButton("上"));
    vLayout->addWidget(new QPushButton("中"));
    vLayout->addWidget(new QPushButton("下"));
    QWidget vw;
    vw.setLayout(vLayout);
    mainLayout->addWidget(&vw);

    // ── 4.5 QHBoxLayout（水平布局） ──
    // 笔记原文：
    //   QHBoxLayout *layout = new QHBoxLayout;
    //   layout->addWidget(button1);
    //   layout->addWidget(button2);
    //   layout->addWidget(button3);
    //   window.setLayout(layout);

    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(new QPushButton("左"));
    hLayout->addWidget(new QPushButton("中"));
    hLayout->addWidget(new QPushButton("右"));
    QWidget hw;
    hw.setLayout(hLayout);
    mainLayout->addWidget(&hw);

    // ── 4.6 QGridLayout（网格布局） ──
    // 笔记原文：
    //   QGridLayout *layout = new QGridLayout;
    //   layout->addWidget(button1, 0, 0);
    //   layout->addWidget(button2, 0, 1);
    //   layout->addWidget(button3, 1, 0, 1, 2);
    //   window.setLayout(layout);

    QGridLayout *gLayout = new QGridLayout;
    gLayout->addWidget(new QPushButton("(0,0)"), 0, 0);
    gLayout->addWidget(new QPushButton("(0,1)"), 0, 1);
    gLayout->addWidget(new QPushButton("(1,0+1)跨两列"), 1, 0, 1, 2);
    QWidget gw;
    gw.setLayout(gLayout);
    mainLayout->addWidget(&gw);

    // ── 4.7 QFormLayout（表单布局） ──
    // 笔记原文：
    //   QFormLayout *layout = new QFormLayout;
    //   layout->addRow("用户名:", usernameEdit);
    //   layout->addRow("密  码:", passwordEdit);
    //   layout->addRow("", loginButton);

    QFormLayout *fLayout = new QFormLayout;
    fLayout->addRow("用户名:", new QLineEdit);
    fLayout->addRow("密  码:", new QLineEdit);
    fLayout->addRow("", new QPushButton("登录"));
    QWidget fw;
    fw.setLayout(fLayout);
    mainLayout->addWidget(&fw);

    // ── 4.8 布局嵌套 ──
    // 笔记原文：
    //   QVBoxLayout *mainLayout = new QVBoxLayout;
    //   QHBoxLayout *buttonLayout = new QHBoxLayout;
    //   buttonLayout->addWidget(okButton);
    //   buttonLayout->addWidget(cancelButton);
    //   mainLayout->addWidget(label);
    //   mainLayout->addLayout(buttonLayout);
    //   window.setLayout(mainLayout);

    QVBoxLayout *nestMain = new QVBoxLayout;
    nestMain->addWidget(new QLabel("这是一个标签"));
    QHBoxLayout *nestBtn = new QHBoxLayout;
    nestBtn->addWidget(new QPushButton("确定"));
    nestBtn->addWidget(new QPushButton("取消"));
    nestMain->addLayout(nestBtn);
    QWidget nw;
    nw.setLayout(nestMain);
    mainLayout->addWidget(&nw);

    // ── 4.10 弹簧示例 ──
    // 笔记原文：
    //   QHBoxLayout *layout = new QHBoxLayout;
    //   layout->addStretch();
    //   layout->addWidget(button);
    //   window.setLayout(layout);

    QHBoxLayout *spring1 = new QHBoxLayout;
    spring1->addStretch();
    spring1->addWidget(new QPushButton("右对齐按钮"));
    QWidget s1w;
    s1w.setLayout(spring1);
    mainLayout->addWidget(&s1w);

    // 笔记原文：
    //   QHBoxLayout *layout = new QHBoxLayout;
    //   layout->addWidget(button1);
    //   layout->addStretch();
    //   layout->addWidget(button2);
    //   window.setLayout(layout);

    QHBoxLayout *spring2 = new QHBoxLayout;
    spring2->addWidget(new QPushButton("左侧"));
    spring2->addStretch();
    spring2->addWidget(new QPushButton("右侧"));
    QWidget s2w;
    s2w.setLayout(spring2);
    mainLayout->addWidget(&s2w);

    window.setLayout(mainLayout);
    window.show();
    return app.exec();
}