#ifndef DATAGENERATOR_H
#define DATAGENERATOR_H

#include <QObject>
#include <QPointF>
#include <QVector>
#include <QHash>
#include <QXmlStreamReader>
#include <QPolygonF>
#include "kmlwriter.h"
#include "occupancygrid.h"
#include "point_with_rot.h"

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
    QHash<QString, OccupancyGrid> _grids;

    void getNodesAndWaysFromXml();
    void getCarsFromLogFiles();
    void getCarsGpsPosFromLogFiles(const QString &date,
            const QString &imagesGpsFileName,
            const QString &detectedCarsFileName);
    void getCarPositionsFromAllData(
            const QVector<QString> &allImageNames,
            const QMap<QString, QVector<QVector3D> > &carPosHash,
            const QMap<QString, MyPointF> &imageGpsHash);
    void getCarPositionsFromAllDataLaser(
            const QString &date,
            const QVector<QString> &allImageNames,
            const QMap<QString, QVector<QPointF> > &carPosHash,
            const QMap<QString, MyPointF> &imageGpsHash);
    QPointF getPrevGpsPoint(const QString &name,
            const QMap<QString, MyPointF> &imagePositionHash);
    void storeNewNode(QXmlStreamReader *xmlReader);
    void updateBounds(QXmlStreamReader *xmlReader);
    void storeNewWay(QXmlStreamReader *xmlReader);
    void updateOccupancy(const QString &date, const QPointF& thisPointInMeters,
            const float &angleOfThisGpsPointSystem,
            QVector<QVector3D> &carPositions,
            KmlWriter *kmlWriter, const QString &name);
    void updateOccupancyLaser(
            const QString &date,
            const QPointF& thisPointInMeters,
            const float &angleOfThisGpsPointSystem,
            QVector<QPointF>& carPositions,
            KmlWriter* kmlWriter,
            const QString &name);

signals:
    void boundariesUpdated(QHash<QString, double> &bounds);
    void dataGenerated(
            QVector<QPolygonF> &roads,
            QVector<QPolygonF> &houses,
            QVector<QPolygonF> &parkings,
            QVector<QPolygonF> &other);
    void carsGenerated(QVector<QPointF> &cars);
    void pathGenerated(QPolygonF &path);
    void gridGenerated(OccupancyGrid& grid);

};

#endif // DATAGENERATOR_H
