#include "datagenerator.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QVector3D>
#include "point_with_rot.h"
#include <cmath>

#include "mercator.h"


DataGenerator::DataGenerator(){
}

void DataGenerator::generateData()
{
    getNodesAndWaysFromXml();
    emit dataGenerated(_roads, _houses, _parkings, _other);
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
