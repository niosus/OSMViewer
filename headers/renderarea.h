#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <vector>
#include <mypolygonf.h>
#include <QWheelEvent>


//! [0]
class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);

    QVector<double> _bounds;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void updateBounds(QVector<double> &bounds);
    void receiveNewData(QVector<MyPolygonF> &roads,
            QVector<MyPolygonF> &houses,
            QVector<MyPolygonF> &parkings,
            QVector<MyPolygonF> &other);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);


protected:
    void paintEvent(QPaintEvent *event);

private:
    QVector<MyPolygonF> _roads;
    QVector<MyPolygonF> _houses;
    QVector<MyPolygonF> _parkings;
    QVector<MyPolygonF> _other;

    bool _dragging;
    QPointF _startDragPoint;
    QPointF _dragTranslation;
    float _scaleValue;
    void drawRoads();
    void drawHouses();
    void drawParkings();
    void drawOther();

    QTransform _worldToView;
};

#endif
