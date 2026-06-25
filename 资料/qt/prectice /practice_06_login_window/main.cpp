// 第6章 综合实战：登录窗口
// 笔记中的完整登录窗口代码

#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QMessageBox>
#include <QFont>

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    LoginWidget(QWidget *parent = nullptr) : QWidget(parent)
    {
        setWindowTitle("用户登录");
        setFixedSize(400, 300);

        titleLabel = new QLabel("欢迎登录系统");
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setFont(QFont("微软雅黑", 18, QFont::Bold));

        usernameLabel = new QLabel("用户名:");
        usernameEdit = new QLineEdit;
        usernameEdit->setPlaceholderText("请输入用户名");

        passwordLabel = new QLabel("密  码:");
        passwordEdit = new QLineEdit;
        passwordEdit->setPlaceholderText("请输入密码");
        passwordEdit->setEchoMode(QLineEdit::Password);

        rememberCheck = new QCheckBox("记住密码");

        loginBtn = new QPushButton("登  录");
        clearBtn = new QPushButton("清  空");
        loginBtn->setMinimumHeight(35);
        clearBtn->setMinimumHeight(35);

        QFormLayout *formLayout = new QFormLayout;
        formLayout->addRow(usernameLabel, usernameEdit);
        formLayout->addRow(passwordLabel, passwordEdit);
        formLayout->addRow("", rememberCheck);

        QHBoxLayout *btnLayout = new QHBoxLayout;
        btnLayout->addStretch();
        btnLayout->addWidget(loginBtn);
        btnLayout->addSpacing(20);
        btnLayout->addWidget(clearBtn);
        btnLayout->addStretch();

        QVBoxLayout *mainLayout = new QVBoxLayout;
        mainLayout->addSpacing(20);
        mainLayout->addWidget(titleLabel);
        mainLayout->addSpacing(20);
        mainLayout->addLayout(formLayout);
        mainLayout->addSpacing(10);
        mainLayout->addLayout(btnLayout);
        mainLayout->addStretch();

        setLayout(mainLayout);

        connect(loginBtn, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
        connect(clearBtn, &QPushButton::clicked, this, &LoginWidget::onClearClicked);
        connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginWidget::onLoginClicked);
    }

private slots:
    void onLoginClicked()
    {
        QString username = usernameEdit->text().trimmed();
        QString password = passwordEdit->text();

        if (username.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入用户名！");
            usernameEdit->setFocus();
            return;
        }

        if (password.isEmpty()) {
            QMessageBox::warning(this, "提示", "请输入密码！");
            passwordEdit->setFocus();
            return;
        }

        if (username == "admin" && password == "123456") {
            QMessageBox::information(this, "登录成功", "欢迎回来，管理员！");
        } else {
            QMessageBox::critical(this, "登录失败", "用户名或密码错误！");
            passwordEdit->clear();
            passwordEdit->setFocus();
        }
    }

    void onClearClicked()
    {
        usernameEdit->clear();
        passwordEdit->clear();
        usernameEdit->setFocus();
    }

private:
    QLabel *titleLabel;
    QLabel *usernameLabel;
    QLabel *passwordLabel;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QCheckBox *rememberCheck;
    QPushButton *loginBtn;
    QPushButton *clearBtn;
};

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    LoginWidget login;
    login.show();

    return app.exec();
}

#include "main.moc"