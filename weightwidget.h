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
    void weightReflash(EzCamH3AWeight cfg);
    void weightGenerate(int width1,int height1,int h_start2,int v_start2,int width2,int height2,int weight,unsigned char* win_coeffs);
    void weightGenerate(const unsigned char* src,unsigned char* dst);
    void changeMode(PAINT_MODE);
    PAINT_MODE currentMode(){return curMode;}
    void flip(QPoint);

signals:
    void curPosInfo(QPoint,unsigned char);
public slots:
private:
    int weightWidth=1;
    int weightHeight=1;
//    int widgetWidth=0;
//    int widgetHeight=0;
    int weightPrimary = 0;
    int weightSecondary = 0;
    int pixelPrimary=0;
    int pixelTotal=0;
    int curWeight=50;
    bool Editable = false;
    qreal HRatio=1;
    qreal VRatio=1;
    virtual void resizeEvent(QResizeEvent *event);
    QImage* srcImg=NULL;
    QImage* editImg=NULL;
    PAINT_MODE curMode=MODE_DEFAULT;
    unsigned char weightAt(QPoint);
    colorGroup mainColor={0xFF795548,0xFFBDBDBD};
protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent*event);

};

#endif // WEIGHTWIDGET_H
