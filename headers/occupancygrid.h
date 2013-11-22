#ifndef OCCUPANCYGRID_H
#define OCCUPANCYGRID_H

#include <QHash>
#include <QPointF>
#include <QVector>
#include <QDebug>

typedef QHash<int, QHash<int, qreal> > Grid;

class OccupancyGrid
{
public:
    enum CellState {OCCUPIED, FREE};
    OccupancyGrid():
        _maxX(INT_MIN),
        _maxY(INT_MIN),
        _minX(INT_MAX),
        _minY(INT_MAX){}
    OccupancyGrid(const OccupancyGrid& other)
    {
        _grid = other._grid;
        _maxX = other._maxX;
        _maxY = other._maxY;
        _minX = other._minX;
        _minY = other._minY;
    }

    void updateOccupancy(
        const QPointF& cameraPosition,
        const QVector<QPointF>& carPositions);
    qreal getCellProbability(const QPoint& cell);
    void getBounds(
        int& minX, int& maxX,
        int& minY, int& maxY) const;


private:
    Grid _grid;

    int _maxX;
    int _maxY;
    int _minX;
    int _minY;

    static constexpr qreal CELL_WIDTH = 1;
    static constexpr qreal PRIOR = 0.5;
    static constexpr qreal OCCUPIED_UPDATE_PROB = 0.55;
    static constexpr qreal FREE_UPDATE_PROB = 0.45;

    QPoint findCell(const QPointF& position);
    void updateCellProbability(
            const QPoint& cell,
            const CellState& state);
    void updateCellProbability(const int &x,
            const int &y,
            const CellState& state);
    void updateMinMaxVals(const QPoint& cell,
          int &minX, int &maxX,
          int &minY, int &maxY);
};

#endif // OCCUPANCYGRID_H
