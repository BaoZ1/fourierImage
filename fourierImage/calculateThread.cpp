#include "calculateThread.h"


calculateThread::calculateThread(double accuracy, int num, vector<double>(*length), int fineness, int I1, int I2, vector<complex<double> >(*points), vector<complex<double> >(*results), QWidget* parent)
    : QThread()
{
    //this->part = 1 / (double)points->size();
    this->accuracy = 1.0 / accuracy;
    this->datas = points;
    this->results = results;
    this->begin = I1;
    this->end = I2;
    this->serialNumber = num;
    this->keyNum = fineness;
    this->totalLength = length;
}

calculateThread::~calculateThread()
{
}

void calculateThread::run()
{
    for (int j = begin; j < end; j++)
    {
        complex<double> integral = { 0, 0 };

        for (double m = 0; m < totalLength->at(totalLength->size() - 1) - accuracy; m += accuracy)
        {
            for (int n = 0; n < datas->size(); n++)
            {
                if (m < totalLength->at(n + 1) && m >= totalLength->at(n))
                {
                    complex<double> func = datas->at(n) + (datas->at((n + 1) % datas->size()) - datas->at(n)) * ((m - totalLength->at(n)) / (totalLength->at(n + 1) - totalLength->at(n)));
                    complex<double> EXfunc{ cos(-(j - keyNum) * m * 2 * PI / totalLength->at(totalLength->size() - 1)), sin(-(j - keyNum) * m * 2 * PI / totalLength->at(totalLength->size() - 1)) };
                    complex<double> DI = func * EXfunc * accuracy / totalLength->at(totalLength->size() - 1);
                    integral = integral + DI;
                }
            }
        }
        results->at(j) = integral;
        emit sendProgress(serialNumber, integral);
    }
}
