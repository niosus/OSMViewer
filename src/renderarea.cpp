#include <QtGui>
#include <QDebug>

#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    shape = Polygon;
    antialiased = true;

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(false);
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize RenderArea::sizeHint() const
{
    return QSize(400, 400);
}

void RenderArea::receiveNewData(QVector<QPolygonF>& ways)
{
    this->_ways = ways;
    update();
}

void RenderArea::updateBounds(const QVector<double> &bounds)
{
    _bounds = bounds;
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    painter.save();

    switch (shape) {
    case Polygon:
        for (QPolygonF way : _ways)
        {
            if (*way.begin() == *(way.end()-1))
            {
                painter.drawPolygon(way);
            }
            else
            {
                painter.drawPolyline(way);
            }
        }
        break;
    }
    painter.restore();


    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}
