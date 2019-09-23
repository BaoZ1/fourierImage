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
    int dotNum, * selectedListPoint, pointSize;
    complex<int> mouseCoord_widget, startingPoint, mouseMode;
    //实部 0~n：对应序号点附近/(shiftPressed)之后的连线上；-1：空白区域        
    //虚部 0：松开；1：按下 2：正在拖动坐标系 3： 正在拖动路径点 4：按下shift松开 5： 按下shift按下
    double scaleRatio;
    complex<double> widgetCenter, mouseCoord_paint;
    QPushButton* homeBtn;
    QLabel* cursorCoord;

signals:
    void addPoint(complex<double>);
    void insertPoint(complex<double>, int);
    void coordUpDate(complex<double>);
    void mouseLeaved();
};
