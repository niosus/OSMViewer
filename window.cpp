#include <QtGui>

#include "renderarea.h"
#include "window.h"
#include "datagenerator.h"

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
                dataGenerator, SIGNAL(dataGenerated(std::vector<QPoint>)),
                renderArea, SLOT(receiveNewData(std::vector<QPoint>)));
    dataGenerator->generateData();
}

