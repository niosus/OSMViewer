#include <QtGui>
#include <QDebug>
#include <QPen>

#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(false);
    _dragTranslation = QPointF(0,0);
    _dragging = false;
}

void RenderArea::mouseMoveEvent(QMouseEvent * event)
{
    if (_dragging)
    {
        _dragTranslation += event->pos() - _startDragPoint;
        _startDragPoint = event->pos();
        update();
    }
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    qDebug()<<"pressed";
    _dragging = true;
    _startDragPoint = event->pos();
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<"released";
    _dragging = false;
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 400);
}

void RenderArea::receiveNewData(QVector<MyPolygonF> &roads, QVector<MyPolygonF> &houses)
{
    for (MyPolygonF &way : roads)
    {
        way.normalize(_bounds);
    }
    for (MyPolygonF &way : houses)
    {
        way.normalize(_bounds);
    }
    this->_houses = houses;
    this->_roads = roads;
    update();
}

void RenderArea::updateBounds(QVector<double> &bounds)
{
    _bounds = bounds;
}

void RenderArea::drawRoads()
{
    QPainter painter(this);
    painter.translate(_dragTranslation);
    painter.scale(this->size().width(), this->size().width());
    QPen pen(Qt::red);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(0.01);
    painter.setPen(pen);
    painter.setBrush(Qt::SolidPattern);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.scale(1, 1);
    painter.save();
    for (MyPolygonF road : _roads)
    {
        painter.drawPolyline(road);
    }
    painter.restore();
}

void RenderArea::drawHouses()
{
    QPainter painter(this);
    painter.translate(_dragTranslation);
    painter.scale(this->size().width(), this->size().width());
    QPen pen(Qt::black);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(0);
    painter.setPen(pen);
    painter.setBrush(Qt::SolidPattern);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    for (MyPolygonF house : _houses)
    {
        painter.drawPolygon(house);
    }
    painter.restore();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    qDebug()<< "painting";
    drawRoads();
    drawHouses();
}
