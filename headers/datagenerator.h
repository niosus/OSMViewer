#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QHash>
#include <QXmlStreamReader>
#include "mypolygonf.h"

class DataGenerator: public QObject
{
    Q_OBJECT

public:
    DataGenerator();
    void generateData();

private:
    enum NODE_ATTRS {LONGITUDE, LATITUDE, ID};

    QHash<long, QPointF> _nodes;
    QVector<MyPolygonF> _roads;
    QVector<MyPolygonF> _houses;

    void getNodesAndWaysFromXml();
    void storeNewNode(QXmlStreamReader *xmlReader);
    void updateBounds(QXmlStreamReader *xmlReader);
    void storeNewWay(QXmlStreamReader *xmlReader);

signals:
    void boundariesUpdated(QVector<double> &bounds);
    void dataGenerated(QVector<MyPolygonF> &roads, QVector<MyPolygonF> &houses);

};

#endif // DATAGENERATOR_H
