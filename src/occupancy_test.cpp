
#include "occupancy_test.h"

void TestOccupancyGrid::updateOccupancy()
{
    OccupancyGrid grid(1);
    QPointF cameraPos(0.2, 0.2);
    QVector<QPointF> carPositions;
    carPositions.push_back(QPointF(1.2, 1.2));
    grid.updateOccupancy(cameraPos, carPositions, false);
    QCOMPARE(grid.getCellProbability(QPoint(0,0)), 0.1);
    QCOMPARE(grid.getCellProbability(QPoint(1,0)), 0.1);
    QCOMPARE(grid.getCellProbability(QPoint(0,1)), 0.1);
    QCOMPARE(grid.getCellProbability(QPoint(1,1)), 0.9);
    QCOMPARE(grid.getCellProbability(QPoint(1,2)), 0.5);
    QCOMPARE(grid.getCellProbability(QPoint(2,1)), 0.5);
    QCOMPARE(grid.getCellProbability(QPoint(2,2)), 0.5);
}
