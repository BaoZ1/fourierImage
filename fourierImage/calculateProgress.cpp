#include "calculateProgress.h"

calculateProgress::calculateProgress(int max, vector<complex<double> >(*V), QWidget* parent)
    : QWidget(parent)
{
    progress = 0;

    this->setFixedSize(400, 250);
    this->setWindowTitle(tr("Calculating..."));
    this->results = V;
    for (int i = 0; i < 5; i++)
    {
        msg[i] = new QLabel(this);
        msg[i]->setGeometry(30, 120 - 30 * i, 370, 30);
        msg[i]->setText("...");
    }
    bar = new QProgressBar(this);
    bar->setGeometry(30, 150, 340, 50);
    bar->setRange(0, max);
    bar->setValue(0);

    this->setAttribute(Qt::WA_DeleteOnClose);
}

calculateProgress::~calculateProgress()
{
    for (int i = 0; i < threadNum; i++)
    {
        threads[i]->exit();
    }
}

void calculateProgress::calculate()
{
    int mark = 0;
    for (int i = 0; i < threadNum; i++)
    {
        threads[i] = new calculateThread(accuracy, i + 1, length, fineness, mark, mark + threadPart + ((i < threadRest) ? 1 : 0), &points, results);
        connect(threads[i], SIGNAL(sendProgress(int, complex<double>)), this, SLOT(getData(int, complex<double>)), Qt::BlockingQueuedConnection);
        threads[i]->start();
        mark += threadPart + ((i < threadRest) ? 1 : 0);
    }
}

void calculateProgress::finish()
{
    finishL = new QLabel(this);
    finishL->setText(tr("Finished!"));
    finishL->setGeometry(30, 200, 100, 50);
    finishL->show();

    startBtn = new QPushButton(this);
    startBtn->setText(tr("Start now!"));
    startBtn->setGeometry(250, 210, 120, 30);
    connect(startBtn, &QPushButton::clicked, [&] {emit start(); });
    startBtn->show();
}

void calculateProgress::addPoint(complex<double> C)
{
    this->points.push_back(C);
}

void calculateProgress::addOptions_calculate(double D, int I1, int I2, vector<double>(*V))
{
    this->fineness = I1;
    this->threadNum = I2;
    this->accuracy = D;
    this->length = V;

    this->threadPart = (2 * fineness + 1) / threadNum;
    this->threadRest = (2 * fineness + 1) % threadNum;
    this->threads.resize(threadNum);


    calculate();
}

void calculateProgress::getData(int I, complex<double> C)
{
    progress++;
    for (int i = 4; i > 0; i--)
    {
        msg[i]->setText(msg[i - 1]->text());
    }
    msg[0]->setText(tr("Thread") + QString::number(I) + QString(": ") + tr("result") + QString(" -> ") + QString::number(C.real()) + QString((C.imag() > 0) ? "+" : "") + QString::number(C.imag()) + QString("i"));
    bar->setValue(progress);
    if (progress == bar->maximum())
    {
        finish();
    }
}