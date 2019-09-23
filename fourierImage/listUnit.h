#pragma once

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QListWidgetItem>
#include <complex>

using std::complex;

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
    QString text();
    complex<double> data();
    void enter();
    void setNumber(int);

protected:
    void enterEvent(QEvent*);
    void leaveEvent(QEvent*);

private:
    QListWidgetItem* itemPtr;
    QPushButton* delBtn, * insertBtn, * changeBtn;
    QLabel* storage;
    complex<double> pointData;
    int number;

signals:
    void delU(listUnit*);
    void insertU(listUnit*);
    void changeU(listUnit*);
    void enterO(int);
};
