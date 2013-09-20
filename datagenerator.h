#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <vector>
#include <QPoint>

class DataGenerator: public QObject
{
    Q_OBJECT
public:
    DataGenerator();
    void generateData();

signals:
    void dataGenerated(std::vector<QPoint> coords);
};

#endif // DATAGENERATOR_H
