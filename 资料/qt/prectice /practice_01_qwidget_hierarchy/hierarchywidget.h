#ifndef HIERARCHYWIDGET_H
#define HIERARCHYWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>

class HierarchyWidget : public QWidget
{
    Q_OBJECT

public:
    HierarchyWidget(QWidget *parent = nullptr);

private slots:
    void onShowHierarchy();

private:
    void setupUI();
    QTextEdit *textDisplay;
    QPushButton *showBtn;
};

#endif // HIERARCHYWIDGET_H
