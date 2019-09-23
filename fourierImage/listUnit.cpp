#include "listUnit.h"
#include <QHBoxLayout>

listUnit::listUnit(int I, QString S, QListWidgetItem* LWI, QWidget* parent)
    : QWidget(parent)
{
    this->itemPtr = LWI;
    this->setFixedSize(255, 30);
    this->setMouseTracking(true);
    this->pointData = { S.left(S.indexOf(",")).toDouble(), S.right(S.length() - S.indexOf(',') - 1).toDouble() };
    this->number = I;

    storage = new QLabel(this);
    storage->setGeometry(0, 0, this->width() - 3 * this->height(), this->height());
    storage->setText(QString::number(number) + " (" + S + ")");

    delBtn = new QPushButton(this);
    delBtn->setGeometry(this->width() - 2.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
    delBtn->setText(tr("D"));
    delBtn->hide();

    insertBtn = new QPushButton(this);
    insertBtn->setGeometry(this->width() - 1.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
    insertBtn->setText(tr("I"));
    insertBtn->hide();

    changeBtn = new QPushButton(this);
    changeBtn->setGeometry(this->width() - 0.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
    changeBtn->setText(tr("C"));
    changeBtn->hide();

    connect(delBtn, &QPushButton::clicked, [&] {emit delU(this); });
    connect(insertBtn, &QPushButton::clicked, [&] {emit insertU(this); });
    connect(changeBtn, &QPushButton::clicked, [&] {emit changeU(this); });
}

listUnit::listUnit(int I, complex<double> C, QListWidgetItem* LWI, QWidget* parent)
    : QWidget(parent)
{
    QString S = QString::number(C.real()) + "," + QString::number(C.imag());

    this->itemPtr = LWI;
    this->setFixedSize(255, 30);
    this->setMouseTracking(true);
    this->pointData = { S.left(S.indexOf(",")).toDouble(), S.right(S.length() - S.indexOf(',') - 1).toDouble() };
    this->number = I;

    storage = new QLabel(this);
    storage->setGeometry(0, 0, this->width() - 3 * this->height(), this->height());
    storage->setText(QString::number(number) + " (" + S + ")");

    delBtn = new QPushButton(this);
    delBtn->setGeometry(this->width() - 2.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
    delBtn->setText(tr("D"));
    delBtn->hide();

    insertBtn = new QPushButton(this);
    insertBtn->setGeometry(this->width() - 1.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
    insertBtn->setText(tr("I"));
    insertBtn->hide();

    changeBtn = new QPushButton(this);
    changeBtn->setGeometry(this->width() - 0.9 * this->height(), 0.1 * this->height(), 0.8 * this->height(), 0.8 * this->height());
    changeBtn->setText(tr("C"));
    changeBtn->hide();

    connect(delBtn, &QPushButton::clicked, [&] {emit delU(this); });
    connect(insertBtn, &QPushButton::clicked, [&] {emit insertU(this); });
    connect(changeBtn, &QPushButton::clicked, [&] {emit changeU(this); });
}

listUnit::~listUnit()
{
}

QListWidgetItem* listUnit::item()
{
    return itemPtr;
}

void listUnit::setText(QString S)
{
    this->storage->setText(QString::number(number) + " (" + S + ")");
    this->pointData = { S.left(S.indexOf(",")).toDouble(), S.right(S.length() - S.indexOf(',') - 1).toDouble() };
}

void listUnit::setData(complex<double> C)
{
    this->storage->setText(QString::number(number) + " (" + QString::number(C.real()) + "," + QString::number(C.imag()) + ")");
    this->pointData = C;
}

QString listUnit::text()
{
    return storage->text();
}

complex<double> listUnit::data()
{
    return pointData;
}

void listUnit::enter()
{
    this->enterEvent(0);
}

void listUnit::setNumber(int I)
{
    this->number = I;
    this->storage->setText(QString::number(number) + " (" + QString::number(pointData.real()) + "," + QString::number(pointData.imag()) + ")");
}

void listUnit::enterEvent(QEvent*)
{
    delBtn->show();
    insertBtn->show();
    changeBtn->show();

    emit enterO(number);
}

void listUnit::leaveEvent(QEvent*)
{
    delBtn->hide();
    insertBtn->hide();
    changeBtn->hide();

    emit enterO(-1);
}
