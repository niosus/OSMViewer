#include <QtGui>

#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    shape = Line;
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
    return QSize(400, 200);
}

void RenderArea::setShape(Shape shape)
{
    this->shape = shape;
//    update();
}

void RenderArea::receiveNewData(std::vector<QPoint> coords)
{
    this->coords = coords;
    update();
}

void RenderArea::paintEvent(QPaintEvent * /* event */)
{

    QRect rect(coords[0].x(), coords[0].y(), coords[2].x(), coords[2].y());

    QPainter painter(this);
    painter.setPen(pen);
    painter.setBrush(brush);
    if (antialiased)
        painter.setRenderHint(QPainter::Antialiasing, true);

    painter.save();

    switch (shape) {
    case Line:
        painter.drawLine(rect.bottomLeft(), rect.topRight());
        break;
    case Rect:
        painter.drawRect(rect);
        break;
    }
    painter.restore();


    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.setPen(palette().dark().color());
    painter.setBrush(Qt::NoBrush);
    painter.drawRect(QRect(0, 0, width() - 1, height() - 1));
}
