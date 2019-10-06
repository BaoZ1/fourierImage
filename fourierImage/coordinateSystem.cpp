#include "coordinateSystem.h"
#include <QPainter>
#include <cmath>

coordinateSystem::coordinateSystem(vector<listUnit*>(*list), vector<bool>(*hiddens), QWidget* parent)
	: QWidget(parent)
{
	points = list;
	mouseMode = { -1, 0 };
	scaleRatio = 1;
	widgetCenter = { 0, 0 };
	widgetCenterOld = { 0, 0 };
	pointSize = 2000;
	homeBtn = new QPushButton(this);
	hiddenLines = hiddens;
	shiftPressed = false;
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
	QPen p3(Qt::red, 1000 / scaleRatio, Qt::DotLine);
	QPen p4(Qt::blue, 1000 / scaleRatio, Qt::SolidLine);

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
		if (hiddenLines->at(i))
		{
			pt.setPen(p4);
			pt.drawLine(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().imag() * 1000 + pointSize / 2 / scaleRatio);
			pt.setPen(p2);
		}
		else
		{
			pt.drawLine(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().imag() * 1000 + pointSize / 2 / scaleRatio);
		}

		if (i == mouseMode.real())
		{
			if (shiftPressed)
			{
				pt.setPen(p3);
				pt.drawLine(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at((i + 1) % points->size())->data().imag() * 1000 + pointSize / 2 / scaleRatio);
				pt.setPen(p2);
			}
		}
	}
	for (int i = 0; i < points->size(); i++)
	{
		pt.drawEllipse(QPoint(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
		if (i == mouseMode.real())
		{
			if (!shiftPressed)
			{
				pt.setPen(p3);
				pt.drawEllipse(QPoint(points->at(i)->data().real() * 1000 - pointSize / 2 / scaleRatio, points->at(i)->data().imag() * 1000 + pointSize / 2 / scaleRatio), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
				pt.setPen(p2);
			}
		}
	}
}

void coordinateSystem::mousePressEvent(QMouseEvent* e)//改鼠标事件为什么会让按shift的时候数组越界？？？？？？？？
{
	startingPoint = mouseCoord_widget;
	mouseMode.imag(pressed);
}

void coordinateSystem::mouseReleaseEvent(QMouseEvent* e)
{
	if (e->pos().x() == startingPoint.real() && e->pos().y() == startingPoint.imag())//对是否为单击的判断
	{
		if (mouseMode.real() != -1)
		{
			if (e->button() == Qt::LeftButton)
			{
				if (shiftPressed)
				{
					emit insertPoint(mouseCoord_paint, mouseMode.real());
				}
			}
			else if (e->button() == Qt::RightButton)
			{
				if (shiftPressed)
				{
					hiddenLines->at(mouseMode.real()) = !hiddenLines->at(mouseMode.real());
				}
				else
				{
					emit delPoint(mouseMode.real());
					mouseMode.real(-1);
				}
			}
		}
		else
		{
			if (e->button() == Qt::LeftButton)
			{
				if (!shiftPressed)
				{
					emit addPoint(mouseCoord_paint);
				}
			}
		}
	}

	mouseMode.imag(released);

	update();
}

void coordinateSystem::mouseMoveEvent(QMouseEvent* e)
{
	if (mouseMode.imag() == pressed)
	{
		if (e->buttons() & Qt::LeftButton)
		{
			if (mouseMode.real() != -1)
			{
				points->at(mouseMode.real())->setData(mouseCoord_paint);
			}
			else
			{
				widgetCenter += {((double)(startingPoint.real() - mouseCoord_widget.real())) / (this->width() / 200.0) / scaleRatio, ((double)(mouseCoord_widget.imag() - startingPoint.imag())) / (this->width() / 200.0) / scaleRatio};
				startingPoint = mouseCoord_widget;
			}
		}
	}
	else
	{
		bool mark = true;

		if (shiftPressed)
		{
			for (int i = 0; i < points->size(); i++)
			{
				complex<double> P1 = points->at(i)->data();
				complex<double> P2 = points->at((i + 1) % points->size())->data();

				if (P1.imag() == P2.imag())//TODO 这玩意好像还是不太正常
				{
					//水平的特殊情况
					bool V = fabs(mouseCoord_paint.imag() - P1.imag()) <= 5 / scaleRatio;
					bool H = (mouseCoord_paint.real() - P1.real()) * (mouseCoord_paint.real() - P2.real()) <= 0;

					if (V && H)
					{
						mouseMode.real(i);
						mark = false;

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
						mouseMode.real(i);
						mark = false;

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
						mouseMode.real(i);
						mark = false;

						break;
					}
				}
			}
		}
		else
		{
			for (int i = 0; i < points->size(); i++)
			{
				if ((pow(mouseCoord_paint.real() - points->at(i)->data().real(), 2) + pow(mouseCoord_paint.imag() - points->at(i)->data().imag(), 2)) <= pow(4 / scaleRatio, 2))
				{
					mouseMode.real(i);
					mark = false;
				}
			}
		}

		if (mark)
		{
			mouseMode.real(-1);
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
		shiftPressed = true;
	}
	update();
}

void coordinateSystem::keyReleaseEvent(QKeyEvent* e)
{
	if (e->key() == Qt::Key_Shift)
	{
		shiftPressed = false;
	}
	update();
}