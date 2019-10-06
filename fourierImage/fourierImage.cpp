#include "fourierImage.h"
#include <QPainter>
#include <cmath>

fourierImage::fourierImage(bool B, int I1, int I2, int I3, vector<complex<double> >(*V), int threadNum, double length, vector<complex<double> >(*hiddens), QWidget* parent)
	: QWidget(parent)
{
	this->resize(I1, I2);
	this->setWindowTitle(tr("Fourier Image"));
	this->setMouseTracking(true);

	QPalette palette(this->palette());
	palette.setColor(QPalette::Background, Qt::black);
	this->setPalette(palette);

	coefficients_I = V;
	settingWidth = I1;
	settingHeight = I2;
	scaleRatio = 1;
	widgetCenter = { 0,0 };
	pressed = false;
	suspended = false;
	following = false;
	fineness = I3;
	antialiasing = B;
	imageProgress = 0;
	totalLength = length;
	hiddenPaths = hiddens;
	PATH = new QPainterPath;
	path = new QPainterPath;
	timer = new QTimer(this);;

	suspendBtn = new QPushButton(this);
	suspendBtn->setText(tr("suspend"));
	suspendBtn->setGeometry(I1 - 100, (int)(I2 * 0.3), 100, 30);

	followBtn = new QPushButton(this);
	followBtn->setText(tr("follow"));
	followBtn->setGeometry(I1 - 100, (int)(I2 * 0.3) - 40, 100, 30);

	homeBtn = new QPushButton(this);
	homeBtn->setText("H");
	homeBtn->setGeometry(I1 - 100, (int)(I2 * 0.3) - -80, 100, 30);

	connect(suspendBtn, &QPushButton::clicked, [&] {
		if (suspended)
		{
			timer->start();
			suspendBtn->setText(tr("suspend"));
			suspended = false;
		}
		else
		{
			timer->stop();
			suspendBtn->setText(tr("continue"));
			suspended = true;
		}
		}
	);

	connect(followBtn, &QPushButton::clicked, [&] {
		if (following)
		{
			followBtn->setText(tr("follow"));
			following = false;
		}
		else
		{
			followBtn->setText(tr("stop follow"));
			following = true;
		}
		}
	);

	connect(homeBtn, &QPushButton::clicked, [&] {widgetCenter = 0; scaleRatio = 1; });

	connect(timer, &QTimer::timeout, [&] {calculate(); });

	timer->start(10);
}

void fourierImage::paintEvent(QPaintEvent*)
{
	QPainter pt(this);

	pt.setWindow((-100 * (this->width() / (double)settingWidth) / scaleRatio + widgetCenter.real()) * 1000, (100 * (this->height() / (double)settingHeight) / scaleRatio + widgetCenter.imag()) * 1000, 200 * (this->width() / (double)settingWidth) * 1000 / scaleRatio, -200 * (this->height() / (double)settingHeight) * 1000 / scaleRatio);

	if (antialiasing)
	{
		pt.setRenderHint(QPainter::Antialiasing, true);
	}

	pt.setPen(QColor(Qt::red));
	pt.setBrush(QColor(Qt::red));
	pt.drawEllipse(QPoint(0, 0), (int)(2000 / scaleRatio), (int)(2000 / scaleRatio));

	pt.setBrush(Qt::NoBrush);

	QPen p1(Qt::yellow, 1000 / scaleRatio, Qt::SolidLine);
	QPen p2(Qt::white, 500 / scaleRatio, Qt::SolidLine);
	QPen p3(Qt::cyan, 200 / scaleRatio, Qt::SolidLine);
	QPen p4(Qt::magenta, 750 / scaleRatio, Qt::SolidLine);

	pt.setPen(p1);
	pt.drawPath(*PATH);

	pt.setPen(p2);
	pt.drawPath(*path);

	double minimumSize = 1000 / scaleRatio;

	for (int i = 0; i < path->elementCount() - 1; i++)
	{
		double a = path->elementAt(i + 1).x - path->elementAt(i).x;
		double b = path->elementAt(i + 1).y - path->elementAt(i).y;
		if ((fabs(a) < minimumSize) && (fabs(b) < minimumSize))
		{
			continue;
		}

		int r = (int)sqrt(pow(a, 2) + pow(b, 2));

		pt.setPen(p3);
		pt.setBrush(Qt::NoBrush);
		pt.drawEllipse(QPoint(path->elementAt(i).x, path->elementAt(i).y), r, r);

		/*pt.setPen(p4);
		pt.setBrush(QColor(Qt::magenta));
		pt.drawEllipse(QPoint(path->elementAt(i).x, path->elementAt(i).y), (int)(500 / scaleRatio), (int)(500 / scaleRatio));*/
	}
}

void fourierImage::mousePressEvent(QMouseEvent*)
{
	pressed = true;
	startingPoint = mouseCoord_widget;
}

void fourierImage::mouseReleaseEvent(QMouseEvent*)
{
	pressed = false;
}

void fourierImage::mouseMoveEvent(QMouseEvent* e)
{
	if (pressed && !following)
	{
		widgetCenter += {((double)(startingPoint.real() - mouseCoord_widget.real())) / (this->width() / 200) / scaleRatio, ((double)(mouseCoord_widget.imag() - startingPoint.imag())) / (this->height() / 200) / scaleRatio};
		startingPoint = mouseCoord_widget;
	}

	mouseCoord_widget = { e->pos().x(), e->pos().y() };
	update();
}

void fourierImage::wheelEvent(QWheelEvent* e)
{
	if (e->delta() > 0)
	{
		scaleRatio = scaleRatio >= 1000 ? 1000 : scaleRatio *= 1.1;
	}
	else
	{
		scaleRatio = scaleRatio <= 0.5 ? 0.5 : scaleRatio /= 1.1;
	}
	update();
}

void fourierImage::resizeEvent(QResizeEvent*)
{
	suspendBtn->setGeometry(this->width() - 100, (int)(this->height() * 0.3), 100, 30);
	followBtn->setGeometry(this->width() - 100, (int)(this->height() * 0.3) - 40, 100, 30);
	homeBtn->setGeometry(this->width() - 100, (int)(this->height() * 0.3) - -80, 100, 30);
}

void fourierImage::calculate()
{
	delete path;
	path = new QPainterPath;

	path->moveTo(0, 0);

	complex<double> D1 = { 0,0 };
	for (int i = 0; i < coefficients_I->size(); i++)
	{
		double D = (i - fineness) * imageProgress * 2 * PI;
		complex<double> D2 = { cos(D), sin(D) };
		D2 *= coefficients_I->at(i);
		D1 += D2;
		path->lineTo(D1.real() * 1000, D1.imag() * 1000);
	}

	if (imageProgress == 0)
	{
		PATH->moveTo(D1.real() * 1000, D1.imag() * 1000);
	}
	else if (imageProgress <= 1)
	{
		bool draw = true;
		for (int i = 0; i < hiddenPaths->size(); i++)
		{
			if (imageProgress * totalLength >= hiddenPaths->at(i).real() && imageProgress * totalLength <= hiddenPaths->at(i).imag())
			{
				draw = false;

				if (fabs(imageProgress * totalLength - hiddenPaths->at(i).imag()) <= 0.0005 * totalLength)
				{
					PATH->moveTo(D1.real() * 1000, D1.imag() * 1000);
				}
				break;
			}
		}

		if (draw)
		{
			PATH->lineTo(D1.real() * 1000, D1.imag() * 1000);
		}
	}

	if (following)
	{
		widgetCenter = D1;
	}

	imageProgress += 0.0005;

	update();
}