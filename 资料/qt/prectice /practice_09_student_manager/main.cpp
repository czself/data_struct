#include <QApplication>
#include "studentmanager.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    StudentManager manager;
    manager.show();
    return app.exec();
}
