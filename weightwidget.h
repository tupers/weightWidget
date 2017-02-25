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
#include "ezstream.h"

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

signals:

public slots:
private:
//    QVector<QRectF> rectList;
    int weightWidth=0;
    int weightHeight=0;
    int widgetWidth=0;
    int widgetHeight=0;
    virtual void resizeEvent(QResizeEvent *event);
    unsigned char* data;
    QImage* img=NULL;
protected:
    void paintEvent(QPaintEvent *event);

};

#endif // WEIGHTWIDGET_H
