#pragma once

#include <QWidget>
#include <vector>
#include <complex>
#include <QWheelEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QVector>
#include <QPushButton>
#include "calculateThread.h"

using std::vector;
using std::complex;

class fourierImage : public QWidget
{
	Q_OBJECT

public:
	fourierImage(bool, int, int, int, vector<complex<double> >(*), int, double, vector<complex<double> >(*), QWidget* parent = Q_NULLPTR);
	void paintEvent(QPaintEvent*);

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void resizeEvent(QResizeEvent*);

private:
	QTimer* timer;
	double imageProgress, scaleRatio, totalLength;
	QPainterPath* PATH, * path;
	vector<complex<double> >* coefficients_I, * hiddenPaths;
	complex<int> mouseCoord_widget, startingPoint;
	complex<double> widgetCenter;
	bool pressed, suspended, following, antialiasing;
	int fineness, settingWidth, settingHeight;
	QPushButton* suspendBtn, * followBtn, * homeBtn;

	void calculate();

signals:
	void sendProgress(int, complex<double>);
};
