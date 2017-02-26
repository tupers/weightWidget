#ifndef WEIGHTWIDGET_H
#define WEIGHTWIDGET_H

#include <QObject>
#include <QWidget>
#include <QBoxLayout>
#include <QVector>
#include <QPushButton>
#include <QRect>
#include <QPainter>
#include <qDebug>
#include <QResizeEvent>
#include <QtMath>
#include <QRgb>
#include "ezstream.h"

enum PAINT_MODE
{
    MODE_DEFAULT=0,
    MODE_CUSTOM
};

typedef struct
{
    QRgb primary;
    QRgb secondary;
}colorGroup;

class weightWidget : public QWidget
{
    Q_OBJECT
public:
    explicit weightWidget(QWidget *parent = 0);
    ~weightWidget();
    void weightResize(int cols,int rows);
    int weightSize(){return weightWidth*weightHeight;}
    bool weightReflash(EzCamH3AWeight cfg);
    void weightGenerate(int width1,int height1,int h_start2,int v_start2,int width2,int height2,int weight,unsigned char* win_coeffs);//generate according EzCamH3AWeight
    void weightGenerate();//generate according optional setting
    void changeMode(PAINT_MODE);
    void editRecover(){*editImg=*editImg_bak;}
    void setWeight(int wt){curWeight = wt;}//set weightMap weight
    PAINT_MODE currentMode(){return curMode;}

signals:
    void curPosInfo(QPoint,unsigned char);
public slots:
private:
    virtual void resizeEvent(QResizeEvent *event);

    void flip(QPoint);
    void format(QPoint,QRgb);
    unsigned char weightAt(QPoint);

    int weightWidth=1;
    int weightHeight=1;
    //    int widgetWidth=0;
    //    int widgetHeight=0;
    int weightPrimary = 0;
    int weightSecondary = 0;
    int pixelPrimary=0;
    int pixelTotal=0;
    int curWeight=50;//weightMap weight
    bool Editable = false;
    PAINT_MODE curMode=MODE_DEFAULT;
    QImage* srcImg=NULL;//weight map
    QImage* editImg=NULL;
    QImage* editImg_bak=NULL;
    colorGroup mainColor={0xFF795548,0xFFBDBDBD};

    qreal HRatio=1;
    qreal VRatio=1;
    QPoint lastMovePos;//covert into orign img position
    QPoint lastClickPos;//covert into orign img position

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent*event);
    void mouseMoveEvent(QMouseEvent*event);

};

#endif // WEIGHTWIDGET_H
