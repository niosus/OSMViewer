#include "datagenerator.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <cmath>

#include "mercator.h"


DataGenerator::DataGenerator(){}

void DataGenerator::generateData()
{
    getNodesAndWaysFromXml();
    getCarsFromLogFiles();
    emit dataGenerated(_roads, _houses, _parkings, _other);
    emit carsGenerated(_cars);
}

void DataGenerator::getCarsFromLogFiles()
{
    QFile * logImagesGps = new QFile(":/log/log_images_gps.txt");
    QFile * logImagesRects = new QFile(":/log/output_log.dat");
    if (!logImagesGps->open(QIODevice::ReadOnly)
            || !logImagesRects->open(QIODevice::ReadOnly))
    {
        QMessageBox::critical(new QWidget,"No Log File",
        "log file does not exist",
        QMessageBox::Ok);
        return;
    }
    QMap<QString, QPointF> imageGpsHash;
    QMap<QString, QRectF> imageCarRectHash;

    QTextStream inGps(logImagesGps);
    while(!inGps.atEnd()) {
        QString line = inGps.readLine();
        QStringList fields = line.split("\t\t");
        QString name = fields[1].split("=")[1];
        float lon = fields[2].split("=")[1].toFloat();
        float lat = fields[3].split("=")[1].toFloat();
        imageGpsHash[name]=QPointF(lon,lat);
    }

    QTextStream inRects(logImagesRects);
    while(!inRects.atEnd()) {
        QString line = inRects.readLine();
        QStringList fields = line.split("\t");
        QString name = fields[1];
        float x = fields[3].toFloat();
        float y = fields[4].toFloat();
        float width = fields[6].toFloat();
        float height = fields[7].toFloat();
        imageCarRectHash[name]=QRectF(x,y, width, height);
    }
    getCarPositionsFromAllData(imageCarRectHash, imageGpsHash);

    logImagesGps->close();
    logImagesRects->close();
    delete logImagesGps;
    delete logImagesRects;
}

QPointF DataGenerator::getPrevGpsPoint(const QString &name, const QMap<QString, QPointF> &imageGpsHash)
{
    QMap<QString, QPointF>::const_iterator pointIter;
    pointIter = imageGpsHash.find(name);
    if (pointIter==imageGpsHash.end())
    {
        return QPointF();
    }
    QPointF refPoint = *pointIter;
    while (pointIter!=imageGpsHash.begin() && *pointIter == refPoint)
    {
        pointIter--;
    }
    return *pointIter;
}


void DataGenerator::getCarPositionsFromAllData(
        const QMap<QString, QRectF> &imageCarRectHash,
        const QMap<QString, QPointF> &imageGpsHash)
{
    const float gradInPx = 0.13f;
    const float f = -14.85f;
    const float objectSize = 1.6f;
    _cars.clear();
    for (auto name:imageCarRectHash.keys())
    {
        QRectF carRect = imageCarRectHash.value(name);
        QPointF carRectCenter = QPointF(
                    carRect.x() + carRect.width()/2,
                    carRect.y() + carRect.height()/2);
        qDebug()<<"car center on image" << carRectCenter;
        float distanceFromCamera = (-carRect.height() * objectSize)/f;
        qDebug()<<"car distance from camera" << distanceFromCamera;
        float carAnglePos = 90.0f - carRectCenter.x()*gradInPx;
        qDebug()<<"car angle from center" << distanceFromCamera;
        carAnglePos = carAnglePos * M_PI / 180.0;
        float carX = distanceFromCamera * cos(carAnglePos);
        float carY = distanceFromCamera * sin(carAnglePos);
        QPointF carInCameraViewPosition(carX, carY);
        qDebug()<<"car in camera" << carInCameraViewPosition;

        QPointF thisPoint = imageGpsHash.value(name);
        QPointF prevPoint = getPrevGpsPoint(name, imageGpsHash);
        QPointF direction = thisPoint - prevPoint;
        float angleOfThisGpsPointSystem = atan2(direction.y(), direction.x());
        QTransform transform;
        transform.translate(merc_x(thisPoint.x()), merc_y(thisPoint.y()));
        transform.rotate(angleOfThisGpsPointSystem);
        QPointF carGlobalPos = transform.map(carInCameraViewPosition);
        qDebug() << carGlobalPos;
        _cars.push_back(carGlobalPos);
    }
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
}
