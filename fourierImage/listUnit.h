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

	//��������ϵ�����꣬�������Լ���ȥԲ������
	void setControlData(complex<double>);

	void insertCirclePoint();
	void delCirlcePoint();

	//setR��setRad���Ѿ���setControlData����,ֻ������µ��ʱ����Ҫ�趨һ��

	//mode = 0ʱ���ػ����ƣ�1ʱ���ؽǶ���
	double rad(int mode = 0);
	//����circlePoint��˵ֻ��Ҫ����radian���ȣ�����������Բ�ĵġ�����{cos,sin}����Ϸ����Ǻ����û���ֵ
	void setRad(complex<double>);
	double r();
	void setR(double);
	QString text();

	int circlePointNum();

	//����Բ������
	complex<double> data();

	//���ؿ��Ƶ�����
	complex<double> controlPointData();

	//���ؿ��Ƶ����Բ�ĵ�����
	complex<double> circleControl();

	//��������enterEvent
	void enter();

	//ˢ��storage������
	void refresh(int);

	//����Բ��painter����
	QPoint coord();

	//���ؿ��Ƶ��painter����
	QPoint controlPointCoord();

	//����Բ����Ӿ���
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

	int circlePoints;//Բ�ϵ��������ʼ���ص���������Ƶ�����һ������һ����
	double radius, radian;// radius�ǰ뾶,radian�ǻ���
	complex<double> controlData;//���Ƶ����Բ�ĵ�����

signals:
	void delU(listUnit*);
	void insertU(listUnit*);
	void changeU(listUnit*);
	void enterO(int);
};
