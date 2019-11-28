#include "coordinateWidget.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>

coordinateWidget::coordinateWidget(QWidget* O, QListWidget* L, QLabel* PL, vector<listUnit*>(*V), QWidget* parent)
	: QWidget(parent)
{
	windowPos = QPoint(500, 200);
	windowWidth = 910;
	windowHeight = 615;
	mode = 0;

	this->setGeometry(windowPos.x(), windowPos.y(), windowWidth, windowHeight);

	setAutoFillBackground(false);
	setWindowFlags(Qt::FramelessWindowHint);
	setAttribute(Qt::WA_TranslucentBackground, true);
	this->setMouseTracking(true);

	coord = new QLabel;
	coord->setFixedHeight(30);

	ground = new coordinateSystem(V);
	ground->grabKeyboard();

	closeBtn = new QPushButton;
	closeBtn->setFixedSize(25, 25);
	closeBtn->setText("X");
	connect(closeBtn, &QPushButton::clicked, [&] {ground->releaseKeyboard(); this->close(); });

	pointNum = PL;
	pointNum->setFixedHeight(30);

	list = L;
	list->setFixedWidth(290);

	transparencyL = new QLabel;
	transparencyL->setText(tr("transparency"));
	transparencyL->setFixedWidth(130);

	transparencyS = new QSlider;
	transparencyS->setOrientation(Qt::Horizontal);
	transparencyS->setFixedSize(150, 30);
	transparencyS->setValue(255);
	transparencyS->setMaximum(255);
	transparencyS->setMinimum(0);
	connect(transparencyS, &QSlider::valueChanged, [&]
		{
			repaint();
		}
	);

	QVBoxLayout* windowLayout = new QVBoxLayout;
	QHBoxLayout* titleLayout = new QHBoxLayout;
	QHBoxLayout* mainLayout = new QHBoxLayout;
	QVBoxLayout* rightLayout = new QVBoxLayout;
	QVBoxLayout* leftLayout = new QVBoxLayout;
	QHBoxLayout* transparencyLY = new QHBoxLayout;

	titleLayout->addStretch();
	titleLayout->addWidget(closeBtn);
	transparencyLY->addWidget(transparencyL);
	transparencyLY->addWidget(transparencyS);
	rightLayout->addWidget(pointNum);
	rightLayout->addWidget(list);
	rightLayout->addLayout(transparencyLY);
	rightLayout->addWidget(coord);
	leftLayout->addWidget(ground);
	mainLayout->addLayout(leftLayout);
	mainLayout->addLayout(rightLayout);
	windowLayout->addLayout(titleLayout);
	windowLayout->addLayout(mainLayout);
	this->setLayout(windowLayout);

	OP = O;

	connect(ground, SIGNAL(addPoint(complex<double>)), this, SLOT(add(complex<double>)));
	connect(ground, SIGNAL(insertPoint(complex<double>, int)), this, SLOT(insert(complex<double>, int)));
	connect(ground, SIGNAL(delPoint(int)), this, SLOT(del(int)));
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

void coordinateWidget::mouseMoveEvent(QMouseEvent* e)
{
	ground->repaint();

	switch (mode)
	{
	case 0:
		if (this->width() - e->pos().x() <= 10 && this->height() - e->pos().y() <= 10)
		{
			setCursor(Qt::SizeFDiagCursor);
		}
		else
		{
			setCursor(Qt::ArrowCursor);
		}
		break;

	case 1:
		endPoint = this->mapToGlobal(e->pos());

		this->resize(windowWidth + (endPoint.x() - beginPoint.x()), windowHeight + (endPoint.y() - beginPoint.y()));
		break;

	case 2:
		endPoint = this->mapToGlobal(e->pos());

		this->setGeometry(windowPos.x() + (endPoint.x() - beginPoint.x()), windowPos.y() + (endPoint.y() - beginPoint.y()), windowWidth, windowHeight);
		break;
	}
}

void coordinateWidget::mousePressEvent(QMouseEvent* e)
{
	beginPoint = this->mapToGlobal(e->pos());
	if (this->width() - e->pos().x() <= 10 && this->height() - e->pos().y() <= 10)
	{
		mode = 1;
	}
	else if (e->pos().y() <= 45)
	{
		mode = 2;
	}
}

void coordinateWidget::mouseReleaseEvent(QMouseEvent* e)
{
	if (mode == 1)
	{
		windowWidth += endPoint.x() - beginPoint.x();
		windowHeight += endPoint.y() - beginPoint.y();
	}
	else if (mode == 2)
	{
		windowPos.setX(windowPos.x() + (endPoint.x() - beginPoint.x()));
		windowPos.setY(windowPos.y() + (endPoint.y() - beginPoint.y()));
	}

	mode = 0;
}

void coordinateWidget::paintEvent(QPaintEvent*)
{
	QPainter p(this);
	p.setPen(Qt::NoPen);
	p.setBrush(QColor(200, 200, 200));

	QRegion r1(this->rect());
	QRegion r2(QRect(10, 45, this->width() - 320, this->height() - 55));
	QPainterPath pp;
	pp.addRegion(r1.subtracted(r2));

	p.drawPath(pp);
	p.setBrush(QColor(200, 200, 200, transparencyS->value()));
	p.drawRect(QRect(10, 10, this->width() - 320, this->height() - 20));
}

void coordinateWidget::insert(complex<double> C, int I)
{
	emit insertPoint(C, I);
}

void coordinateWidget::del(int I)
{
	emit delPoint(I);
}

void coordinateWidget::labelChange(complex<double> D)
{
	coord->setText("x:" + QString::number(D.real()) + " , y:" + QString::number(D.imag()));
}

void coordinateWidget::add(complex<double> C)
{
	emit addPoint(C);
}