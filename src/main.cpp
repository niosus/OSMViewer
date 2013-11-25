#include <QApplication>

#include "window.h"
#include "occupancy_test.h"

int main(int argc, char *argv[])
{
    TestOccupancyGrid test;
    QTest::qExec(&test);
    QApplication app(argc, argv);
    Window window;
    window.show();
    return app.exec();
}
