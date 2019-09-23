#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QCheckBox>
#include <vector>
#include <complex>
#include "listUnit.h"
#include "coordinateWidget.h"
#include "calculateProgress.h"
#include "fourierImage.h"

#define pointsLimit 500

using std::vector;
using std::complex;

class options : public QWidget
{
    Q_OBJECT

public:
    options(QWidget* parent = 0);
    ~options();

private:
    QLineEdit* point, * sizeO, * finenessO, * threadNumO, * calculateAccuracyO;
    QLabel* pointL, * sizeOL, * finenessOL, * pointListL, * record, * threadNumOL, * calculateAccuracyOL;
    QPushButton* addBtn, * graphBtn, * finishBtn, * listSwitch;
    QCheckBox* antialiasingCB;
    QListWidget* pointList, * pointList_dots;
    int pointNum;
    vector<listUnit*>units;
    coordinateWidget* dotsGraph;
    QRegExp pointFormat;
    calculateProgress* progressBar;
    fourierImage* image;
    vector<complex<double> >coefficients;
    QWidget* dotsWidget;
    vector<double> linesLength;

    void listUpDate();

private slots:
    void add();
    void del(listUnit*);
    void insert(listUnit*);
    void change(listUnit*);
    void addFromCoord(complex<double>);
    void insertFromCoord(complex<double>, int);

signals:
    void inputPoint(complex<double>);
    void inputOptions_image(int, int);
    void inputOptions_calculate(double, int, int, vector<double>*);
};
