#pragma once

#include <QThread>
#include <complex>
#include <vector>

#define PI 3.1415926535897932

using std::complex;
using std::vector;

class calculateThread : public QThread
{
    Q_OBJECT

public:
    calculateThread(double, int, vector<double>(*), int, int, int, vector<complex<double> >(*), vector<complex<double> >(*), QWidget* parent = 0);
    ~calculateThread();

protected:
    void run();

private:
    int begin, end, serialNumber, keyNum;
    double part, progress, accuracy;
    vector<complex<double> >* datas, * results, * coefficients, * path;
    vector<double>* totalLength;

signals:
    void sendProgress(int, complex<double>);
    void finished();
};
