
#include "test_occupancy.h"

void TestOccupancyGrid::updateOccupancy()
{
    OccupancyGrid grid(1);
    QPointF cameraPos(0.2, 0.2);
    QVector<QPointF> carPositions;
    carPositions.push_back(QPointF(1.2, 1.2));
    QVector<QPointF> marginPoints;
    marginPoints.append(QPointF(0, 2));
    marginPoints.append(QPointF(2, 0));
    qreal delta = 0.0001;
    grid.add(cameraPos, carPositions, marginPoints);
    QCOMPARE(grid.getCellProbability(QPoint(0,0)), OccupancyGrid::freeProbability());
    QCOMPARE(grid.getCellProbability(QPoint(1,0)), OccupancyGrid::freeProbability());
    QCOMPARE(grid.getCellProbability(QPoint(0,1)), OccupancyGrid::freeProbability());
    QCOMPARE(grid.getCellProbability(QPoint(1,3)), OccupancyGrid::NOT_TOUCHED);
    QCOMPARE(grid.getCellProbability(QPoint(1,2)), OccupancyGrid::NOT_TOUCHED);
    QCOMPARE(grid.getCellProbability(QPoint(2,1)), OccupancyGrid::NOT_TOUCHED);
    QCOMPARE(grid.getCellProbability(QPoint(2,2)), OccupancyGrid::NOT_TOUCHED);
    QCOMPARE(grid.getCellProbability(QPoint(1,1)), OccupancyGrid::occupiedProbability());
}
