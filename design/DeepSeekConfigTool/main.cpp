#include <QApplication>
#include <QPalette>
#include <QStyleFactory>
#include "mainwindow.h"

static void applyDarkTheme(QApplication &app)
{
    app.setStyle(QStyleFactory::create("Fusion"));

    QPalette p;
    p.setColor(QPalette::Window,          QColor(30,  30,  46));
    p.setColor(QPalette::WindowText,      QColor(205, 214, 244));
    p.setColor(QPalette::Base,            QColor(24,  24,  37));
    p.setColor(QPalette::AlternateBase,   QColor(30,  30,  46));
    p.setColor(QPalette::ToolTipBase,     QColor(69,  71,  90));
    p.setColor(QPalette::ToolTipText,     QColor(205, 214, 244));
    p.setColor(QPalette::Text,            QColor(205, 214, 244));
    p.setColor(QPalette::Button,          QColor(49,  50,  68));
    p.setColor(QPalette::ButtonText,      QColor(205, 214, 244));
    p.setColor(QPalette::BrightText,      QColor(249, 226, 175));
    p.setColor(QPalette::Link,            QColor(137, 180, 250));
    p.setColor(QPalette::Highlight,       QColor(137, 180, 250));
    p.setColor(QPalette::HighlightedText, QColor(30,  30,  46));
    p.setColor(QPalette::PlaceholderText, QColor(108, 112, 134));

    // Disabled colors
    p.setColor(QPalette::Disabled, QPalette::WindowText, QColor(108, 112, 134));
    p.setColor(QPalette::Disabled, QPalette::Text,       QColor(108, 112, 134));
    p.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(108, 112, 134));

    app.setPalette(p);

    app.setStyleSheet(R"(
        QToolTip {
            background-color: #45455a;
            border: 1px solid #585b70;
            padding: 4px;
            font-size: 12px;
        }
    )");
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QApplication::setApplicationName("DeepSeek 实验配置工具");

    applyDarkTheme(app);

    MainWindow w;
    w.show();

    return app.exec();
}
