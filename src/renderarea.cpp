#include <QtGui>
#include <QDebug>
#include <QPen>
#include <QPolygonF>
#include <QColor>
#include <QFont>
#include <QFontMetrics>

#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(false);
}

void RenderArea::mouseMoveEvent(QMouseEvent * event)
{
    bool invertible;
    QTransform viewToWorld = this->worldToView.inverted(&invertible);

    if (invertible) {

        // world coordinates of two successive pointer positions
        QPointF pointerLast = viewToWorld.map(QPointF(this->mouseMoveLast));
        QPointF pointer = viewToWorld.map(QPointF(event->globalPos()));

        QPointF dragVector = pointer - pointerLast;
        this->worldToView.translate(dragVector.x(), dragVector.y());

        update();
    }

    this->mouseMoveLast = event->globalPos();
}

void RenderArea::mousePressEvent(QMouseEvent *event)
{
    this->mouseMoveLast = event->globalPos();
}

void RenderArea::wheelEvent(QWheelEvent *event)
{
    double zoom = 1.2;
    double s = event->angleDelta().y() > 0 ? zoom : 1. / zoom;

    bool invertible;
    QTransform viewToWorld = this->worldToView.inverted(&invertible);

    if (invertible) {

        QPointF pointer = viewToWorld.map(QPointF(event->pos()));

        // Read backwards

        // 3. move origin back to where the pointer was
        this->worldToView.translate(pointer.x(), pointer.y());

        // 2. scale about origin
        this->worldToView.scale(s, s);

        // 1. move world so that position of pointer is moved to origin
        this->worldToView.translate(- pointer.x(), -pointer.y());

        update();
    }
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(600, 600);
}

void RenderArea::receiveNewData(
        QVector<QPolygonF> &roads,
        QVector<QPolygonF> &houses,
        QVector<QPolygonF> &parkings,
        QVector<QPolygonF> &other)
{
    this->_houses = houses;
    this->_roads = roads;
    this->_parkings = parkings;
    this->_other = other;

    update();
}

void RenderArea::updateBounds(QHash<QString, double> &bounds)
{
    _bounds = bounds;

    // canvas
    double width = this->frameSize().width();
    double height = this->frameSize().height();
    double canvasSize = qMin(width, height);

    // world
    double worldWidth = (bounds["xMax"] - bounds["xMin"]);
    double worldHeight = (bounds["yMax"] - bounds["yMin"]);
    double worldSize = qMax(worldWidth, worldHeight);
    QPointF worldCenter = QPointF( bounds["xMin"] + worldWidth / 2.,
            bounds["yMin"] + worldHeight / 2.);


    // Transformation
    //    from: pixel coordinate system (upper left is (0,0))
    //    to: map coordinate system (Mercartor'd geo coordinates)
    //
    // You can construct it if you start with the destination system
    // then think how you need to transform the data so it ends up
    // correctly in the origin coordinate system

    this->worldToView.reset();

    // Read backwards

    // 4. move the center of the map (now on upper-left) to the center of the canvas
    this->worldToView.translate( width / 2, height / 2);

    // 3. Flip y-coordinate since pixel coordinate grow downwards
    this->worldToView.scale(1., -1.);

    // 2. Normalize ( " / worldSize " ) then stretch to fill canvas ("canvasSize ")
    this->worldToView.scale(canvasSize / worldSize, canvasSize / worldSize);

    // 1. Center the map
    this->worldToView.translate(- worldCenter.x(), - worldCenter.y());
}

void RenderArea::drawRoads(QPainter & painter)
{
    painter.save();

    QPen pen(QColor(160,160,240));
    pen.setCapStyle(Qt::RoundCap);

    pen.setWidthF(0);
    painter.setPen(pen);

    painter.setBrush(Qt::SolidPattern);

    for (QPolygonF road : _roads)
    {
        painter.drawPolyline(road);
    }

    painter.restore();
}

void RenderArea::drawHouses(QPainter & painter)
{
    painter.save();

    QPen pen(QColor(50,50,50));
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(0.8 * this->hairLineWidth());
    painter.setPen(pen);

    painter.setBrush(Qt::white);
    for (QPolygonF house : _houses)
    {
        painter.drawPolygon(house);
    }

    painter.restore();
}

void RenderArea::drawParkings(QPainter & painter)
{
    painter.save();

    QPen pen(QColor(0,200,10));
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(10 * this->hairLineWidth());
    painter.setPen(pen);

    painter.setBrush(QColor(0,200,10));

    painter.setOpacity(0.8);

    for (QPolygonF parking : _parkings)
    {
        painter.drawPolygon(parking);
    }

    painter.restore();
}

void RenderArea::drawOther(QPainter & painter)
{
    painter.save();

    QPen pen(Qt::gray);
    pen.setCapStyle(Qt::RoundCap);
    pen.setWidthF(this->hairLineWidth());
    painter.setPen(pen);
    painter.setBrush(Qt::darkGray);
    for (QPolygonF other : _other)
    {
        painter.drawPolyline(other);
    }

    painter.restore();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    // draw world
    painter.setWorldTransform(this->worldToView);
    drawParkings(painter);
    drawRoads(painter);
    drawHouses(painter);
    drawOther(painter);

    // draw on map
    painter.setWorldTransform(QTransform());
    drawRuler(painter);
}

double RenderArea::hairLineWidth()
{
    double s = this->worldToView.m11();
    if (s != 0)
        return 1. / s;
    else
        return 1.;
}


void RenderArea::drawRuler(QPainter & painter)
{
    painter.save();
    painter.setOpacity(0.8);

    // ruler in pixels
    int fullRulerWidth = 100;
    int rulerHeight = 10;

    bool invertible;
    QTransform viewToWorld = this->worldToView.inverted(&invertible);
    if (invertible) {

        double fullRulerWidthWorld = fullRulerWidth * viewToWorld.m11();

        // cut off precision
        double magnitude = qFloor( qLn(fullRulerWidthWorld) / qLn(10.));
        double base = qPow(10., magnitude);
        double r = qRound(fullRulerWidthWorld / base);
        double rulerWidthWorld = ceil(r * base);

        // length of ruler in pixels
        int rulerWidth = rulerWidthWorld / viewToWorld.m11();

        int height = this->frameSize().height();
        int margin = 10;
        int rulerX = margin;
        int rulerY = height - ( rulerHeight + margin );

        // draw ruler

        painter.save();
        painter.setRenderHint(QPainter::Antialiasing, false);

        painter.fillRect(rulerX, rulerY, rulerWidth, rulerHeight, QBrush(Qt::white));

        QPen pen(Qt::black);
        pen.setCapStyle(Qt::SquareCap);
        pen.setWidth(2);
        painter.setPen(pen);



        painter.drawLine(rulerX, rulerY, rulerX, rulerY + rulerHeight);
        painter.drawLine(rulerX + rulerWidth, rulerY, rulerX + rulerWidth, rulerY + rulerHeight);

        pen.setWidth(1);
        painter.setPen(pen);
        painter.drawLine(rulerX, rulerY + rulerHeight, rulerX + rulerWidth, rulerY + rulerHeight );

        painter.restore();

        // draw label

        int padding = 2;
        QFont rulerFont("sans");
        rulerFont.setPixelSize(rulerHeight - padding);

        QString label;
        if (rulerWidthWorld >= 1000) {
            label = QString("%1 km").arg(qFloor(rulerWidthWorld / 1000));
        } else {
            label = QString("%1 m").arg(rulerWidthWorld);
        }
        QFontMetrics metrics(rulerFont);

        painter.setFont(rulerFont);
        painter.drawText(rulerX + rulerWidth / 2 - metrics.width(label) / 2, rulerY + rulerHeight - padding, label);


    }


    painter.restore();
}
