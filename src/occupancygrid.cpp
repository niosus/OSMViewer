#include "occupancygrid.h"
#include <math.h>
#include <QDebug>
using namespace std;

void OccupancyGrid::updateMinMaxVals(const QPoint& cell,
                      int& minX, int& maxX,
                      int& minY, int& maxY)
{
    minX = min((int) cell.x(), minX);
    maxX = max((int) cell.x(), maxX);
    minY = min((int) cell.y(), minY);
    maxY = max((int) cell.y(), maxY);

    // update global bounds
    _minX = min(minX, _minX);
    _maxX = max(maxX, _maxX);
    _minY = min(minY, _minY);
    _maxY = max(maxY, _maxY);
}

void OccupancyGrid::getBounds(int &minX, int &maxX,
                      int &minY, int &maxY) const
{
    minX = _minX * CELL_WIDTH;
    maxX = _maxX * CELL_WIDTH;
    minY = _minY * CELL_WIDTH;
    maxY = _maxY * CELL_WIDTH;
}


void OccupancyGrid::updateOccupancy(
        const QPointF& cameraPosition,
        const QVector<QPointF>& carPositions)
{
    qDebug()<<"update Occupancy"<<cameraPosition.x()<<cameraPosition.y();
    int minX = INT_MAX;
    int maxX = INT_MIN;
    int minY = INT_MAX;
    int maxY = INT_MIN;
    QPoint cameraCell = findCell(cameraPosition);
    qDebug()<<"here"<<cameraCell;
    updateMinMaxVals(cameraCell, minX, maxX, minY, maxY);
    QVector<QPoint> carCells;
    for (const auto& carPos: carPositions)
    {
        QPoint cell = findCell(carPos);
        carCells.push_back(cell);
        updateMinMaxVals(cell, minX, maxX, minY, maxY);
    }
    qDebug()<<"bounds"<< minX << maxX << minY << maxY;
    // update all values in rect from min to max
    for (int x = minX; x <= maxX; ++x)
    {
        for (int y = minY; y <= maxY; ++y)
        {
            qDebug()<<x<<y;
            QPoint temp = QPoint(x,y);
            if (!carCells.contains(temp))
            {
                qDebug()<<"updating FREE";
                updateCellProbability(temp, FREE);
            }
            else
            {
                qDebug()<<"updating OCCUPIED";
                updateCellProbability(temp, OCCUPIED);
            }
        }
    }
    qDebug()<<"done";
}

// returns -1 if no such cell, or probability otherwise
qreal OccupancyGrid::getCellProbability(const QPoint& cell)
{
    QHash<int, QHash<int, qreal> >::const_iterator iterX = _grid.find(cell.x() / CELL_WIDTH);
    QHash<int, qreal>::const_iterator iterY;
    if (iterX != _grid.end())
    {
        iterY = iterX.value().find(cell.y() / CELL_WIDTH);
        if (iterY != iterX.value().end())
        {
            return iterY.value();
        }
    }
    return -1;
}

QPoint OccupancyGrid::findCell(const QPointF& position)
{
    int x = floor(position.x() / CELL_WIDTH);
    int y = floor(position.y() / CELL_WIDTH);
    return QPoint(x, y);
}

void OccupancyGrid::updateCellProbability(
        const QPoint& cell,
        const CellState& state)
{
    updateCellProbability(cell.x(), cell.y(), state);
}

void OccupancyGrid::updateCellProbability(
        const int& x,
        const int& y,
        const CellState& state)
{
    qreal prob;
    switch (state)
    {
    case FREE:
        prob = FREE_UPDATE_PROB;
        break;
    case OCCUPIED:
        prob = OCCUPIED_UPDATE_PROB;
        break;
    }
    qreal val = prob + _grid[x][y] - PRIOR < 0;
    val = (val<0)?0:val;
    _grid[x][y] = val;
}

