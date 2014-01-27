#include "occupancy_grid.h"
#include "gridtraversal.h"
#include <math.h>
#include <set>
#include <QDebug>
#include <QPainter>
#include <QCoreApplication>

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
        if (temp.size() < 2) return;
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
        GridTraversal::gridLine(cameraCell, cell, pathToCurrent);
        bool foundCar = false;
        for (const auto& pathCell: pathToCurrent)
        {
            if (carCells.contains(pathCell))
            {
               cellsInFov.push_back(pathCell);
               foundCar = true;
               continue;
            }
            if (foundCar) break;
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
qreal OccupancyGrid::getCellProbability(const QPointF& cell) const
{
    QHash<int, QHash<int, qreal> >::const_iterator iterX =
            _grid.find((int) floor(cell.x() / CELL_WIDTH));
    QHash<int, qreal>::const_iterator iterY;
    if (iterX != _grid.end())
    {
        iterY = iterX.value().find((int) floor(cell.y() / CELL_WIDTH));
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


void OccupancyGrid::writeMapImage(int xMin, int yMin, int xMax, int yMax, QString imageName) const
{
    int width = xMax - xMin;
    int height = yMax - yMin;
    int cellSize = 10; //px
    QImage image(width * cellSize, height * cellSize, QImage::Format_RGB32);
    qDebug() << xMin << xMax << yMin << yMax;
    QRgb color;
    for (int x = 0; x < image.width(); x+=cellSize)
    {
        for (int y = 0; y < image.height(); y+=cellSize)
        {
            for (int xx = x; xx < x + cellSize; ++xx)
            {
                for (int yy = y; yy < y + cellSize; ++yy)
                {
                    QPointF point(xMin + (qreal)xx/cellSize, yMin + (qreal)yy/cellSize);
                    qreal prob = this->getCellProbability(point);
                    if (xx < image.width() && yy < image.height())
                    {
                        QColor colorRgb;
                        if (prob < 0)
                        {
                            colorRgb = QColor(50,50,70);
                        }
                        else
                        {
                            int val = (int) floor((1 - prob) * 255);
                            colorRgb = QColor(val,val,val);
                        }
                        color = colorRgb.rgb();
                        image.setPixel(xx, image.height() - yy, color);
                    }
                }
            }
        }
    }
    QString dirPath = QCoreApplication::applicationDirPath();
    dirPath = dirPath.left(dirPath.lastIndexOf('/')) + "/OSMViewer/grids/";
    qDebug() << "saving";
    qDebug() << image.size();
    qDebug() << image.save(dirPath + imageName + ".jpg");
    qDebug() << "saved"<<dirPath + imageName + ".jpg";
}
