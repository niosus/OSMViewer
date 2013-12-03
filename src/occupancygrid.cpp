#include "occupancygrid.h"
#include "gridtraversal.h"
#include <math.h>
#include <set>
#include <QDebug>

using namespace std;

qreal constexpr OccupancyGrid::PRIOR;
qreal constexpr OccupancyGrid::FREE_UPDATE_PROB;
qreal constexpr OccupancyGrid::OCCUPIED_UPDATE_PROB;
qreal constexpr OccupancyGrid::NOT_TOUCHED;

qreal toLogOdds(const qreal& prob)
{
    return log(prob/(1 - prob));
}

qreal fromLogOdds(const qreal& logOdds)
{
    return 1 - 1 / (1 + exp(logOdds));
}

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

void OccupancyGrid::add(
        const QPointF& cameraPosition,
        const QVector<QPointF>& carPositions,
        const QVector<QPointF>& marginPoints)
{
    int minX = INT_MAX;
    int maxX = INT_MIN;
    int minY = INT_MAX;
    int maxY = INT_MIN;
    QPoint cameraCell = findCellPos(cameraPosition);
    updateMinMaxVals(cameraCell, minX, maxX, minY, maxY);
    QVector<QPoint> carCells;
    for (const auto& carPos: carPositions)
    {
        QPoint cellPos = findCellPos(carPos);
        GridLine temp;
        GridTraversal::gridLine(cellPos, 2 * cellPos - cameraCell, temp);
        carCells.push_back(temp[0]);
        carCells.push_back(temp[1]);

        updateMinMaxVals(cellPos, minX, maxX, minY, maxY);
    }
    Q_ASSERT(marginPoints.size() > 1);
    QPoint leftMostCell = findCellPos(marginPoints[0]);
    QPoint rightMostCell = findCellPos(marginPoints[1]);
    GridLine frontier, pathToCurrent;
    GridTraversal::gridLine(leftMostCell, rightMostCell, frontier);
    QVector<QPoint> cellsInFov;
    for (const auto& cell: frontier)
    {
        GridTraversal::gridLine(cell, cameraCell, pathToCurrent);
        for (const auto& pathCell: pathToCurrent)
        {
            if (!cellsInFov.contains(pathCell))
                cellsInFov.push_back(pathCell);
        }
    }
    for (const auto& cell: cellsInFov)
    {
        if (!carCells.contains(cell))
        {
            updateCellProbability(cell, FREE);
        }
        else
        {
            updateCellProbability(cell, OCCUPIED);
        }
    }
}

// returns PRIOR if no such cell, or probability otherwise
qreal OccupancyGrid::getCellProbability(const QPointF& cell)
{
    QHash<int, QHash<int, qreal> >::const_iterator iterX =
            _grid.find(cell.x() / CELL_WIDTH);
    QHash<int, qreal>::const_iterator iterY;
    if (iterX != _grid.end())
    {
        iterY = iterX.value().find(cell.y() / CELL_WIDTH);
        if (iterY != iterX.value().end())
        {
            return fromLogOdds(iterY.value());
        }
    }
    return NOT_TOUCHED;
}

QPoint OccupancyGrid::findCellPos(const QPointF& position)
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
    if (_grid[x][y] == 0) _grid[x][y] = toLogOdds(PRIOR);
    _grid[x][y] = toLogOdds(prob) + _grid[x][y] - toLogOdds(PRIOR);
}

qreal OccupancyGrid::getCellWidth() const
{
    return CELL_WIDTH;
}

