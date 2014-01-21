#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QHash>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QWheelEvent>
#include <QPolygonF>
#include "occupancygrid.h"
#include "logReader.h"


//! [0]
class RenderArea : public QWidget
{
    Q_OBJECT

public:
    RenderArea(QWidget *parent = 0);


    QSize minimumSizeHint() const;
    QSize sizeHint() const;


public slots:
    void updateBounds(QHash<QString, double> &bounds);
    void receiveNewData(QVector<QPolygonF> &roads,
            QVector<QPolygonF> &houses,
            QVector<QPolygonF> &parkings,
            QVector<QPolygonF> &other);
    void receiveNewCars(QVector<QPointF> &cars);
    void receiveNewPath(QPolygonF &path);
    void receiveGrids(QHash<QString, OccupancyGrid>& grids);

protected:
    void paintEvent(QPaintEvent *event);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

private:
    QVector<QPolygonF> _roads;
    QVector<QPointF> _cars;
    QVector<QPolygonF> _houses;
    QVector<QPolygonF> _parkings;
    QVector<QPolygonF> _other;
    QPolygonF _path;
    OccupancyGrid _grid;

    QHash<QString, double> _bounds;

    QTransform worldToView;
    QPoint mouseMoveLast;

    void drawRoads(QPainter & painter);
    void drawHouses(QPainter & painter);
    void drawParkings(QPainter & painter);
    void drawCars(QPainter & painter);
    void drawPath(QPainter & painter);
    void drawCameraPos(QPainter & painter);
    void drawOther(QPainter & painter);
    void drawOccupancyGrid(QPainter & painter);

    void drawRuler(QPainter & painter);

    // pen size that is drawn with a width of 1px
    // under the worldToView transformation
    double hairLineWidth();

};

#endif
