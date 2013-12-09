#ifndef MYPOINT_H
#define MYPOINT_H

#include <QPointF>

class MyPointF: public QPointF
{
public:
    MyPointF(const qreal& x, const qreal& y, const qreal& theta):
        QPointF(x, y),
        _theta(theta) {}
    MyPointF():QPointF() {}
    qreal theta() const { return _theta; }
    qreal& rTheta() { return _theta; }
private:
    qreal _theta;
};

#endif // MYPOINT_H
