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
    enum Shape { Line, Points, Rect};

    RenderArea(QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

public slots:
    void receiveNewData(std::vector<QPoint> coords);
    void setShape(Shape shape);

protected:
    void paintEvent(QPaintEvent *event);

private:
    Shape shape;
    QPen pen;
    QBrush brush;
    bool antialiased;
    std::vector<QPoint> coords;
};

#endif
