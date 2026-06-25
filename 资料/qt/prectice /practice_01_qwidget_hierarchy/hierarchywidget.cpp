#include "hierarchywidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFont>

HierarchyWidget::HierarchyWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();

    setWindowTitle("第1章练习 - QWidget体系继承关系");
    setGeometry(100, 100, 700, 600);

    onShowHierarchy();
}

void HierarchyWidget::setupUI()
{
    // 标题标签
    QLabel *titleLabel = new QLabel("QWidget 继承体系树");
    titleLabel->setFont(QFont("微软雅黑", 14, QFont::Bold));
    titleLabel->setAlignment(Qt::AlignCenter);

    // 文本显示区域
    textDisplay = new QTextEdit;
    textDisplay->setReadOnly(true);
    textDisplay->setFont(QFont("Courier New", 10));

    // 按钮
    showBtn = new QPushButton("显示继承关系");
    connect(showBtn, &QPushButton::clicked, this, &HierarchyWidget::onShowHierarchy);

    // 布局
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(titleLabel);
    layout->addWidget(textDisplay);
    layout->addWidget(showBtn);

    setLayout(layout);
}

void HierarchyWidget::onShowHierarchy()
{
    QString hierarchy = R"(
═══════════════════════════════════════════════════════════════
                    QWidget 继承体系完全图
═══════════════════════════════════════════════════════════════

                          QObject
                            │
                            └─── QWidget (所有UI控件的基类)
                                   │
                    ┌──────────────┼──────────────┬───────────┐
                    │              │              │           │
              【输入控件】     【显示控件】   【选择控件】   【容器】
                    │              │              │           │
            ┌───────┴───────┐  ┌────┴────┐  ┌────┴───┐   ┌───┴────┐
        QLineEdit   QTextEdit QLabel  QTextBrowser QCheckBox QFrame
                               │                    │
                          ┌────┴────┐           QComboBox
                       QPixmap   QMovie
                             
【数据显示】                【列表/表格】          【按钮】
    │                           │                    │
 ┌─────┐                   ┌────┴────┐          QPushButton
QImage               QListWidget  QTableWidget  QToolButton


═══════════════════════════════════════════════════════════════
                        核心特性说明
═══════════════════════════════════════════════════════════════

1. QObject
   └─ 处理信号槽、对象通信、生命周期管理

2. QWidget
   └─ 所有可视化UI元素的基类
   └─ 提供几何管理、事件处理、样式

3. 常用子类分类：

   【输入类控件】
   • QLineEdit      - 单行文本输入
   • QTextEdit      - 多行文本输入
   • QSpinBox       - 数字输入框
   • QSlider        - 滑块
   • QDial          - 旋钮

   【显示类控件】
   • QLabel         - 文本/图片显示
   • QTextBrowser   - 富文本显示
   • QProgressBar   - 进度条
   • QLCDNumber     - LCD数字显示

   【选择类控件】
   • QCheckBox      - 复选框（多选）
   • QRadioButton   - 单选按钮（单选）
   • QComboBox      - 下拉列表
   • QListWidget    - 列表框
   • QTableWidget   - 表格
   • QTreeWidget    - 树形结构

   【按钮类控件】
   • QPushButton    - 普通按钮
   • QToolButton    - 工具按钮
   • QDialogButtonBox - 对话框按钮组

   【容器类控件】
   • QFrame         - 框架容器
   • QGroupBox      - 分组框
   • QTabWidget     - 标签页
   • QMainWindow    - 主窗口
   • QDialog        - 对话框


═══════════════════════════════════════════════════════════════
                        为什么学 QWidget？
═══════════════════════════════════════════════════════════════

✓ 逻辑直观      - 按钮就是按钮，窗口就是窗口
✓ 事件驱动      - 点击按钮 → 执行函数，思维清晰
✓ GUI设计工具   - 支持 Qt Designer 可视化设计
✓ 文档完整      - 网上教程最多，学习资源丰富
✓ 学习成本低    - 上手快，易于完成课程设计
✓ 适合项目      - 学生管理、图书管理、记事本等


═══════════════════════════════════════════════════════════════
                    【总结】一切都是 Widget
═══════════════════════════════════════════════════════════════
)";

    textDisplay->setText(hierarchy);
}
