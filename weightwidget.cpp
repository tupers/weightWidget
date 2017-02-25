#include "weightwidget.h"

weightWidget::weightWidget(QWidget *parent) : QWidget(parent)
{
    setStyleSheet("background-color:white;");
}

weightWidget::~weightWidget()
{
    //    if(!rectList.isEmpty())
    //    {
    //        rectList.clear();
    //        rectList.squeeze();
    //    }
    if(data!=NULL)
    {
        delete data;
        data=NULL;
    }
    if(img!=NULL)
    {
        delete img;
        img=NULL;
    }
}

void weightWidget::weightResize(int cols, int rows)
{
    weightWidth=cols;
    weightHeight=rows;
    //    if(!rectList.isEmpty())
    //        rectList.clear();
    //    resize(weightWidth,weightHeight);
    if(data!=NULL)
    {
        delete data;
        data=NULL;
    }
    if(img!=NULL)
    {
        delete img;
        img=NULL;
    }
    //    int i,j;
    //    for(i=0;i<rows;i++)
    //    {
    //        for(j=0;j<cols;j++)
    //        {
    //            //            QRect* rect = new QRect(j*2,x*2,2,2);
    //            QRectF rect = QRectF(j,i,1,1);
    //            //rect.setSize(QSize());
    //            rectList.append(rect);
    //        }
    //    }
    data = new unsigned char[weightWidth*weightHeight];
    memset(data,0,weightWidth*weightHeight*sizeof(unsigned char));
    update();
}

void weightWidget::weightReflash(EzCamH3AWeight cfg)
{
    weightGenerate(cfg.width1,cfg.height1,cfg.h_start2,cfg.v_start2,cfg.width2,cfg.height2,cfg.weight,data);
    //    FILE* fp;
    //    fp=fopen("./weight","wb");
    //    fwrite(data,1,weightSize(),fp);
    //    fclose(fp);
    if(img!=NULL)
    {
        delete img;
        img=NULL;
    }
    img = new QImage(data,weightWidth,weightHeight,QImage::Format_Grayscale8);
    update();
}

void weightWidget::weightGenerate(int width1, int height1, int h_start2, int v_start2, int width2, int height2, int weight, unsigned char *win_coeffs)
{
    int total_pixels_1, total_pixels_2;
    int pixel_weight1, pixel_weight2;
    int max_weight;
    int shift;
    int i, j;

    /* calculate weight for each window */
    total_pixels_2 = width2 * height2;
    total_pixels_1 = width1 * height1 - total_pixels_2;

    pixel_weight1 = (100 - weight) * 65536 / total_pixels_1;
    pixel_weight2 = weight * 65536 / total_pixels_2;

    /* normalize weights */
    max_weight = pixel_weight1 > pixel_weight2 ? pixel_weight1 : pixel_weight2;
    shift = 0;
    while(max_weight >= 255){
        shift ++;
        max_weight = (max_weight + 1) >> 1;
    }
    pixel_weight1 = (pixel_weight1 + (1<<(shift-1))) >> shift;
    pixel_weight2 = (pixel_weight2 + (1<<(shift-1))) >> shift;

    if(pixel_weight2 > pixel_weight1 ){
        pixel_weight2 = (pixel_weight1 * total_pixels_1  * weight) /
                (total_pixels_2 * (100 - weight));
    } else {
        pixel_weight1 = (pixel_weight2 * total_pixels_2  * (100 - weight)) /
                (total_pixels_1 * weight);
    }


    for(i = 0; i < height1; i ++){
        for(j = 0; j < width1; j ++) {
            if(i >= v_start2 && i < (v_start2 + height2)
                    && j >= h_start2 && j < (h_start2 + width2)){
                win_coeffs[i * width1 + j] = pixel_weight2;
            }else {
                win_coeffs[i * width1 + j] = pixel_weight1;
            }
        }
    }
    qDebug()<<"center brightness:"<<pixel_weight2<<"other brightness:"<<pixel_weight1;
}

void weightWidget::resizeEvent(QResizeEvent *event)
{
    //    QWidget::resizeEvent(event);
    //    qDebug()<<event->size();
    widgetWidth=event->size().width();
    widgetHeight=event->size().height();
}

void weightWidget::paintEvent(QPaintEvent *event)
{
    //    qDebug()<<"paint";
    if(img==NULL)
        return;
    QPainter painter(this);
    //    painter.setPen(Qt::NoPen);
    painter.scale(widgetWidth/weightWidth,widgetHeight/weightHeight);
    painter.drawImage(0,0,*img);
    //    int i;
    //    painter.scale(widgetWidth/weightWidth,widgetHeight/weightHeight);
    //    for(i=0;i<weightWidth*weightHeight;i++)
    //    {
    //        painter.setBrush(QBrush(QColor(data[i],data[i],data[i])));
    //        painter.drawRects(rectList);
    //    }
}

