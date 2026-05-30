#include "logindialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QVBoxLayout>

LoginDialog::LoginDialog(QWidget *parent)
    : QDialog(parent),
      m_messageLabel(0),
      m_accountEdit(0),
      m_passwordEdit(0),
      m_loginButton(0),
      m_exitButton(0)
{
    initUi();
    initStyle();
}

void LoginDialog::initUi()
{
    setWindowTitle("登录");
    setFixedSize(420, 300);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(34, 28, 34, 26);
    mainLayout->setSpacing(16);

    QLabel *titleLabel = new QLabel("欢迎来带数据结构课设");
    titleLabel->setObjectName("loginTitleLabel");
    titleLabel->setAlignment(Qt::AlignCenter);

    m_messageLabel = new QLabel;
    m_messageLabel->setObjectName("loginMessageLabel");
    m_messageLabel->setAlignment(Qt::AlignCenter);
    m_messageLabel->setFixedHeight(22);

    QLabel *accountLabel = new QLabel("帐号");
    accountLabel->setObjectName("loginFieldLabel");
    m_accountEdit = new QLineEdit;
    m_accountEdit->setPlaceholderText("请输入帐号");
    m_accountEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_]{0,16}"), this));

    QLabel *passwordLabel = new QLabel("密码");
    passwordLabel->setObjectName("loginFieldLabel");
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setPlaceholderText("请输入密码");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_passwordEdit->setValidator(new QRegExpValidator(QRegExp("[A-Za-z0-9_!@#$%^&*]{0,20}"), this));

    QHBoxLayout *accountLayout = new QHBoxLayout;
    accountLayout->setSpacing(12);
    accountLayout->addWidget(accountLabel);
    accountLayout->addWidget(m_accountEdit);

    QHBoxLayout *passwordLayout = new QHBoxLayout;
    passwordLayout->setSpacing(12);
    passwordLayout->addWidget(passwordLabel);
    passwordLayout->addWidget(m_passwordEdit);

    m_loginButton = new QPushButton("登录");
    m_exitButton = new QPushButton("退出");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_loginButton);
    buttonLayout->addWidget(m_exitButton);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(m_messageLabel);
    mainLayout->addLayout(accountLayout);
    mainLayout->addLayout(passwordLayout);
    mainLayout->addStretch();
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(m_loginButton, &QPushButton::clicked, this, &LoginDialog::onLoginClicked);
    connect(m_exitButton, &QPushButton::clicked, this, &LoginDialog::reject);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::onLoginClicked);
    connect(m_accountEdit, &QLineEdit::returnPressed, m_passwordEdit, QOverload<>::of(&QLineEdit::setFocus));
}

void LoginDialog::initStyle()
{
    setStyleSheet(
        "QDialog { background-color: #20232A; color: #DCDCDC; }"
        "QLabel#loginTitleLabel { color: #FFFFFF; font-size: 22px; font-weight: bold; }"
        "QLabel#loginMessageLabel { color: #FFB86C; font-size: 13px; }"
        "QLabel#loginFieldLabel { color: #DCDCDC; font-size: 14px; min-width: 36px; }"
        "QLineEdit { background-color: #15171C; color: #E6E6E6; border: 1px solid #3A3F4B; padding: 8px; }"
        "QLineEdit:focus { border: 1px solid #4FC3F7; }"
        "QPushButton { background-color: #2D333F; color: #E6E6E6; border: 1px solid #4B5363; padding: 8px 22px; }"
        "QPushButton:hover { background-color: #3A4252; }"
    );
}

void LoginDialog::onLoginClicked()
{
    const QString account = m_accountEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    QRegExp accountRegExp("^[A-Za-z0-9_]{2,16}$");
    QRegExp passwordRegExp("^[A-Za-z0-9_!@#$%^&*]{6,20}$");

    if (!accountRegExp.exactMatch(account) || !passwordRegExp.exactMatch(password)) {
        m_messageLabel->setText("账号或密码格式错误");
        return;
    }

    if (account == "sz" && password == "1qazsz") {
        accept();
        return;
    }

    m_messageLabel->setText("账号或密码错误");
}
