#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QPointF>
#include <QPolygonF>
#include <QVector>
#include <QHash>
#include <QXmlStreamReader>

class DataGenerator: public QObject
{
    Q_OBJECT

public:
    DataGenerator();
    void generateData();

private:
    QHash<long, QPointF> _nodes;
    QVector<QPolygonF> _ways;
    QVector<double> _bounds;

    void getNodesAndWaysFromXml();
    void storeNewNode(QXmlStreamReader *xmlReader);
    void updateBounds(QXmlStreamReader *xmlReader);
    void storeNewWay(QXmlStreamReader *xmlReader);

signals:
    void boundariesUpdated(QVector<double> &bounds);
    void dataGenerated(QVector<QPolygonF> &ways);

};

#endif // DATAGENERATOR_H
