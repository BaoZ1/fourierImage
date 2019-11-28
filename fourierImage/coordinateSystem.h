#pragma once

#include <QWidget>
#include <vector>
#include "listUnit.h"
#include <QWheelEvent>
#include <QResizeEvent>
#include <complex>

using std::vector;
using std::complex;

class coordinateSystem : public QWidget
{
	Q_OBJECT

public:
	coordinateSystem(vector<listUnit*>(*), QWidget* parent = 0);
	~coordinateSystem();
	void paintEvent(QPaintEvent*);

protected:
	void mousePressEvent(QMouseEvent*);
	void mouseReleaseEvent(QMouseEvent*);
	void mouseMoveEvent(QMouseEvent*);
	void wheelEvent(QWheelEvent*);
	void leaveEvent(QEvent*);
	void resizeEvent(QResizeEvent*);
	void keyPressEvent(QKeyEvent*);
	void keyReleaseEvent(QKeyEvent*);

private:
	vector<listUnit*>* points;
	int dotNum, pointSize;
	complex<int> mouseCoord_widget, startingPoint, mouseMode;
	enum modes { released, pressed };
	double scaleRatio;
	complex<double> widgetCenter, widgetCenterOld, mouseCoord_paint;
	QPushButton* homeBtn;
	bool shiftPressed, C_keyPressed;

signals:
	void addPoint(complex<double>);
	void insertPoint(complex<double>, int);
	void delPoint(int);
	void coordUpDate(complex<double>);
	void mouseLeaved();
};
