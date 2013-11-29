#include <QApplication>

#include "window.h"
#include "test_occupancy.h"
#include "test_bresenhem.h"

int main(int argc, char *argv[])
{
    TestOccupancyGrid test;
    TestGridTraversal test2;
    QTest::qExec(&test);
    QTest::qExec(&test2);
    QApplication app(argc, argv);
    Window window;
    window.show();
    return app.exec();
}
