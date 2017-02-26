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
    if(srcImg!=NULL)
    {
        delete srcImg;
        srcImg=NULL;
    }
    if(editImg!=NULL)
    {
        delete editImg;
        editImg=NULL;
    }
}

void weightWidget::weightResize(int cols, int rows)
{
    weightWidth=cols;
    weightHeight=rows;
    if(srcImg!=NULL)
    {
        delete srcImg;
        srcImg=NULL;
    }
    if(editImg!=NULL)
    {
        delete editImg;
        editImg = NULL;
    }
    srcImg = new QImage(weightWidth,weightHeight,QImage::Format_Grayscale8);
    srcImg->fill(Qt::black);
    editImg = new QImage(weightWidth,weightHeight,QImage::Format_RGB32);
    editImg->fill(QColor(mainColor.secondary));
    pixelTotal = weightWidth*weightHeight;
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
    update();
}

void weightWidget::weightReflash(EzCamH3AWeight cfg)
{
    weightGenerate(cfg.width1,cfg.height1,cfg.h_start2,cfg.v_start2,cfg.width2,cfg.height2,cfg.weight,srcImg->bits());
    //*editImg=srcImg->convertToFormat(QImage::Format_RGB32);
    int cols,rows;
    //    unsigned int threshold = QColor(weightPrimary,weightPrimary,weightPrimary).rgb();
    for(rows=0;rows<weightHeight;rows++)
    {
        for(cols=0;cols<weightWidth;cols++)
        {
            *((unsigned int*)editImg->bits()+rows*weightWidth+cols)=*(srcImg->bits()+rows*weightWidth+cols)==weightPrimary?mainColor.primary:mainColor.secondary;
        }
    }
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
    pixelPrimary = total_pixels_2;
    curWeight = weight;

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
    weightPrimary = pixel_weight2;
    weightSecondary = pixel_weight1;
    //    qDebug()<<"center brightness:"<<pixel_weight2<<"other brightness:"<<pixel_weight1;
}

void weightWidget::weightGenerate(const unsigned char *src, unsigned char *dst)
{
    int total_pixels_1, total_pixels_2;
    int pixel_weight1, pixel_weight2;
    int max_weight,shift,cols,rows;
    int weight = curWeight;
    total_pixels_2 = pixelPrimary;
    total_pixels_1 = pixelTotal - total_pixels_2;

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

    weightPrimary = pixel_weight2;
    weightSecondary = pixel_weight1;

    for(rows=0;rows<weightHeight;rows++)
    {
        for(cols=0;cols<weightWidth;cols++)
        {
            *(dst+rows*weightWidth+cols)=*((unsigned int*)src+rows*weightWidth+cols)==mainColor.primary?weightPrimary:weightSecondary;
        }
    }
}

void weightWidget::changeMode(PAINT_MODE mode)
{
    curMode = mode;
    switch(mode)
    {
    case MODE_DEFAULT:
        setCursor(Qt::ArrowCursor);
        Editable=false;
        update();
        break;
    case MODE_CUSTOM:
        setCursor(Qt::CrossCursor);
        Editable=true;
        update();
        break;
    }
}

void weightWidget::flip(QPoint pos)
{
    if(editImg->pixel(pos)==mainColor.primary)
    {
        editImg->setPixelColor(pos,QColor(mainColor.secondary));
        pixelPrimary--;
    }
    else
    {
        editImg->setPixelColor(pos,QColor(mainColor.primary));
        pixelPrimary++;
    }
    update();
}

void weightWidget::resizeEvent(QResizeEvent *event)
{
    //    QWidget::resizeEvent(event);
    //    widgetWidth=event->size().width();
    //    widgetHeight=event->size().height();
    HRatio=(qreal)event->size().width()/(qreal)weightWidth;
    VRatio=(qreal)event->size().height()/(qreal)weightHeight;
}

unsigned char weightWidget::weightAt(QPoint pos)
{
    if(srcImg==NULL||pos.x()>weightWidth||pos.y()>weightHeight)
        return 0;

    return *(srcImg->bits()+pos.x()+pos.y()*weightWidth);
}

void weightWidget::paintEvent(QPaintEvent *event)
{
    if(srcImg==NULL)
        return;
    QPainter painter(this);
    painter.scale(HRatio,VRatio);
    if(Editable)
        painter.drawImage(0,0,*editImg);
    else
        painter.drawImage(0,0,*srcImg);

}

void weightWidget::mousePressEvent(QMouseEvent *event)
{
    if(event->button()==Qt::RightButton&&currentMode()!=MODE_DEFAULT)
    {
        weightGenerate(editImg->bits(),srcImg->bits());
        changeMode(MODE_DEFAULT);
    }
    else
    {
    qreal orignX = event->pos().x()/HRatio;
    qreal orignY = event->pos().y()/VRatio;
    QPoint orign(qFloor(orignX),qFloor(orignY));
    emit curPosInfo(orign,weightAt(orign));
    if(Editable)
        flip(orign);
    }
}

