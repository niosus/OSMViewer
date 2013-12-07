
#include "test_occupancy.h"

void TestOccupancyGrid::updateOccupancy()
{
    OccupancyGrid grid(1);
    QPointF cameraPos(2.2, 2.2);
    QVector<QPointF> carPositions;
    carPositions.push_back(QPointF(6.2, 3.2));
    carPositions.push_back(QPointF(3.2, 5.2));
    QVector<QPointF> marginPoints;
    marginPoints.append(QPointF(2, 22));
    marginPoints.append(QPointF(22, 2));
    qreal eps = 0.001;
    grid.add(cameraPos, carPositions, marginPoints);
//    QCOMPARE(grid.getCellProbability(QPoint(0,0)), OccupancyGrid::freeProbability());
//    QCOMPARE(grid.getCellProbability(QPoint(1,0)), OccupancyGrid::freeProbability());
//    QCOMPARE(grid.getCellProbability(QPoint(0,1)), OccupancyGrid::freeProbability());
//    QCOMPARE(grid.getCellProbability(QPoint(1,3)), OccupancyGrid::NOT_TOUCHED);
//    QCOMPARE(grid.getCellProbability(QPoint(1,2)), OccupancyGrid::NOT_TOUCHED);
//    QCOMPARE(grid.getCellProbability(QPoint(2,1)), OccupancyGrid::NOT_TOUCHED);
//    QCOMPARE(grid.getCellProbability(QPoint(2,2)), OccupancyGrid::NOT_TOUCHED);
//    Q_ASSERT(grid.getCellProbability(QPoint(1,1)) < 0.987805 + eps
//             && grid.getCellProbability(QPoint(1,1)) > 0.987805 - eps);
    grid.writeMapImage(2, 2, 30, 30, "testmap");
}
