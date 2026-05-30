#include <QApplication>
#include "bestpractices.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    BestPractices demo;
    demo.show();

    return app.exec();
}
