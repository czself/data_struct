#include <QApplication>
#include "hierarchywidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    HierarchyWidget widget;
    widget.show();

    return app.exec();
}
