#ifndef TEST_BRESENHEM_H
#define TEST_BRESENHEM_H

#include <QtTest/QtTest>
#include "gridtraversal.h"

class TestGridTraversal: public QObject
{
    Q_OBJECT
private slots:
    void gridLine();
};

#endif // TEST_BRESENHEM_H
