#include "datagenerator.h"

DataGenerator::DataGenerator(){}

void DataGenerator::generateData()
{
    std::vector<QPoint> points;
    QPoint point(0,0);

    point.setX(10);
    point.setY(10);
    points.push_back(point);

    point.setX(10);
    point.setY(60);
    points.push_back(point);

    point.setX(60);
    point.setY(60);
    points.push_back(point);

    point.setX(60);
    point.setY(10);
    points.push_back(point);

    emit dataGenerated(points);
}
