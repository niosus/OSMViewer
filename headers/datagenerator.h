#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QHash>
#include <QXmlStreamReader>
#include <QPolygonF>

class DataGenerator: public QObject
{
    Q_OBJECT

public:
    DataGenerator();
    void generateData();

private:
    enum NODE_ATTRS {LONGITUDE, LATITUDE, ID};

    QHash<long, QPointF> _nodes;
    QVector<QPolygonF> _roads;
    QVector<QPolygonF> _houses;
    QVector<QPolygonF> _parkings;
    QVector<QPolygonF> _other;
    QVector<QPointF> _cars;
    QPolygonF _path;

    void getNodesAndWaysFromXml();
    void getCarsFromLogFiles();
    void getCarPositionsFromAllData(const QMap<QString, QVector<QVector3D> > &carPosHash,
            const QMap<QString, QPointF> &imageGpsHash);
    QPointF getPrevGpsPoint(
            const QString &name,
            const QMap<QString, QPointF> &imageGpsHash);
    void storeNewNode(QXmlStreamReader *xmlReader);
    void updateBounds(QXmlStreamReader *xmlReader);
    void storeNewWay(QXmlStreamReader *xmlReader);

signals:
    void boundariesUpdated(QHash<QString, double> &bounds);
    void dataGenerated(
            QVector<QPolygonF> &roads,
            QVector<QPolygonF> &houses,
            QVector<QPolygonF> &parkings,
            QVector<QPolygonF> &other);
    void carsGenerated(QVector<QPointF> &cars);
    void pathGenerated(QPolygonF &path);

};

#endif // DATAGENERATOR_H
