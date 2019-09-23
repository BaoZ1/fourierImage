#include "coordinateWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>

coordinateWidget::coordinateWidget(QWidget* O, QListWidget* L, QLabel* PL, vector<listUnit*>(*V), QWidget* parent)
    : QWidget(parent)
{
    this->resize(910, 610);

    coord = new QLabel;
    coord->setFixedHeight(30);

    pointNum = PL;
    pointNum->setFixedHeight(30);

    list = L;
    list->setFixedWidth(290);

    ground = new coordinateSystem(V);

    QHBoxLayout* mainLayout = new QHBoxLayout;
    QVBoxLayout* rightLayout = new QVBoxLayout;
    QVBoxLayout* leftLayout = new QVBoxLayout;

    rightLayout->addWidget(pointNum);
    rightLayout->addWidget(list);
    rightLayout->addWidget(coord);
    leftLayout->addWidget(ground);
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    this->setLayout(mainLayout);

    OP = O;

    connect(ground, SIGNAL(addPoint(complex<double>)), this, SLOT(add(complex<double>)));
    connect(ground, SIGNAL(insertPoint(complex<double>, int)), this, SLOT(insert(complex<double>, int)));
    connect(ground, SIGNAL(coordUpDate(complex<double>)), this, SLOT(labelChange(complex<double>)));
    connect(ground, &coordinateSystem::mouseLeaved, this, [&] {coord->setText("x:-- , y:--"); });
}

coordinateWidget::~coordinateWidget()
{

}

void coordinateWidget::closeEvent(QCloseEvent*)
{
    list->setParent(OP);
    pointNum->setParent(OP);
    emit closing();
}

void coordinateWidget::mouseMoveEvent(QMouseEvent*)
{
    ground->repaint();
}

void coordinateWidget::insert(complex<double> C, int I)
{
    emit insertPoint(C, I);
}

void coordinateWidget::labelChange(complex<double> D)
{
    coord->setText("x:" + QString::number(D.real()) + " , y:" + QString::number(D.imag()));
}

void coordinateWidget::add(complex<double> C)
{
    emit addPoint(C);
}