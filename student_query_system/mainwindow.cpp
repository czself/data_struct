#include "mainwindow.h"

#include "sqlexecutor.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QShortcut>
#include <QTableWidgetItem>
#include <QTableWidget>
#include <QTextEdit>

MainWindow::MainWindow()
{
    m_window = new QWidget;
    m_sqlExecutor = new SqlExecutor;

    initUi();
    initStyle();

    QString errorMessage;
    if (!m_sqlExecutor->load(&errorMessage)) {
        m_statusLabel->setText(errorMessage);
    }
}

MainWindow::~MainWindow()
{
    delete m_sqlExecutor;
    delete m_window;
}

void MainWindow::show()
{
    m_window->show();
}

void MainWindow::initUi()
{
    m_window->setWindowTitle("学生学习管理系统");
    m_window->resize(1100, 680);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧功能栏：只放本系统需要的功能，不放“源代码管理”
    QWidget *sideBar = new QWidget;
    sideBar->setFixedWidth(62);
    sideBar->setObjectName("sideBar");

    QVBoxLayout *sideLayout = new QVBoxLayout;
    sideLayout->setContentsMargins(8, 14, 8, 14);
    sideLayout->setSpacing(10);

    m_btnQuery   = createActivityButton("SQL", true);   // SQL 查询
    m_btnStudent = createActivityButton("生", false);    // tab_student.csv
    m_btnCourse  = createActivityButton("课", false);    // tab_course.csv
    m_btnClass   = createActivityButton("班", false);    // tab_class.csv
    m_btnScore   = createActivityButton("分", false);    // tab_score.csv

    sideLayout->addWidget(m_btnQuery);
    sideLayout->addWidget(m_btnStudent);
    sideLayout->addWidget(m_btnCourse);
    sideLayout->addWidget(m_btnClass);
    sideLayout->addWidget(m_btnScore);
    sideLayout->addStretch();
    sideBar->setLayout(sideLayout);

    // 右侧内容区
    QWidget *content = new QWidget;
    content->setObjectName("content");

    QVBoxLayout *contentLayout = new QVBoxLayout;
    contentLayout->setContentsMargins(18, 16, 18, 14);
    contentLayout->setSpacing(12);

    QLabel *titleLabel = new QLabel("学生学习管理系统");
    titleLabel->setObjectName("titleLabel");
    contentLayout->addWidget(titleLabel);

    QLabel *fileLabel = new QLabel("数据来源：tab_student.csv  tab_course.csv  tab_class.csv  tab_score.csv");
    fileLabel->setObjectName("fileLabel");
    contentLayout->addWidget(fileLabel);

    // SQL 输入区域：后面在这里读取用户输入的 SQL 语句
    QGroupBox *sqlGroup = new QGroupBox("SQL 查询");
    QVBoxLayout *sqlLayout = new QVBoxLayout;

    m_sqlEdit = new QTextEdit;
    m_sqlEdit->setPlaceholderText("请输入 SQL 查询语句");
    m_sqlEdit->setFixedHeight(120);

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    m_btnRun = new QPushButton("执行查询");
    m_btnClear = new QPushButton("清空");

    // 使用 lambda 调用类里的函数，这样 MainWindow 不需要继承 QWidget
    QObject::connect(m_btnRun, &QPushButton::clicked, [this]() {
        onRunSql();
    });
    QShortcut *enterShortcut = new QShortcut(QKeySequence(Qt::Key_Return), m_window);
    enterShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(enterShortcut, &QShortcut::activated, [this]() {
        onRunSql();
    });

    QShortcut *keypadEnterShortcut = new QShortcut(QKeySequence(Qt::Key_Enter), m_window);
    keypadEnterShortcut->setContext(Qt::WidgetWithChildrenShortcut);
    QObject::connect(keypadEnterShortcut, &QShortcut::activated, [this]() {
        onRunSql();
    });

    QObject::connect(m_btnClear, &QPushButton::clicked, [this]() {
        onClearSql();
    });
    QObject::connect(m_btnQuery, &QPushButton::clicked, [this]() {
        setActivityButtonChecked(m_btnQuery);
        m_statusLabel->setText("SQL 查询模式");
    });
    QObject::connect(m_btnStudent, &QPushButton::clicked, [this]() {
        onShowTable("student");
    });
    QObject::connect(m_btnCourse, &QPushButton::clicked, [this]() {
        onShowTable("course");
    });
    QObject::connect(m_btnClass, &QPushButton::clicked, [this]() {
        onShowTable("class");
    });
    QObject::connect(m_btnScore, &QPushButton::clicked, [this]() {
        onShowTable("score");
    });

    buttonLayout->addStretch();
    buttonLayout->addWidget(m_btnRun);
    buttonLayout->addWidget(m_btnClear);

    sqlLayout->addWidget(m_sqlEdit);
    sqlLayout->addLayout(buttonLayout);
    sqlGroup->setLayout(sqlLayout);
    contentLayout->addWidget(sqlGroup);

    // 查询结果区域：这里只搭 UI，表格内容等 SQL 解析完成后再填充
    QGroupBox *resultGroup = new QGroupBox("查询结果");
    QVBoxLayout *resultLayout = new QVBoxLayout;

    m_resultTable = new QTableWidget;
    m_resultTable->setRowCount(0);
    m_resultTable->setColumnCount(0);
    m_resultTable->horizontalHeader()->setStretchLastSection(true);

    resultLayout->addWidget(m_resultTable);
    resultGroup->setLayout(resultLayout);
    contentLayout->addWidget(resultGroup);

    m_statusLabel = new QLabel("就绪");
    m_statusLabel->setObjectName("statusLabel");
    contentLayout->addWidget(m_statusLabel);

    content->setLayout(contentLayout);

    mainLayout->addWidget(sideBar);
    mainLayout->addWidget(content);
    m_window->setLayout(mainLayout);
}

void MainWindow::initStyle()
{
    // 暗黑色系样式：用资料中已经出现过的 setStyleSheet 写法
    m_window->setStyleSheet(
        "QWidget { background-color: #20232A; }"
        "QWidget#sideBar { background-color: #171A21; }"
        "QWidget#content { background-color: #20232A; color: #DCDCDC; }"
        "QLabel#titleLabel { color: #FFFFFF; font-size: 22px; font-weight: bold; }"
        "QLabel#fileLabel { color: #9DA5B4; font-size: 13px; }"
        "QLabel#statusLabel { color: #9DA5B4; padding-top: 2px; }"
        "QGroupBox { color: #DCDCDC; border: 1px solid #3A3F4B; margin-top: 8px; padding: 8px; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 10px; padding: 0 4px; }"
        "QTextEdit { background-color: #15171C; color: #E6E6E6; border: 1px solid #3A3F4B; padding: 8px; }"
        "QPushButton { background-color: #2D333F; color: #E6E6E6; border: 1px solid #4B5363; padding: 7px 16px; }"
        "QPushButton:hover { background-color: #3A4252; }"
        "QTableWidget { background-color: #15171C; color: #E6E6E6; gridline-color: #3A3F4B; border: 1px solid #3A3F4B; }"
        "QHeaderView::section { background-color: #2B303A; color: #FFFFFF; padding: 6px; border: 1px solid #3A3F4B; }"
    );
}

QPushButton *MainWindow::createActivityButton(const QString &text, bool checked)
{
    QPushButton *button = new QPushButton(text);
    button->setFixedSize(46, 42);
    button->setCheckable(true);
    button->setChecked(checked);

    button->setStyleSheet(
        "QPushButton { background-color: #171A21; color: #AEB6C5; border: none; font-weight: bold; }"
        "QPushButton:hover { background-color: #252A34; color: #FFFFFF; }"
        "QPushButton:checked { background-color: #2D333F; color: #FFFFFF; border-left: 3px solid #4FC3F7; }"
    );

    return button;
}

void MainWindow::onRunSql()
{
    QString sql = m_sqlEdit->toPlainText().trimmed();

    if (sql.isEmpty()) {
        m_statusLabel->setText("请输入 SQL 查询语句");
        return;
    }

    SqlResult result = m_sqlExecutor->execute(sql);
    if (result.needsConfirm) {
        const QMessageBox::StandardButton answer = QMessageBox::question(
            m_window,
            "确认删除",
            result.message + "，确定继续吗？",
            QMessageBox::Yes | QMessageBox::No,
            QMessageBox::No
        );

        if (answer != QMessageBox::Yes) {
            m_statusLabel->setText("已取消删除");
            return;
        }

        result = m_sqlExecutor->execute(sql, true);
    }

    showSqlResult(result);
}

void MainWindow::onClearSql()
{
    m_sqlEdit->clear();
    m_resultTable->clear();
    m_resultTable->setRowCount(0);
    m_resultTable->setColumnCount(0);
    m_statusLabel->setText("已清空");
}

void MainWindow::onShowTable(const QString &tableName)
{
    if (tableName == "student") {
        setActivityButtonChecked(m_btnStudent);
    } else if (tableName == "course") {
        setActivityButtonChecked(m_btnCourse);
    } else if (tableName == "class") {
        setActivityButtonChecked(m_btnClass);
    } else if (tableName == "score") {
        setActivityButtonChecked(m_btnScore);
    }

    showSqlResult(m_sqlExecutor->showTable(tableName));
}

void MainWindow::showSqlResult(const SqlResult &result)
{
    if (!result.success) {
        m_statusLabel->setText(result.message);
        return;
    }

    m_resultTable->clear();
    m_resultTable->setColumnCount(result.headers.size());
    m_resultTable->setRowCount(result.rows.size());
    m_resultTable->setHorizontalHeaderLabels(result.headers);

    for (int row = 0; row < result.rows.size(); ++row) {
        const QStringList rowData = result.rows.at(row);
        for (int column = 0; column < result.headers.size(); ++column) {
            const QString text = column < rowData.size() ? rowData.at(column) : "";
            QTableWidgetItem *item = new QTableWidgetItem(text);
            m_resultTable->setItem(row, column, item);
        }
    }

    m_resultTable->resizeColumnsToContents();
    m_statusLabel->setText(result.message);
}

void MainWindow::setActivityButtonChecked(QPushButton *checkedButton)
{
    m_btnQuery->setChecked(m_btnQuery == checkedButton);
    m_btnStudent->setChecked(m_btnStudent == checkedButton);
    m_btnCourse->setChecked(m_btnCourse == checkedButton);
    m_btnClass->setChecked(m_btnClass == checkedButton);
    m_btnScore->setChecked(m_btnScore == checkedButton);
}
