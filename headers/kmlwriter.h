#ifndef KMLWRITER_H
#define KMLWRITER_H

// createkml.cc
// This program uses the KmlFactory to create a Point Placemark and
// prints the resultant KML on standard output.

#include <iostream>
#include <QString>
#include <QFile>
#include <QPointF>
#include <QTextStream>
#include "kml/dom.h"

// libkml types are in the kmldom namespace
using kmldom::CoordinatesPtr;
using kmldom::KmlPtr;
using kmldom::KmlFactory;
using kmldom::PlacemarkPtr;
using kmldom::PointPtr;
using kmldom::StylePtr;
using kmldom::IconStylePtr;

void writeTestKml();

class KmlWriter
{
public:
    KmlWriter(QString name="/home/igor/autoTest.kml");
    void addPoint(const int &id, const QString &name, const QPointF &coords);
    ~KmlWriter();
private:
    QString _kmlFileName;
};

#endif // KMLWRITER_H
