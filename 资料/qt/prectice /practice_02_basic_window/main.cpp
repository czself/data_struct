#include <QApplication>
#include <QWidget>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);   // 1. 创建 Qt 应用对象

    QWidget window;                 // 2. 创建窗口
    window.resize(400, 300);        // 3. 设置窗口大小（宽400，高300）
    window.setWindowTitle("我的第一个Qt窗口"); // 设置标题
    window.show();                  // 4. 显示窗口

    return app.exec();              // 5. 进入事件循环
}