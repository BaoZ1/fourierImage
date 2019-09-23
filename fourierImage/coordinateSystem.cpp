#include "coordinateSystem.h"
#include <QPainter>
#include <cmath>

coordinateSystem::coordinateSystem(vector<listUnit*>(*V), QWidget* parent)
    : QWidget(parent)
{
    points = V;
    mouseMode = { -1,0 };
    scaleRatio = 1;
    widgetCenter = { 0,0 };
    pointSize = 2000;
    homeBtn = new QPushButton(this);

    this->setMouseTracking(true);

    homeBtn->setGeometry(570, 120, 30, 30);
    homeBtn->setText("H");

    connect(homeBtn, &QPushButton::clicked, [&] {
        widgetCenter = { 0, 0 };
        scaleRatio = 1;
        update();
        });
}

coordinateSystem::~coordinateSystem()
{
}

void coordinateSystem::paintEvent(QPaintEvent*)
{
    mouseCoord_paint = { (mouseCoord_widget.real() - (this->width() / 2.0)) / 3 / scaleRatio + widgetCenter.real(), (-(mouseCoord_widget.imag()) + (this->height() / 2.0)) / 3 / scaleRatio + widgetCenter.imag() };
    emit coordUpDate(mouseCoord_paint);

    if (mouseMode.real() >= 0)
    {
        this->setCursor(Qt::CrossCursor);
    }
    else
    {
        this->setCursor(Qt::ArrowCursor);
    }

    QPainter pt(this);

    QPen p1(Qt::black, 1000 / scaleRatio, Qt::DotLine);
    QPen p2(Qt::black, 1000 / scaleRatio, Qt::SolidLine);
    QPen p3(Qt::red, 1000 / scaleRatio, Qt::SolidLine);


    pt.setWindow((-100 * (this->width() / 600.0) / scaleRatio + widgetCenter.real()) * 1000, (100 * (this->height() / 600.0) / scaleRatio + widgetCenter.imag()) * 1000, 200 * (this->width() / 600.0) * 1000 / scaleRatio, -200 * (this->height() / 600.0) * 1000 / scaleRatio);
    pt.setRenderHint(QPainter::Antialiasing, true);

    pt.setPen(p1);
    pt.drawLine(-200000, 0, 200000, 0);
    pt.drawLine(0, 200000, 0, -200000);
    pt.drawRect(200000, 200000, -400000, -400000);

    pt.setPen(p2);
    pt.setBrush(QColor(Qt::black));
    for (int i = 0; i < points->size(); i++)
    {
        if (i == mouseMode.real())
        {
            if (mouseMode.imag() == 4)
            {
                pt.setPen(p3);
                pt.drawLine(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().imag() * 1000 + pointSize / 2 / scaleRatio);
                pt.setPen(p2);
                pt.drawEllipse(QPoint(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
            }
            else
            {
                pt.setPen(p3);
                pt.drawEllipse(QPoint(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
                pt.setPen(p2);
                pt.drawLine(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().imag() * 1000 + pointSize / 2 / scaleRatio);
            }
        }
        else
        {
            pt.drawEllipse(QPoint(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
            pt.drawLine(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().imag() * 1000 + pointSize / 2 / scaleRatio);
        }
    }
}

void coordinateSystem::mousePressEvent(QMouseEvent*)
{
    if (mouseMode.imag() == 0)
    {
        if (mouseMode.real() == -1)
        {
            startingPoint = mouseCoord_widget;
            mouseMode += {0, 1};
        }
        else
        {
            mouseMode += {0, 3};
        }
    }
    else if (mouseMode.imag() == 4)
    {
        if (mouseMode.real() != -1)
        {
            mouseMode += {0, 1};
        }
    }
}

void coordinateSystem::mouseReleaseEvent(QMouseEvent*)
{
    if (mouseMode.imag() == 1)
    {
        if (fabs(mouseCoord_paint.real()) < 200 && fabs(mouseCoord_paint.imag()) < 200)
        {
            mouseMode = { (int)points->size(), 0 };
            emit addPoint(mouseCoord_paint);
        }
        else
        {
            mouseMode = { -1, 0 };
        }
    }
    else if (mouseMode.imag() == 2)
    {
        mouseMode -= {0, 2};
    }
    else if (mouseMode.imag() == 3)
    {
        mouseMode -= {0, 3};
    }
    else if (mouseMode.imag() == 5)
    {
        if (mouseMode.real() != -1)
        {
            emit insertPoint(mouseCoord_paint, mouseMode.real());
        }
        mouseMode -= {0, 1};
    }
    update();
}

void coordinateSystem::mouseMoveEvent(QMouseEvent* e)
{
    switch (mouseMode.imag())
    {
    case 0:
        mouseMode = { -1,0 };
        for (int i = 0; i < points->size(); i++)
        {
            if ((pow(mouseCoord_paint.real() - points->at(i)->data().real(), 2) + pow(mouseCoord_paint.imag() - points->at(i)->data().imag(), 2)) <= pow(4 / scaleRatio, 2))
            {
                mouseMode = { i, 0 };
            }
        }
        break;
    case 1:
        mouseMode += {0, 1};
    case 2:
        widgetCenter += {((double)(startingPoint.real() - mouseCoord_widget.real())) / (this->width() / 200.0) / scaleRatio, ((double)(mouseCoord_widget.imag() - startingPoint.imag())) / (this->width() / 200.0) / scaleRatio};
        startingPoint = mouseCoord_widget;
        break;
    case 3:
        points->at(mouseMode.real())->setData(mouseCoord_paint);
        break;
    case 4:
        mouseMode = { -1, 4 };

        for (int i = 0; i < points->size(); i++)
        {
            complex<double> P1 = points->at(i)->data();
            complex<double> P2 = points->at((i + 1) % points->size())->data();

            if (P1.imag() == P2.imag())
            {
                //水平的特殊情况
                bool V = fabs(mouseCoord_paint.imag() - P1.imag()) <= 5 / scaleRatio;
                bool H = (mouseCoord_paint.real() - P1.real()) * (mouseCoord_paint.real() - P2.real()) <= 0;

                if (V && H)
                {
                    mouseMode = { i, 4 };
                    break;
                }
            }
            else if (P1.real() == P2.real())
            {
                //竖直的特殊情况
                bool V = (mouseCoord_paint.imag() - P1.imag()) * (mouseCoord_paint.imag() - P2.imag()) <= 0;
                bool H = fabs(mouseCoord_paint.real() - P1.real()) <= 5 / scaleRatio;

                if (V && H)
                {
                    mouseMode = { i, 4 };
                    break;
                }
            }
            else
            {
                //其他情况，一般式或者点斜式
                double K = (P2.imag() - P1.imag()) / (P2.real() - P1.real());
                double B = P1.imag() - K * P1.real();
                //y = kx + b -> -kx + y - b = 0
                double k = 1 / K;
                double b1 = P1.imag() - k * P1.real();
                double b2 = P2.imag() - k * P2.real();

                double bm = mouseCoord_paint.imag() - k * mouseCoord_paint.real();
                bool V = (bm - b1) * (bm - b2) <= 0;
                bool H = (fabs(-K * mouseCoord_paint.real() + mouseCoord_paint.imag() - B) / sqrt(K * K + 1)) <= 5 / scaleRatio;

                if (V && H)
                {
                    mouseMode = { i, 4 };
                    break;
                }
            }
        }
    }

    mouseCoord_widget = { e->pos().x(), e->pos().y() };
    update();
}

void coordinateSystem::wheelEvent(QWheelEvent* e)
{
    if (e->delta() > 0)
    {
        scaleRatio = scaleRatio >= 1000 ? 1000 : scaleRatio *= 1.1;
    }
    else
    {
        scaleRatio = scaleRatio <= 0.3 ? 0.3 : scaleRatio /= 1.1;
    }
    update();
}

void coordinateSystem::leaveEvent(QEvent*)
{
    emit mouseLeaved();
}

void coordinateSystem::resizeEvent(QResizeEvent*)
{
    this->homeBtn->setGeometry(this->width() - 30, (int)(this->height() * 0.2), 30, 30);
}

void coordinateSystem::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Shift)
    {
        mouseMode = { -1, 4 };
    }
    update();
}

void coordinateSystem::keyReleaseEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Shift)
    {
        mouseMode = { -1, 0 };
    }
    update();
}

