#ifndef MYPOLYGONF_H
#define MYPOLYGONF_H

#include <QPolygonF>
#include <QVector>
#include <QDebug>

class MyPolygonF: public QPolygonF
{
public:
    void normalize(const QVector<double>& bounds)
    {
        for (QPointF &p: *this)
        {
            p.rx() = ((p.x() - bounds[0]) / (bounds[2] - bounds[0]));
            p.ry() = ((p.y() - bounds[1]) / (bounds[3] - bounds[1]));
        }
    }
};

#endif // MYPOLYGONF_H
