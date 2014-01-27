#ifndef PARKING_LOTS_H
#define PARKING_LOTS_H

#include <QVector>
#include <QPointF>
#include "occupancy_cell.h"

class ParkingLots
{
public:
    ParkingLots();
    ParkingLots(const qreal &leftCorner, const qreal &bottomCorner);
    void update(const QVector<QPointF>& cars);
    void writeImage(
            int xMin,
            int yMin,
            int xMax,
            int yMax,
            QString imageName) const;

    void updateLeftFree();
private:
    void updateClosest(const QPointF& detection);

    QVector<QPointF> _centers;
    QVector<OccupancyCell> _occupancy;
    QVector<int> _tempUpdatedIndeces;

    static const int _numOfRows = 15;
    static const int _numOfCols = 12;

    static const int _width = 50; //px
    static const int _height = 25; //px
    static const int _gap = 60; //px
    static constexpr float _metersInPx = 0.1F; // meters in pixels
};

#endif // PARKING_LOTS_H
