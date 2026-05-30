#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class QLabel;
class QLineEdit;
class QPushButton;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = 0);

private:
    void initUi();
    void initStyle();
    void onLoginClicked();

private:
    QLabel *m_messageLabel;
    QLineEdit *m_accountEdit;
    QLineEdit *m_passwordEdit;
    QPushButton *m_loginButton;
    QPushButton *m_exitButton;
};

#endif
