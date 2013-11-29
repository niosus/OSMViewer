#include "test_bresenhem.h"
#include <QDebug>

void TestGridTraversal::gridLine()
{
    GridTraversal bresenhem;
    QPoint start(0, 0);
    QPoint end(4, 2);
    QVector<QPoint> line;
    bresenhem.gridLine(start, end, line);
    QCOMPARE(line[0], QPoint(0, 0));
    QCOMPARE(line[1], QPoint(1, 1));
    QCOMPARE(line[2], QPoint(2, 1));
    QCOMPARE(line[3], QPoint(3, 2));
    QCOMPARE(line[4], QPoint(4, 2));
}
