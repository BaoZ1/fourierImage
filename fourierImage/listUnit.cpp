#pragma once

#include "listUnit.h"
#include <QHBoxLayout>

constexpr double Pi = 3.1415926535897932;

listUnit::listUnit(int I, QString S, QListWidgetItem* LWI, QWidget* parent)
	: QWidget(parent)
{
	this->itemPtr = LWI;
	this->setFixedSize(255, 30);
	this->setMouseTracking(true);
	this->pointData = { S.left(S.indexOf(",")).toDouble(), S.right(S.length() - S.indexOf(',') - 1).toDouble() };
	this->number = I;
	this->pointMode = pathPoint;
	this->lineMode = show;
	this->onControl = false;
	this->circlePoints = 0;

	storage = new QLabel(this);
	storage->setGeometry(0, 0, this->width(), this->height());
	storage->setText(QString::number(number) + " (" + S + ")");

	delBtn = new QPushButton(this);
	delBtn->setGeometry(this->width() - 2.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
	delBtn->setText(tr("D"));
	delBtn->hide();

	insertBtn = new QPushButton(this);
	insertBtn->setGeometry(this->width() - 1.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
	insertBtn->setText(tr("I"));
	insertBtn->hide();

	changeBtn = new QPushButton(this);
	changeBtn->setGeometry(this->width() - 0.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
	changeBtn->setText(tr("C"));
	changeBtn->hide();

	connect(delBtn, &QPushButton::clicked, [&] {emit delU(this); });
	connect(insertBtn, &QPushButton::clicked, [&] {emit insertU(this); });
	connect(changeBtn, &QPushButton::clicked, [&] {emit changeU(this); });
}

listUnit::listUnit(int I, complex<double> C, QListWidgetItem* LWI, QWidget* parent)
	: QWidget(parent)
{
	QString S = QString::number(C.real()) + "," + QString::number(C.imag());

	listUnit::listUnit(I, S, LWI);
}

listUnit::~listUnit()
{
}

QListWidgetItem* listUnit::item()
{
	return itemPtr;
}

void listUnit::setText(QString S)
{
	this->storage->setText(QString::number(number) + " (" + S + ")");
	this->pointData = { S.left(S.indexOf(",")).toDouble(), S.right(S.length() - S.indexOf(',') - 1).toDouble() };
}

void listUnit::setData(complex<double> C)
{
	this->storage->setText(QString::number(number) + " (" + QString::number(C.real()) + "," + QString::number(C.imag()) + ")");
	this->pointData = C;
}

void listUnit::setControlData(complex<double> C)
{
	controlData = C - pointData;

	radius = sqrt(pow(controlData.real(), 2) + pow(controlData.imag(), 2));

	complex<double> D = controlData / r();

	setRad(D);
}

void listUnit::insertCirclePoint()
{
	circlePoints++;
}

void listUnit::delCirlcePoint()
{
	circlePoints--;
}

double listUnit::rad(int I)
{
	if (I == 0)
	{
		return radian;
	}
	else if (I == 1)
	{
		return radian / (2 * Pi) * 360;
	}
}

void listUnit::setRad(complex<double> D)
{
	radian = D.imag() > 0 ? acos(D.real()) : 2 * Pi - acos(D.real());
}

double listUnit::r()
{
	return radius;
}

void listUnit::setR(double D)
{
	radius = D;
}

QString listUnit::text()
{
	return storage->text();
}

int listUnit::circlePointNum()
{
	return circlePoints;
}

complex<double> listUnit::data()
{
	return pointData;
}

complex<double> listUnit::controlPointData()
{
	return data() + circleControl();
}

complex<double> listUnit::circleControl()
{
	return controlData;
}

void listUnit::enter()
{
	this->enterEvent(0);
}

void listUnit::refresh(int I)
{
	this->number = I;
	QString extra;

	extra += QString::number(I);

	switch (pointMode)
	{
	case controlPoint:
		extra += " B ";
		break;

	case circleCenter:
		extra += " C ";
		break;

	case circlePoint:
		extra += " R ";
		break;

	case pathPoint:
		extra += " P ";
		break;
	}
	this->storage->setText(extra + " (" + QString::number(pointData.real()) + "," + QString::number(pointData.imag()) + ")");
}

QPoint listUnit::coord()
{
	QPoint c((int)(data().real() * 1000), (int)(data().imag() * 1000));
	return c;
}

QPoint listUnit::controlPointCoord()
{
	QPoint c((int)((data().real() + controlData.real()) * 1000), (int)((data().imag() + controlData.imag()) * 1000));

	return c;
}

QRect listUnit::rect()
{
	int R = (int)(radius * 1000);
	QPoint vertex = coord() - QPoint(R, R);

	return QRect(vertex, QSize(2 * R, 2 * R));
}

void listUnit::enterEvent(QEvent*)
{
	delBtn->show();
	insertBtn->show();
	changeBtn->show();

	emit enterO(number);
}

void listUnit::leaveEvent(QEvent*)
{
	delBtn->hide();
	insertBtn->hide();
	changeBtn->hide();

	emit enterO(-1);
}