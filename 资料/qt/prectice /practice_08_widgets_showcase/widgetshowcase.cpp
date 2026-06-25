#include "widgetshowcase.h"
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QLabel>
#include <QFont>

WidgetShowcase::WidgetShowcase(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("第8章 - 常用控件速查表");
    setGeometry(50, 50, 1200, 700);

    QVBoxLayout *main = new QVBoxLayout;

    QLabel *title = new QLabel("Qt 常用控件速查表");
    title->setStyleSheet("font-size: 16px; font-weight: bold;");
    main->addWidget(title);

    QTableWidget *table = new QTableWidget;
    table->setColumnCount(4);
    table->setHorizontalHeaderLabels({"控件类名", "用  途", "常用信号", "示例代码"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setAlternatingRowColors(true);

    QList<QStringList> data = {
        {"QWidget", "基础窗口", "—", "QWidget window;"},
        {"QPushButton", "按钮", "clicked()", "new QPushButton()"},
        {"QLabel", "文本显示", "—", "new QLabel()"},
        {"QLineEdit", "单行输入", "textChanged()", "new QLineEdit()"},
        {"QTextEdit", "多行文本", "textChanged()", "new QTextEdit()"},
        {"QCheckBox", "复选框", "stateChanged()", "new QCheckBox()"},
        {"QComboBox", "下拉框", "currentIndexChanged()", "new QComboBox()"},
        {"QListWidget", "列表", "itemClicked()", "new QListWidget()"},
        {"QTableWidget", "表格", "cellClicked()", "new QTableWidget()"},
        {"QTimer", "定时器", "timeout()", "new QTimer()"},
        {"QFile", "文件操作", "—", "QFile file()"},
        {"QVBoxLayout", "垂直布局", "—", "new QVBoxLayout()"},
    };

    table->setRowCount(data.count());
    for(int i=0; i<data.count(); i++) {
        for(int j=0; j<4; j++) {
            table->setItem(i, j, new QTableWidgetItem(data[i][j]));
        }
    }

    main->addWidget(table);

    QLabel *info = new QLabel(
        "说明：所有 new 出来的控件都指定父对象 (this) "
        "| 使用布局管理控件，不要用 move() "
        "| 自定义槽函数需要 Q_OBJECT 宏"
    );
    info->setStyleSheet("background-color: #FFF3CD; padding: 10px;");
    info->setWordWrap(true);
    main->addWidget(info);

    setLayout(main);
}
