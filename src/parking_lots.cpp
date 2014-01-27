#include "parking_lots.h"
#include <QDebug>
#include <QImage>
#include <QColor>
#include <QCoreApplication>

ParkingLots::ParkingLots()
{
}

ParkingLots::ParkingLots(const qreal &leftCorner, const qreal &bottomCorner)
{
    QPointF tempPoint;
    QPointF leftBottomCorner(leftCorner, bottomCorner);
    OccupancyCell emptyCell;
    QPointF leftBottomUtm(871831, 6077369);
    qDebug() << "num of rows"<<_numOfRows;
    qDebug() << "num of cols"<<_numOfCols;
    for (int colNum = 0; colNum < _numOfCols; ++colNum)
    {
        for (int rowNum = 0; rowNum < _numOfRows; ++rowNum)
        {
            tempPoint.ry() =
                    leftBottomCorner.y() * _metersInPx + leftBottomUtm.y() +
                    rowNum * _height * _metersInPx +
                    (_height / 2) * _metersInPx;
            tempPoint.rx() =
                    leftBottomCorner.x() * _metersInPx + leftBottomUtm.x() +
                    colNum * _width * _metersInPx +
                    (_width / 2) * _metersInPx;
            // move if there is a gap between
            // the rows are as follows:
            // 01___23___45___67___ ...
            tempPoint.rx() = tempPoint.x() + (colNum / 2) * (_gap) * _metersInPx;
            _centers.append(tempPoint);
            _occupancy.append(emptyCell);
        }
    }
}

void ParkingLots::updateClosest(const QPointF &detection)
{
    qDebug() <<qSetRealNumberPrecision(15)<< "searching close to "<<detection;
    qreal minDist = 10E+20; //should be big enough
    int minIndex = -1;
    qDebug()<< "centers: "<<_centers.size();
    for (int i = 0; i < _centers.size(); ++i)
    {
        qreal currentDist = sqrt(QPointF::dotProduct(_centers[i] - detection, _centers[i] - detection));
        if (minDist > currentDist)
        {
            minDist = currentDist;
            minIndex = i;
            qDebug()<<qSetRealNumberPrecision(15)<<"dist to "<<_centers[i]<<" is"<<currentDist;
        }
    }
    if (minIndex < 0)
    {
        qDebug()<<"parking lots update failed for" << detection;
        return;
    }
    _occupancy[minIndex].occupied++;
    _tempUpdatedIndeces.append(minIndex);
}

void ParkingLots::updateLeftFree()
{
    for (int i = 0; i < _occupancy.size(); ++i)
    {
        if (!_tempUpdatedIndeces.contains(i))
        {
            _occupancy[i].free++;
        }
    }
    _tempUpdatedIndeces.clear();
}

void ParkingLots::update(const QVector<QPointF> &cars)
{
    for (const QPointF& car: cars)
    {
        this->updateClosest(car);
    }
//    this->updateLeftFree();
}

void ParkingLots::writeImage(
        int xMin,
        int yMin,
        int xMax,
        int yMax,
        QString imageName) const
{
    int width = xMax - xMin;
    int height = yMax - yMin;
    int cellSize = 10; //px
    QImage image(width * cellSize, height * cellSize, QImage::Format_RGB32);
    qDebug() << xMin << xMax << yMin << yMax;
    QColor colorGreen = QColor(0, 100, 0);
    QColor colorRed = QColor(100, 0, 0);
    QColor colorGray = QColor(50, 50, 50);

    for (int i = 0; i < _occupancy.size(); ++i)
    {
        int currentCenterX = floor((_centers[i].x() - xMin) * cellSize);
        int currentCenterY = floor((_centers[i].y() - yMin) * cellSize);
        for (int xx = currentCenterX - cellSize / 2; xx < currentCenterX + cellSize / 2; ++xx)
        {
           for (int yy = currentCenterY - cellSize / 2; yy < currentCenterY + cellSize / 2; ++yy)
           {
               OccupancyCell cell = _occupancy[i];
               qDebug()<<"free"<<cell.free<<"occ"<<cell.occupied;
               /*if (cell.free < 1 || cell.occupied < 1)
               {
                   image.setPixel(xx, image.height() - yy, colorGray.rgb());
               }
               else*/ if (cell.free > cell.occupied)
               {
                   image.setPixel(xx, image.height() - yy, colorGreen.rgb());
               }
               else if (cell.occupied > cell.free)
               {
                   image.setPixel(xx, image.height() - yy, colorRed.rgb());
               }
               else
               {
                   image.setPixel(xx, image.height() - yy, colorGray.rgb());
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
