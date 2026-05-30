#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QString>

class QLabel;
class QPushButton;
class QTableWidget;
class QTextEdit;
class QWidget;
class SqlExecutor;
struct SqlResult;

class MainWindow
{
public:
    MainWindow();
    ~MainWindow();

    void show();

private:
    void onRunSql();
    void onClearSql();
    void onShowTable(const QString &tableName);
    void showSqlResult(const SqlResult &result);
    void setActivityButtonChecked(QPushButton *checkedButton);

    void initUi();
    void initStyle();
    QPushButton *createActivityButton(const QString &text, bool checked);

private:
    QWidget *m_window;

    QPushButton *m_btnQuery;
    QPushButton *m_btnStudent;
    QPushButton *m_btnCourse;
    QPushButton *m_btnClass;
    QPushButton *m_btnScore;

    QPushButton *m_btnRun;
    QPushButton *m_btnClear;

    QTextEdit *m_sqlEdit;
    QTableWidget *m_resultTable;
    QLabel *m_statusLabel;

    SqlExecutor *m_sqlExecutor;
};

#endif
