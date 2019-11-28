#pragma once

#include <QWidget>
#include <QListWidget>
#include "coordinateSystem.h"
#include <QCloseEvent>
#include <complex>

using std::complex;

class coordinateWidget : public QWidget
{
	Q_OBJECT

public:
	coordinateWidget(QWidget*, QListWidget*, QLabel*, vector<listUnit*>(*), QWidget* parent = 0);
	~coordinateWidget();

protected:
	void closeEvent(QCloseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void paintEvent(QPaintEvent*);

private:
	QListWidget* list;
	coordinateSystem* ground;
	QWidget* OP;
	QLabel* coord, * pointNum, * transparencyL;
	QSlider* transparencyS;
	QPoint windowPos, beginPoint, endPoint;
	QPushButton* closeBtn;
	int windowWidth, windowHeight, mode;

private slots:
	void add(complex<double>);
	void insert(complex<double>, int);
	void del(int);
	void labelChange(complex<double> D);

signals:
	void addPoint(complex<double>);
	void insertPoint(complex<double>, int);
	void delPoint(int);
	void closing();
};
