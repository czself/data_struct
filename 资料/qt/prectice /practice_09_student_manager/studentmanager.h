#ifndef STUDENTMANAGER_H
#define STUDENTMANAGER_H

#include <QWidget>
#include <QTableWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QLabel>

struct Student {
    QString id;
    QString name;
    int score;
};

class StudentManager : public QWidget
{
    Q_OBJECT
public:
    StudentManager(QWidget *parent = nullptr);
private slots:
    void onAddStudent();
    void onDeleteStudent();
    void onModifyStudent();
    void onStatistics();
    void onTableItemClicked(int row, int column);
private:
    void refreshTable();
    void clearInputs();

    QLineEdit *idEdit;
    QLineEdit *nameEdit;
    QSpinBox *scoreEdit;
    QTableWidget *table;
    QLabel *statsLabel;
    QList<Student> students;
};

#endif
