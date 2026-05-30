#include <QApplication>
#include "widgetshowcase.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    WidgetShowcase showcase;
    showcase.show();
    return app.exec();
}
