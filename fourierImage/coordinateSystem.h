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
    //ʵ�� 0~n����Ӧ��ŵ㸽��/(shiftPressed)֮��������ϣ�-1���հ�����        
    //�鲿 0���ɿ���1������ 2�������϶�����ϵ 3�� �����϶�·���� 4������shift�ɿ� 5�� ����shift����
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
