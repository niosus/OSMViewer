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
    _scaleValue = this->frameSize().width();
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
    qDebug()<<"pressed"<< event->button();
    _dragging = true;
    _startDragPoint = event->pos();
}

void RenderArea::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug()<<"released";
    _dragging = false;
}

void RenderArea::wheelEvent(QWheelEvent *event)
{
    bool good;
    QTransform viewToWorld = _worldToView.inverted(&good);

    QPointF mouseInWorld = viewToWorld.map(event->posF());
    qDebug() << mouseInWorld;

    QPoint numDegrees = event->angleDelta()/3;

    _scaleValue += numDegrees.y();

    _worldToView.reset();
    _worldToView.translate(_dragTranslation.x(), _dragTranslation.y());
    _worldToView.scale(_scaleValue, _scaleValue);
    viewToWorld = _worldToView.inverted(&good);
    QPointF mouseInWorld2 = viewToWorld.map(event->posF());
    qDebug() << mouseInWorld2;
    _dragTranslation += (mouseInWorld2-mouseInWorld)*_scaleValue;
    event->accept();
    update();
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 400);
}

void RenderArea::receiveNewData(
        QVector<MyPolygonF> &roads,
        QVector<MyPolygonF> &houses,
        QVector<MyPolygonF> &parkings,
        QVector<MyPolygonF> &other)
{
    for (MyPolygonF &way : roads)
    {
        way.normalize(_bounds);
    }
    for (MyPolygonF &way : houses)
    {
        way.normalize(_bounds);
    }
    for (MyPolygonF &way : parkings)
    {
        way.normalize(_bounds);
    }
    for (MyPolygonF &way : other)
    {
        way.normalize(_bounds);
    }
    this->_houses = houses;
    this->_roads = roads;
    this->_parkings = parkings;
    this->_other = other;
    update();
}

void RenderArea::updateBounds(QVector<double> &bounds)
{
    _bounds = bounds;
}

void RenderArea::drawRoads()
{
    QPainter painter(this);

    _worldToView.reset();
    _worldToView.translate(_dragTranslation.x(), _dragTranslation.y());
    _worldToView.scale(_scaleValue, _scaleValue);
    painter.setWorldTransform(_worldToView);

    QPen pen(Qt::red);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(0);
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
    painter.scale(_scaleValue, _scaleValue);
    QPen pen(Qt::darkGray);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(0);
    painter.setPen(pen);
    painter.setBrush(Qt::darkGray);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    for (MyPolygonF house : _houses)
    {
        painter.drawPolygon(house);
    }
    painter.restore();
}

void RenderArea::drawParkings()
{
    QPainter painter(this);
    painter.translate(_dragTranslation);
    painter.scale(_scaleValue, _scaleValue);
    QPen pen(Qt::darkBlue);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(0);
    painter.setPen(pen);
    painter.setBrush(Qt::darkBlue);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setOpacity(0.3);
    painter.save();
    for (MyPolygonF parking : _parkings)
    {
        painter.drawPolygon(parking);
    }
    painter.restore();
}

void RenderArea::drawOther()
{
    QPainter painter(this);
    painter.translate(_dragTranslation);
    painter.scale(_scaleValue, _scaleValue);
    QPen pen(Qt::black);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(0);
    painter.setPen(pen);
//    painter.setBrush(Qt::darkGray);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.save();
    for (MyPolygonF other : _other)
    {
        painter.drawPolyline(other);
    }
    painter.restore();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    qDebug()<< "painting";
    drawRoads();
    drawHouses();
    drawParkings();
    drawOther();
}
