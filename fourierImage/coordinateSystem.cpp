#include "coordinateSystem.h"
#include <QPainter>
#include <cmath>

coordinateSystem::coordinateSystem(vector<listUnit*>(*list), QWidget* parent)
	: QWidget(parent)
{
	points = list;
	mouseMode = { -1, 0 };
	scaleRatio = 1;
	widgetCenter = { 0, 0 };
	widgetCenterOld = { 0, 0 };
	pointSize = 2000;
	homeBtn = new QPushButton(this);
	shiftPressed = false;
	C_keyPressed = false;
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

	QPen p1(Qt::black, 1000 / scaleRatio, Qt::DotLine);//坐标轴
	QPen p2(Qt::black, 1000 / scaleRatio, Qt::SolidLine);//实线
	QPen p3(Qt::red, 1000 / scaleRatio, Qt::DotLine);//选中点/线段
	QPen p4(Qt::blue, 1000 / scaleRatio, Qt::SolidLine);//隐藏线段
	QPen p5(Qt::green, 1000 / scaleRatio, Qt::SolidLine);//辅助线

	pt.setWindow((-100 * (this->width() / 600.0) / scaleRatio + widgetCenter.real()) * 1000, (100 * (this->height() / 600.0) / scaleRatio + widgetCenter.imag()) * 1000, 200 * (this->width() / 600.0) * 1000 / scaleRatio, -200 * (this->height() / 600.0) * 1000 / scaleRatio);
	pt.setRenderHint(QPainter::Antialiasing, true);

	pt.setPen(p1);

	pt.drawLine(-200000, 0, 200000, 0);
	pt.drawLine(0, 200000, 0, -200000);
	pt.drawRect(200000, 200000, -400000, -400000);

	if (points->size() == 0)
	{
		return;
	}

	listUnit* thisPoint;
	listUnit* nextPoint = points->at(0);

	for (int i = 0; i < points->size(); i++)
	{
		thisPoint = nextPoint;
		nextPoint = points->at((i + 1) % points->size());

		pt.setPen(thisPoint->lineMode == listUnit::show ? p2 : p4);

		switch (thisPoint->pointMode)
		{
		case listUnit::pathPoint:
			if (nextPoint->pointMode == listUnit::circleCenter)
			{
				pt.drawLine(thisPoint->coord(), nextPoint->controlPointCoord());
			}
			else
			{
				pt.drawLine(thisPoint->coord(), nextPoint->coord());
			}

			if (i == mouseMode.real() && shiftPressed)
			{
				pt.setPen(p3);

				pt.drawLine(thisPoint->coord(), nextPoint->coord());
			}

			break;

		case listUnit::circleCenter:
			QRect rect = thisPoint->rect();
			for (int j = 0; j < thisPoint->circlePointNum(); j++)
			{
				pt.drawArc(rect, (int)(points->at(i + j)->rad(1) * 16), (int)(points->at(i + j + 1)->rad(1) * 16 + nextPoint->rad(1) * 16));
			}

			pt.drawLine(points->at(i + thisPoint->circlePointNum())->coord(), points->at((i + thisPoint->circlePointNum() + 1) % points->size())->coord());

			i += thisPoint->circlePointNum();

			break;
		}
	}

	pt.setPen(p2);
	pt.setBrush(QColor(Qt::black));

	for (int i = 0; i < points->size(); i++)
	{
		listUnit* P = points->at(i);

		pt.drawEllipse(P->coord(), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));

		if (P->pointMode == listUnit::circleCenter)
		{
			pt.setPen(p5);
			pt.drawEllipse(P->controlPointCoord(), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
			pt.setPen(p2);

			for (int j = 1; j < P->circlePointNum(); j++)
			{
				pt.drawEllipse(points->at(i + j)->coord(), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
			}

			i += P->circlePointNum();
		}

		if (i == mouseMode.real())
		{
			if (!shiftPressed)
			{
				pt.setPen(p3);
				pt.drawEllipse((P->pointMode == listUnit::circleCenter && P->onControl) ? (P->controlPointCoord()) : (P->coord()), (int)(pointSize / scaleRatio), (int)(pointSize / scaleRatio));
				pt.setPen(p2);
			}
		}
	}
}

void coordinateSystem::mousePressEvent(QMouseEvent* e)
{
	startingPoint = mouseCoord_widget;//记录鼠标按下时的位置
	if (C_keyPressed)//按着C键，会创建一个圆
	{
		if (mouseMode.real() == -1)
		{
			//圆心
			addPoint(mouseCoord_paint);
			points->at(points->size() - 1)->pointMode = listUnit::circleCenter;

			//把圆心的控制点设为鼠标当前按着的点
			points->at(points->size() - 1)->onControl = true;
			mouseMode.real(points->size() - 1);

			//添加圆上的点，这个会被永远压在控制点下面，无法直接操作
			addPoint(mouseCoord_paint);
			points->at(points->size() - 1)->pointMode = listUnit::circlePoint;
			points->at(points->size() - 1)->setRad({ 1,0 });//反三角还是不太确定。。。
			points->at(points->size() - 2)->insertCirclePoint();
		}
		else
		{
			points->at(mouseMode.real())->pointMode = listUnit::circleCenter;
			points->at(mouseMode.real())->onControl = true;
			points->at(mouseMode.real())->insertCirclePoint();

			insertPoint(mouseCoord_paint, mouseMode.real() + 1);
			points->at(mouseMode.real() + 1)->pointMode = listUnit::circlePoint;
		}
	}

	mouseMode.imag(pressed);

	update();
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
					points->at(mouseMode.real())->lineMode = (points->at(mouseMode.real())->lineMode == listUnit::show) ? (listUnit::hide) : (listUnit::show);
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
	if (mouseMode.imag() == pressed)//鼠标按下
	{
		if (e->buttons() & Qt::LeftButton)//左键按下
		{
			if (mouseMode.real() != -1)//光标下有东西
			{
				listUnit* thisPoint = points->at(mouseMode.real());
				if (thisPoint->pointMode == listUnit::circleCenter)//光标下的部分属于某个圆心点
				{
					if (thisPoint->onControl)//光标下是一个控制点
					{
						thisPoint->setControlData(mouseCoord_paint);
					}
					else//光标下是圆心
					{
						thisPoint->setData(mouseCoord_paint);
					}

					for (int i = 1; i <= thisPoint->circlePointNum(); i++)
					{///////////////////这一段写的不对哦
						complex<double> data = { thisPoint->r() * cos(thisPoint->rad() + points->at(mouseMode.real() + i)->rad()) , thisPoint->r() * sin(thisPoint->rad() + points->at(mouseMode.real() + i)->rad()) };
						points->at(mouseMode.real() + i)->setData(thisPoint->data() + data);
					}
				}
				else//光标下是一个普通点（一个点带一个线）
				{
					thisPoint->setData(mouseCoord_paint);
				}

				if (C_keyPressed)
				{
					thisPoint->setControlData(mouseCoord_paint);
				}
			}
			else
			{
				widgetCenter += {((double)(startingPoint.real() - mouseCoord_widget.real())) / (this->width() / 200.0) / scaleRatio, ((double)(mouseCoord_widget.imag() - startingPoint.imag())) / (this->width() / 200.0) / scaleRatio};
				startingPoint = mouseCoord_widget;
			}
		}
	}
	else//鼠标没有按下
	{
		bool mark = true;//鼠标下是空的

		if (shiftPressed)//按着Shift，要判断鼠标是否在某根线上
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
		else//没按Shift，要判断鼠标下是否有一个点
		{
			for (int i = 0; i < points->size(); i++)
			{
				switch (points->at(i)->pointMode)
				{
				case listUnit::pathPoint:
				case listUnit::circlePoint:
					if ((pow(mouseCoord_paint.real() - points->at(i)->data().real(), 2) + pow(mouseCoord_paint.imag() - points->at(i)->data().imag(), 2)) <= pow(4 / scaleRatio, 2))
					{
						mouseMode.real(i);
						mark = false;
						break;
					}
					break;

				case listUnit::circleCenter:
					if ((pow(mouseCoord_paint.real() - points->at(i)->data().real(), 2) + pow(mouseCoord_paint.imag() - points->at(i)->data().imag(), 2)) <= pow(4 / scaleRatio, 2))
					{
						mouseMode.real(i);
						points->at(i)->onControl = false;
						mark = false;
						break;
					}
					else if ((pow(mouseCoord_paint.real() - points->at(i)->controlPointData().real(), 2) + pow(mouseCoord_paint.imag() - points->at(i)->controlPointData().imag(), 2)) <= pow(4 / scaleRatio, 2))
					{
						mouseMode.real(i);
						points->at(i)->onControl = true;
						mark = false;
						break;
					}
					break;
				}

				if (!mark)
				{
					break;
				}
			}
		}

		if (mark)//鼠标坐标在所有点或线之外
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
	switch (e->key())
	{
	case Qt::Key_Shift:
		shiftPressed = true;

		break;

	case Qt::Key_C:
		C_keyPressed = true;

		break;
	}
	update();
}

void coordinateSystem::keyReleaseEvent(QKeyEvent* e)
{
	switch (e->key())
	{
	case Qt::Key_Shift:
		shiftPressed = false;

		break;

	case Qt::Key_C:
		C_keyPressed = false;

		break;
	}
	update();
}