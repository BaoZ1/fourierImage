#pragma once

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <complex>
#include "calculateThread.h"
#include <vector>

using std::complex;
using std::vector;

class calculateProgress : public QWidget
{
	Q_OBJECT

public:
	calculateProgress(int, vector<complex<double> >(*), QWidget* parent = 0);
	~calculateProgress();

private:
	QLabel* (msg)[5], * finishL;
	QProgressBar* bar;
	int progress;
	QPushButton* startBtn;
	vector<calculateThread*>threads;
	vector<complex<double> > points, * results;
	vector<double>* length;
	int fineness, threadNum, threadPart, threadRest;
	double accuracy;

	void calculate();
	void finish();

private slots:
	void getData(int, complex<double>);
	void addPoint(complex<double>);
	void addOptions_calculate(double, int, int, vector<double>*);

signals:
	void start();
};
