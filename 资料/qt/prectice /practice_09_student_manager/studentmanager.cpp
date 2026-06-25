#include "studentmanager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QMessageBox>
#include <QHeaderView>

StudentManager::StudentManager(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("第9章 - 学生管理系统");
    setGeometry(50, 50, 900, 600);

    QVBoxLayout *main = new QVBoxLayout;

    QLabel *title = new QLabel("学生管理系统");
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    main->addWidget(title);

    QGroupBox *inputGroup = new QGroupBox("学生信息输入");
    QFormLayout *form = new QFormLayout;
    idEdit = new QLineEdit;
    nameEdit = new QLineEdit;
    scoreEdit = new QSpinBox;
    scoreEdit->setRange(0, 100);
    form->addRow("学号:", idEdit);
    form->addRow("姓名:", nameEdit);
    form->addRow("成绩:", scoreEdit);
    inputGroup->setLayout(form);
    main->addWidget(inputGroup);

    QHBoxLayout *btnLayout = new QHBoxLayout;
    QPushButton *addBtn = new QPushButton("添加");
    QPushButton *deleteBtn = new QPushButton("删除");
    QPushButton *modifyBtn = new QPushButton("修改");
    QPushButton *statsBtn = new QPushButton("统计");
    QPushButton *clearBtn = new QPushButton("清空");
    connect(addBtn, &QPushButton::clicked, this, &StudentManager::onAddStudent);
    connect(deleteBtn, &QPushButton::clicked, this, &StudentManager::onDeleteStudent);
    connect(modifyBtn, &QPushButton::clicked, this, &StudentManager::onModifyStudent);
    connect(statsBtn, &QPushButton::clicked, this, &StudentManager::onStatistics);
    connect(clearBtn, &QPushButton::clicked, this, &StudentManager::clearInputs);
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(deleteBtn);
    btnLayout->addWidget(modifyBtn);
    btnLayout->addWidget(statsBtn);
    btnLayout->addStretch();
    btnLayout->addWidget(clearBtn);
    main->addLayout(btnLayout);

    table = new QTableWidget;
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"学号", "姓名", "成绩"});
    table->horizontalHeader()->setStretchLastSection(true);
    connect(table, &QTableWidget::cellClicked, this, &StudentManager::onTableItemClicked);
    main->addWidget(table);

    statsLabel = new QLabel("总人数：0");
    statsLabel->setStyleSheet("background-color: #ECF0F1; padding: 10px;");
    main->addWidget(statsLabel);

    setLayout(main);

    students << Student{"2024001", "张三", 85}
             << Student{"2024002", "李四", 92}
             << Student{"2024003", "王五", 78};
    refreshTable();
}

void StudentManager::onAddStudent()
{
    QString id = idEdit->text().trimmed();
    QString name = nameEdit->text().trimmed();
    int score = scoreEdit->value();

    if (id.isEmpty() || name.isEmpty()) {
        QMessageBox::warning(this, "提示", "学号和姓名不能为空!");
        return;
    }

    for (const auto &stu : students) {
        if (stu.id == id) {
            QMessageBox::warning(this, "提示", "学号已存在!");
            return;
        }
    }

    students.append(Student{id, name, score});
    refreshTable();
    clearInputs();
}

void StudentManager::onDeleteStudent()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择学生!");
        return;
    }
    students.removeAt(row);
    refreshTable();
}

void StudentManager::onModifyStudent()
{
    int row = table->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "提示", "请先选择学生!");
        return;
    }

    students[row].id = idEdit->text();
    students[row].name = nameEdit->text();
    students[row].score = scoreEdit->value();
    refreshTable();
    clearInputs();
}

void StudentManager::onStatistics()
{
    if (students.isEmpty()) {
        statsLabel->setText("总人数：0");
        return;
    }

    int maxScore = students[0].score;
    int minScore = students[0].score;
    double sum = 0;

    for (const auto &stu : students) {
        maxScore = qMax(maxScore, stu.score);
        minScore = qMin(minScore, stu.score);
        sum += stu.score;
    }

    QString stats = QString("总人数:%1 | 最高分:%2 | 最低分:%3 | 平均分:%.2f")
                        .arg(students.count())
                        .arg(maxScore)
                        .arg(minScore)
                        .arg(sum / students.count());
    statsLabel->setText(stats);
}

void StudentManager::onTableItemClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row >= 0 && row < students.count()) {
        idEdit->setText(students[row].id);
        nameEdit->setText(students[row].name);
        scoreEdit->setValue(students[row].score);
    }
}

void StudentManager::refreshTable()
{
    table->setRowCount(students.count());
    for (int i = 0; i < students.count(); i++) {
        table->setItem(i, 0, new QTableWidgetItem(students[i].id));
        table->setItem(i, 1, new QTableWidgetItem(students[i].name));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(students[i].score)));
    }
}

void StudentManager::clearInputs()
{
    idEdit->clear();
    nameEdit->clear();
    scoreEdit->setValue(0);
    table->clearSelection();
}
