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

	QPen p1(Qt::black, 1000 / scaleRatio, Qt::DotLine);//������
	QPen p2(Qt::black, 1000 / scaleRatio, Qt::SolidLine);//ʵ��
	QPen p3(Qt::red, 1000 / scaleRatio, Qt::DotLine);//ѡ�е�/�߶�
	QPen p4(Qt::blue, 1000 / scaleRatio, Qt::SolidLine);//�����߶�
	QPen p5(Qt::green, 1000 / scaleRatio, Qt::SolidLine);//������

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
	startingPoint = mouseCoord_widget;//��¼��갴��ʱ��λ��
	if (C_keyPressed)//����C�����ᴴ��һ��Բ
	{
		if (mouseMode.real() == -1)
		{
			//Բ��
			addPoint(mouseCoord_paint);
			points->at(points->size() - 1)->pointMode = listUnit::circleCenter;

			//��Բ�ĵĿ��Ƶ���Ϊ��굱ǰ���ŵĵ�
			points->at(points->size() - 1)->onControl = true;
			mouseMode.real(points->size() - 1);

			//���Բ�ϵĵ㣬����ᱻ��Զѹ�ڿ��Ƶ����棬�޷�ֱ�Ӳ���
			addPoint(mouseCoord_paint);
			points->at(points->size() - 1)->pointMode = listUnit::circlePoint;
			points->at(points->size() - 1)->setRad({ 1,0 });//�����ǻ��ǲ�̫ȷ��������
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
	if (e->pos().x() == startingPoint.real() && e->pos().y() == startingPoint.imag())//���Ƿ�Ϊ�������ж�
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
	if (mouseMode.imag() == pressed)//��갴��
	{
		if (e->buttons() & Qt::LeftButton)//�������
		{
			if (mouseMode.real() != -1)//������ж���
			{
				listUnit* thisPoint = points->at(mouseMode.real());
				if (thisPoint->pointMode == listUnit::circleCenter)//����µĲ�������ĳ��Բ�ĵ�
				{
					if (thisPoint->onControl)//�������һ�����Ƶ�
					{
						thisPoint->setControlData(mouseCoord_paint);
					}
					else//�������Բ��
					{
						thisPoint->setData(mouseCoord_paint);
					}

					for (int i = 1; i <= thisPoint->circlePointNum(); i++)
					{///////////////////��һ��д�Ĳ���Ŷ
						complex<double> data = { thisPoint->r() * cos(thisPoint->rad() + points->at(mouseMode.real() + i)->rad()) , thisPoint->r() * sin(thisPoint->rad() + points->at(mouseMode.real() + i)->rad()) };
						points->at(mouseMode.real() + i)->setData(thisPoint->data() + data);
					}
				}
				else//�������һ����ͨ�㣨һ�����һ���ߣ�
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
	else//���û�а���
	{
		bool mark = true;//������ǿյ�

		if (shiftPressed)//����Shift��Ҫ�ж�����Ƿ���ĳ������
		{
			for (int i = 0; i < points->size(); i++)
			{
				complex<double> P1 = points->at(i)->data();
				complex<double> P2 = points->at((i + 1) % points->size())->data();

				if (P1.imag() == P2.imag())//TODO ����������ǲ�̫����
				{
					//ˮƽ���������
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
					//��ֱ���������
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
					//���������һ��ʽ���ߵ�бʽ
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
		else//û��Shift��Ҫ�ж�������Ƿ���һ����
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

		if (mark)//������������е����֮��
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