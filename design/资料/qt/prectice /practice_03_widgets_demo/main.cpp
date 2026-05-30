// 第3章 常用控件详解
// 笔记中各控件的独立代码片段

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QCheckBox>
#include <QComboBox>
#include <QListWidget>
#include <QTableWidget>
#include <QMessageBox>
#include <QTimer>
#include <QDebug>
#include <QFont>
#include <QValidator>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QFile>
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("第3章 - 常用控件详解");
    window.resize(600, 800);

    QVBoxLayout *layout = new QVBoxLayout(&window);

    // ── 3.1 QPushButton ──
    QPushButton *btn = new QPushButton("Hello Qt");
    btn->resize(300, 100);
    btn->setFont(QFont("微软雅黑", 16));
    btn->setStyleSheet("background-color: lightblue;");
    layout->addWidget(btn);

    // ── 3.2 QLabel ──
    QLabel *label = new QLabel("<h2 style='color:red;'>你好，Qt！</h2>");
    label->setAlignment(Qt::AlignCenter);
    layout->addWidget(label);
    QLabel *numLabel = new QLabel;
    numLabel->setNum(42);
    layout->addWidget(numLabel);
    QLabel *numLabel2 = new QLabel;
    numLabel2->setNum(3.14159);
    layout->addWidget(numLabel2);

    // ── 3.3 QLineEdit ──
    QLineEdit *edit = new QLineEdit;
    edit->setPlaceholderText("请输入用户名");
    layout->addWidget(edit);
    QLineEdit *passEdit = new QLineEdit;
    passEdit->setPlaceholderText("密码");
    passEdit->setEchoMode(QLineEdit::Password);
    layout->addWidget(passEdit);

    // ── 3.4 QTextEdit ──
    QTextEdit *editor = new QTextEdit;
    editor->setPlainText("普通文本内容");
    editor->setMaximumHeight(100);
    layout->addWidget(editor);

    // ── 3.5 QCheckBox ──
    QCheckBox *check = new QCheckBox("记住密码");
    layout->addWidget(check);
    QCheckBox *check2 = new QCheckBox("同意协议");
    check2->setChecked(true);
    layout->addWidget(check2);

    // ── 3.6 QComboBox ──
    QComboBox *combo = new QComboBox;
    combo->addItems({"男", "女", "保密"});
    layout->addWidget(combo);

    // ── 3.7 QListWidget ──
    QListWidget *list = new QListWidget;
    list->addItem("第一项");
    list->addItem("第二项");
    list->addItem("第三项");
    list->setMaximumHeight(100);
    layout->addWidget(list);

    // ── 3.8 QTableWidget ──
    QTableWidget *table = new QTableWidget(3, 4);
    table->setHorizontalHeaderLabels({"姓名", "年龄", "性别", "成绩"});
    table->setItem(0, 0, new QTableWidgetItem("张三"));
    table->setItem(0, 1, new QTableWidgetItem("20"));
    table->setItem(0, 2, new QTableWidgetItem("男"));
    table->setItem(0, 3, new QTableWidgetItem("95"));
    table->setMaximumHeight(130);
    layout->addWidget(table);

    // ── 3.9 QMessageBox ──
    QPushButton *msgBtn = new QPushButton("点击弹出信息框");
    QObject::connect(msgBtn, &QPushButton::clicked, [&]() {
        QMessageBox::information(&window, "标题", "操作成功！");
    });
    layout->addWidget(msgBtn);

    // ── 3.10 QTimer ──
    QTimer *timer = new QTimer(&window);
    QLabel *timerLabel = new QLabel("定时器未启动");
    QPushButton *timerBtn = new QPushButton("启动定时器(1秒)");
    QObject::connect(timerBtn, &QPushButton::clicked, [=]() {
        timer->start(1000);
        timerLabel->setText("定时器已启动");
    });
    QObject::connect(timer, &QTimer::timeout, [=]() {
        qDebug() << "定时器触发了！";
    });
    layout->addWidget(timerBtn);
    layout->addWidget(timerLabel);

    // ── 3.11 QFile ──
    QPushButton *fileBtn = new QPushButton("写入文件 data.txt");
    QObject::connect(fileBtn, &QPushButton::clicked, [&]() {
        QFile file("data.txt");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&file);
            out << "要写入的内容" << "\n";
            file.close();
        }
    });
    layout->addWidget(fileBtn);

    window.setLayout(layout);
    window.show();
    return app.exec();
}