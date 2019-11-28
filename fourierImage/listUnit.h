#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <complex>
#include <vector>

using std::complex;
using std::vector;

class listUnit : public QWidget
{
	Q_OBJECT

public:
	listUnit(int, QString, QListWidgetItem*, QWidget* parent = 0);
	listUnit(int, complex<double>, QListWidgetItem*, QWidget* parent = 0);

	~listUnit();
	QListWidgetItem* item();
	void setText(QString);
	void setData(complex<double>);

	//传入坐标系的坐标，函数会自己减去圆心坐标
	void setControlData(complex<double>);

	void insertCirclePoint();
	void delCirlcePoint();

	//setR和setRad都已经在setControlData里了,只有添加新点的时候需要设定一下

	//mode = 0时返回弧度制，1时返回角度制
	double rad(int mode = 0);
	//对于circlePoint来说只需要设置radian弧度，其他参数看圆心的。输入{cos,sin}，结合反三角函数得弧度值
	void setRad(complex<double>);
	double r();
	void setR(double);
	QString text();

	int circlePointNum();

	//返回圆心坐标
	complex<double> data();

	//返回控制点坐标
	complex<double> controlPointData();

	//返回控制点相对圆心的坐标
	complex<double> circleControl();

	//主动触发enterEvent
	void enter();

	//刷新storage的内容
	void refresh(int);

	//返回圆心painter坐标
	QPoint coord();

	//返回控制点的painter坐标
	QPoint controlPointCoord();

	//返回圆的外接矩形
	QRect rect();

	int pointMode, lineMode;
	bool onControl;

	enum pointModes { pathPoint, controlPoint, circleCenter, circlePoint };
	enum lineMode { show, hide };

protected:
	void enterEvent(QEvent*);
	void leaveEvent(QEvent*);

private:
	QListWidgetItem* itemPtr;
	QPushButton* delBtn, * insertBtn, * changeBtn;
	QLabel* storage;
	complex<double> pointData;
	int number;

	int circlePoints;//圆上点的数量（始终重叠的自身控制点和最后一个点算一个）
	double radius, radian;// radius是半径,radian是弧度
	complex<double> controlData;//控制点相对圆心的坐标

signals:
	void delU(listUnit*);
	void insertU(listUnit*);
	void changeU(listUnit*);
	void enterO(int);
};
