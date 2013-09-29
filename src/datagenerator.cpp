#include "datagenerator.h"
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamAttributes>
#include <QMessageBox>
#include <QDebug>


DataGenerator::DataGenerator(){}

void DataGenerator::generateData()
{
    getNodesAndWaysFromXml();
    emit dataGenerated(_ways);
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
            //HACK!!!!!
            if (_bounds.size()>0)
            {
                lat -=  _bounds[0];
                lat /= (_bounds[2] - _bounds[0]);
                lat*=400;
            }
        }
        else if (attr.name()=="lon")
        {
            lon = attr.value().toDouble();
            //HACK!!!!!
            if (_bounds.size()>0)
            {
                lon -=  _bounds[1];
                lon /= (_bounds[3] - _bounds[1]);
                lon*=400;
            }
        }
    }
    if (id && lat && lon)
    {
        QPointF point(lat, lon);
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
    if (minLat && minLon && maxLat && maxLon)
    {
        _bounds.clear();
        _bounds.push_back(minLat);
        _bounds.push_back(minLon);
        _bounds.push_back(maxLat);
        _bounds.push_back(maxLon);
        emit boundariesUpdated(_bounds);
    }
}

void DataGenerator::storeNewWay(QXmlStreamReader *xmlReader)
{
    //We are going to fill this Polygon now
    QPolygonF polygon;

    //loop through everything that a way contains
    while(!(xmlReader->tokenType() == QXmlStreamReader::EndElement
            && xmlReader->name() == "way"))
    {
        if(xmlReader->tokenType() == QXmlStreamReader::StartElement) {
            if(xmlReader->name() == "nd") {
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
        }
        /* ...and next... */
        xmlReader->readNext();
    }
    _ways.push_back(polygon);
}

void DataGenerator::getNodesAndWaysFromXml()
{
    _nodes.clear();
    _ways.clear();
    QFile *xmlFile = new QFile(":/maps/map_test.osm");
    if (!xmlFile->open(QIODevice::ReadOnly)) {
            QMessageBox::critical(new QWidget,"Load XML File Problem",
            "Couldn't open xmlfile.xml to load settings for download",
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
