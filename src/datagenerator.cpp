#include "datagenerator.h"
#include "kmlwriter.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QVector3D>
#include "point_with_rot.h"
#include <cmath>
#include "logReader.h"

#include "mercator.h"


DataGenerator::DataGenerator(){
}

void DataGenerator::generateData()
{
    LogReader logReader(":/log/cars/");
    for (const QString& date: logReader.getAvailableDates())
    {
       QHash<LogReader::LogType, QString> tempHash = logReader.getLogNamesForDate(date);
       qDebug() << "date" << date;
       if (!tempHash.isEmpty())
           getCarsGpsPosFromLogFiles(
                       date,
                       tempHash[LogReader::IMAGES_POS],
                       tempHash[LogReader::CARS_POS]);
    }
    getNodesAndWaysFromXml();
    getCarsFromLogFiles();
    emit dataGenerated(_roads, _houses, _parkings, _other);
    emit carsGenerated(_cars);
    emit pathGenerated(_path);
    emit gridGenerated(_grids[logReader.getAvailableDates()[0]]);
    writeTestKml();
}

void DataGenerator::getCarsFromLogFiles()
{
    QFile * logImagesGps = new QFile(":/log/log_5_12_2013/gps_for_each_image_log.txt");
    QFile * logImagesRects = new QFile(":/log/log_5_12_2013/car_rects_log.dat");
    if (!logImagesGps->open(QIODevice::ReadOnly)
            || !logImagesRects->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(new QWidget,"No Log File",
        "log file does not exist",
        QMessageBox::Ok);
        return;
    }
    QMap<QString, MyPointF> imagePositionHash;
    QMap<QString, QVector<QVector3D> > carPosHash;

    // reading in all images
    QTextStream inPositions(logImagesGps);
    while(!inPositions.atEnd()) {
        QString line = inPositions.readLine();
        QStringList fields = line.split("\t\t");
        QString name = fields[1].split("=")[1];
        float x = fields[2].split("=")[1].toFloat();
        float y = fields[3].split("=")[1].toFloat();
        float theta = fields[4].split("=")[1].toFloat();
        imagePositionHash[name]=MyPointF(x, y, theta);
//        _path.append(QPointF(merc_x(lon), merc_y(lat)));
    }

    // read in all detected cars' rects
    QTextStream inRects(logImagesRects);
    QVector<QString> allUsedImages;
    while(!inRects.atEnd()) {
        QString line = inRects.readLine();
        if (line.contains("TOTAL_NUM_IMAGES"))
        {
            // no error check.
            // This code is awful. Hope noone ever sees this...
            int num = line.split("\t", QString::SkipEmptyParts)[1].toInt();
            qDebug() << num;
            for (int i = 0; i < num; ++i)
            {
                line = inRects.readLine();
                allUsedImages.push_back(line);
            }
            continue;
        }
        QStringList fields = line.split("\t");
        QString name = fields[0].split(":")[1];
        float x = fields[1].split(":")[1].toFloat();
        float y = fields[2].split(":")[1].toFloat();
        float z = fields[3].split(":")[1].toFloat();
        carPosHash[name].append(QVector3D(x, y, z));
    }
    getCarPositionsFromAllData(allUsedImages, carPosHash, imagePositionHash);

    logImagesGps->close();
    logImagesRects->close();
    delete logImagesGps;
    delete logImagesRects;
}

void DataGenerator::getCarsGpsPosFromLogFiles(
        const QString& date,
        const QString& imagesGpsFileName,
        const QString& detectedCarsFileName)
{
    QFile * logImagesGps = new QFile(imagesGpsFileName);
    QFile * logImagesRects = new QFile(detectedCarsFileName);
    if (!logImagesGps->open(QIODevice::ReadOnly)
            || !logImagesRects->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(new QWidget,"No Log File",
        "log file does not exist",
        QMessageBox::Ok);
        return;
    }
    QMap<QString, MyPointF> imagePositionHash;
    QMap<QString, QVector<QPointF> > carPosHash;

    // reading in all images
    QTextStream inPositions(logImagesGps);
    while(!inPositions.atEnd()) {
        QString line = inPositions.readLine();
        QStringList fields = line.split("\t");
        qDebug()<<fields.size();
        Q_ASSERT(fields.size() == 5);
        QString name = fields[1].split("=")[1];
        float x = fields[2].split("=")[1].toFloat();
        float y = fields[3].split("=")[1].toFloat();
        float theta = fields[4].split("=")[1].toFloat();
        imagePositionHash[name]=MyPointF(x, y, theta);
//        _path.append(QPointF(merc_x(lon), merc_y(lat)));
    }

    // read in all detected cars' rects
    QTextStream inRects(logImagesRects);
    QVector<QString> allUsedImages;
    while(!inRects.atEnd()) {
        QString line = inRects.readLine();
        if (line.contains("TOTAL_NUM_IMAGES"))
        {
            // no error check.
            // This code is awful. Hope noone ever sees this...
            int num = line.split("\t", QString::SkipEmptyParts)[1].toInt();
            for (int i = 0; i < num; ++i)
            {
                line = inRects.readLine();
                allUsedImages.push_back(line);
            }
            continue;
        }
        QStringList fields = line.split("\t");
        QString name = fields[0].split(":")[1];
        float x = fields[2].toFloat();
        float y = fields[4].toFloat();
        carPosHash[name].append(QPointF(x, y));
    }
    getCarPositionsFromAllDataLaser(date, allUsedImages, carPosHash, imagePositionHash);

    logImagesGps->close();
    logImagesRects->close();
    delete logImagesGps;
    delete logImagesRects;
}

void DataGenerator::updateOccupancy(
        const QString& date,
        const QPointF& thisPointInMeters,
        const float &angleOfThisGpsPointSystem,
        QVector<QVector3D>& carPositions,
        KmlWriter* kmlWriter,
        const QString &name)
{
    QVector<QPointF> carCorrectPositions;
    qreal maxDist = 30;
    qreal fieldOfView = M_PI * 97. / 180.; // in radians
    QPointF leftMostPoint(maxDist, maxDist * tan(-fieldOfView / 2));
    QPointF rightMostPoint(maxDist, maxDist * tan(fieldOfView / 2));
    QVector<QPointF> marginPoints;
    QTransform transform;
    transform.translate(thisPointInMeters.x(), thisPointInMeters.y());
    transform.rotate(angleOfThisGpsPointSystem);
    marginPoints.push_back(transform.map(leftMostPoint));
    marginPoints.push_back(transform.map(rightMostPoint));
    for (auto carPos: carPositions)
    {
        QPointF carInCameraViewPosition(carPos.z(), carPos.x());
        if (sqrt(QPointF::dotProduct(carInCameraViewPosition, carInCameraViewPosition)) > 20)
        {
            continue;
        }
        QPointF carGlobalPos = transform.map(carInCameraViewPosition);
        carCorrectPositions.push_back(carGlobalPos);
        _cars.push_back(carGlobalPos);
        kmlWriter->addPoint(0, name,
                            QPointF(merc_lon(carGlobalPos.x()), merc_lat(carGlobalPos.y())));
    }
    _path.append(thisPointInMeters);
    _grids[date].add(thisPointInMeters,
              carCorrectPositions,
              marginPoints);
}

void DataGenerator::updateOccupancyLaser(
        const QString& date,
        const QPointF& thisPointInMeters,
        const float &angleOfThisGpsPointSystem,
        QVector<QPointF>& carPositions,
        KmlWriter* kmlWriter,
        const QString &name)
{
    QVector<QPointF> carCorrectPositions;
    qreal maxDist = 30;
    qreal fieldOfView = M_PI * 97. / 180.; // in radians
    QPointF leftMostPoint(maxDist, maxDist * tan(-fieldOfView / 2));
    QPointF rightMostPoint(maxDist, maxDist * tan(fieldOfView / 2));
    QVector<QPointF> marginPoints;
    QTransform transform;
    transform.translate(thisPointInMeters.x(), thisPointInMeters.y());
    transform.rotate(angleOfThisGpsPointSystem);
    marginPoints.push_back(transform.map(leftMostPoint));
    marginPoints.push_back(transform.map(rightMostPoint));
    for (const auto& carPos: carPositions)
    {
        carCorrectPositions.push_back(carPos);
        _cars.push_back(carPos);
        kmlWriter->addPoint(0, name,
                            QPointF(merc_lon(carPos.x()), merc_lat(carPos.y())));
    }
    _path.append(thisPointInMeters);
    _grids[date].add(thisPointInMeters,
              carCorrectPositions,
              marginPoints);
}

void DataGenerator::getCarPositionsFromAllData(
        const QVector<QString> &allImageNames,
        const QMap<QString, QVector<QVector3D> > &carPosHash,
        const QMap<QString, MyPointF> &imageGpsHash)
{
    _cars.clear();
    int counter = 0;
    KmlWriter *kmlWriter = new KmlWriter();
    for (const auto& name: allImageNames)
    {
        MyPointF thisPoint = imageGpsHash.value(name);
        float angleOfThisGpsPointSystem = thisPoint.theta() * 180 / M_PI;
        QVector<QVector3D> carPositions = carPosHash.value(name);
        updateOccupancy(
                    "hack",
                    QPointF(thisPoint.x(), thisPoint.y()),
                    angleOfThisGpsPointSystem,
                    carPositions, kmlWriter, name);
        QString name;
        name.setNum(++counter);
        name  = "mymap" + name;
    }
    delete kmlWriter;
}

void DataGenerator::getCarPositionsFromAllDataLaser(
        const QString& date,
        const QVector<QString> &allImageNames,
        const QMap<QString, QVector<QPointF> > &carPosHash,
        const QMap<QString, MyPointF> &imageGpsHash)
{
    _cars.clear();
    int counter = 0;
    KmlWriter *kmlWriter = new KmlWriter();
    for (const auto& name: allImageNames)
    {
        MyPointF thisPoint = imageGpsHash.value(name);
        float angleOfThisGpsPointSystem = thisPoint.theta() * 180 / M_PI;
        QVector<QPointF> carPositions = carPosHash.value(name);
        qDebug()<<"here?";
        updateOccupancyLaser(
                    date,
                    QPointF(thisPoint.x(), thisPoint.y()),
                    angleOfThisGpsPointSystem,
                    carPositions,
                    kmlWriter,
                    name);
        qDebug()<<"here!";
        QString name;
        name.setNum(++counter);
        name  = "mymap" + name;
        qDebug() << name;
    }
    delete kmlWriter;
}

void DataGenerator::storeNewNode(QXmlStreamReader *xmlReader)
{
    QXmlStreamAttributes attributes = xmlReader->attributes();
    long id = 0;
    double lat = 0;
    double lon = 0;
    for (QXmlStreamAttribute attr : attributes)
    {
        if (attr.name()=="id")
        {
            id = attr.value().toLong();
        }
        else if (attr.name()=="lat")
        {
            lat = attr.value().toDouble();
        }
        else if (attr.name()=="lon")
        {
            lon = attr.value().toDouble();
        }
    }
    if (id && lat && lon)
    {
        // QPointF point(lat, lon);
        QPointF point(merc_x(lon), merc_y(lat));
        _nodes.insert(id, point);
    }
}

void DataGenerator::updateBounds(QXmlStreamReader *xmlReader)
{
    QXmlStreamAttributes attributes = xmlReader->attributes();
    double minLat = 0, maxLat = 0;
    double minLon = 0, maxLon = 0;
    for (QXmlStreamAttribute attr : attributes)
    {
        if (attr.name()=="minlat")
        {
            minLat = attr.value().toDouble();
        }
        else if (attr.name()=="minlon")
        {
            minLon = attr.value().toDouble();
        }
        else if (attr.name()=="maxlat")
        {
            maxLat = attr.value().toDouble();
        }
        else if (attr.name()=="maxlon")
        {
            maxLon = attr.value().toDouble();
        }
    }


    if (minLat && minLon && maxLat && maxLon) {
        QHash<QString, double> bounds;

        bounds.clear();
        bounds["xMin"] = merc_x(minLon);
        bounds["xMax"] = merc_x(maxLon);
        bounds["yMin"] = merc_y(minLat);
        bounds["yMax"] = merc_y(maxLat);

        emit boundariesUpdated(bounds);
    } else {
        qDebug() << "Could not read bounding box from osm file";
    }
}

void DataGenerator::storeNewWay(QXmlStreamReader *xmlReader)
{
    //We are going to fill this Polygon now
    QPolygonF polygon;
    enum WayType {BUILDING, ROAD, PARKING, NONE};
    WayType wayType = WayType::NONE;
    //loop through everything that a way contains
    while(!(xmlReader->tokenType() == QXmlStreamReader::EndElement
            && xmlReader->name() == "way"))
    {
        if(xmlReader->tokenType() == QXmlStreamReader::StartElement) {
            if(xmlReader->name() == "nd")
            {
                QXmlStreamAttributes attributes = xmlReader->attributes();
                for (QXmlStreamAttribute attr : attributes)
                {
                    if (attr.name()=="ref")
                    {
                        long id = attr.value().toLong();
                        if (_nodes.contains(id))
                        {
                            polygon.push_back(_nodes[id]);
                        }
                    }
                }
            }
            else if (xmlReader->name() == "tag")
            {
                QXmlStreamAttributes attributes = xmlReader->attributes();
                for (int i = 0; i < attributes.size(); ++i)
                {
                    if (attributes[i].name()=="k")
                    {
                        QString key = attributes[i].value().toString();
                        if (key == "building")
                        {
                            wayType = WayType::BUILDING;
                        }
                        else if (key == "amenity")
                        {
                            QString value = attributes[i+1].value().toString();
                            if (value == "parking")
                                wayType = WayType::PARKING;
                        }
                        else if (key == "highway")
                        {
                            wayType = WayType::ROAD;
                        }
                    }
                }
            }
        }
        /* ...and next... */
        xmlReader->readNext();
    }
    if (*(polygon.begin()) == *(polygon.end()-1))
    {
        if (wayType == WayType::BUILDING)
            _houses.push_back(polygon);
        else if (wayType == WayType::PARKING)
            _parkings.push_back(polygon);
    }
    else if (wayType == WayType::ROAD)
    {
        _roads.push_back(polygon);
    }
    else
    {
        _other.push_back(polygon);
    }
}

void DataGenerator::getNodesAndWaysFromXml()
{
    _nodes.clear();
    _houses.clear();
    _roads.clear();
    // QFile * xmlFile = new QFile("/home/stefan/other/downloads/freiburg.osm");
    QFile * xmlFile = new QFile(":/maps/stuhlinger.osm");
    if (!xmlFile->open(QIODevice::ReadOnly)) {
            QMessageBox::critical(new QWidget,"Load OSM File Problem",
            "Couldn't load map file",
            QMessageBox::Ok);
            return;
    }
    QXmlStreamReader *xmlReader = new QXmlStreamReader(xmlFile);

    //Parse the XML until we reach end of it
    while(!xmlReader->atEnd() && !xmlReader->hasError())
    {
        // Read next element
        QXmlStreamReader::TokenType token = xmlReader->readNext();
        //If token is just StartDocument - go to next
        if(token == QXmlStreamReader::StartDocument)
        {
            continue;
        }
        //If token is StartElement - read it
        if(token == QXmlStreamReader::StartElement)
        {
            if (xmlReader->name() == "bounds")
            {
                updateBounds(xmlReader);
            }
            else if(xmlReader->name() == "node")
            {
                storeNewNode(xmlReader);
            }
            else if (xmlReader->name() == "way")
            {
                storeNewWay(xmlReader);
            }
        }
    }

    if(xmlReader->hasError())
    {
        QMessageBox::critical(new QWidget,
        "xmlFile.xml Parse Error",xmlReader->errorString(),
        QMessageBox::Ok);
        return;
    }

    //close reader and flush file
    xmlReader->clear();
    xmlFile->close();
    delete xmlReader;
    delete xmlFile;
}
