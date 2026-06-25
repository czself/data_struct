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