#include <QtGui>
#include <QGridLayout>

#include "renderarea.h"
#include "window.h"
#include "datagenerator.h"
#include "mypolygonf.h"

const int IdRole = Qt::UserRole;

Window::Window()
{
    renderArea = new RenderArea;
    dataGenerator = new DataGenerator;

    QGridLayout *mainLayout = new QGridLayout;

    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(3, 1);
    mainLayout->addWidget(renderArea, 0, 0, 1, 4);

    setLayout(mainLayout);

    setWindowTitle(tr("Test Map"));

    QObject::connect(
                dataGenerator, SIGNAL(dataGenerated(QVector<MyPolygonF>&, QVector<MyPolygonF>&)),
                renderArea, SLOT(receiveNewData(QVector<MyPolygonF>&, QVector<MyPolygonF>&)));
    QObject::connect(
                dataGenerator, SIGNAL(boundariesUpdated(QVector<double>&)),
                renderArea, SLOT(updateBounds(QVector<double>&)));
    dataGenerator->generateData();
}

