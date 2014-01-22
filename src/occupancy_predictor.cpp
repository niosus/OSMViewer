#include "occupancy_predictor.h"
#include <QImage>
#include <QColor>
#include <QCoreApplication>

void OccupancyPredictor::predictFromGrids(const QHash<QString, OccupancyGrid> &grids)
{
    for (const QString& date: grids.keys())
    {
        updatePrediction(grids[date]);
    }
}

void OccupancyPredictor::updatePrediction(const OccupancyGrid &grid)
{
    int minX, maxX, minY, maxY;
    grid.getBounds(minX, maxX, minY, maxY);
    for (int x = minX; x < maxX; ++x)
    {
        for (int y = minY; y < maxY; ++y)
        {
            if (grid.getCellProbability(QPointF(x, y)) > 0.8)
            {
                _prediction[x][y].occupied++;
            }
            else
            {
                _prediction[x][y].free++;
            }
        }
    }
}

PredictedOccupancyGrid OccupancyPredictor::getPrediction()
{
    return _prediction;
}

void OccupancyPredictor::writeImage(
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
    for (int x = 0; x < image.width(); x+=cellSize)
    {
        for (int y = 0; y < image.height(); y+=cellSize)
        {
            for (int xx = x; xx < x + cellSize; ++xx)
            {
                for (int yy = y; yy < y + cellSize; ++yy)
                {
                    if (xx < image.width() && yy < image.height())
                    {
                        OccupancyCell cell = _prediction[xMin + x/cellSize][yMin + y/cellSize];
                        if (cell.free < 1 || cell.occupied < 1)
                        {
                            image.setPixel(xx, image.height() - yy, colorGray.rgb());
                        }
                        else if (cell.free > cell.occupied)
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
        }
    }
    QString dirPath = QCoreApplication::applicationDirPath();
    dirPath = dirPath.left(dirPath.lastIndexOf('/')) + "/OSMViewer/grids/";
    qDebug() << "saving";
    qDebug() << image.size();
    qDebug() << image.save(dirPath + imageName + ".jpg");
    qDebug() << "saved"<<dirPath + imageName + ".jpg";
}
