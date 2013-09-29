#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <vector>


//! [0]
class RenderArea : public QWidget
{
    Q_OBJECT

public:
    enum Shape { Line, Points, Rect, Polygon};

    RenderArea(QWidget *parent = 0);

    QVector<double> _bounds;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void updateBounds(const QVector<double> &bounds);
    void receiveNewData(QVector<QPolygonF> &ways);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Shape shape;
    QPen pen;
    QBrush brush;
    bool antialiased;
    QVector<QPolygonF> _ways;
};

#endif
