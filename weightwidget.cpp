#include "weightwidget.h"

weightWidget::weightWidget(QWidget *parent) : QWidget(parent)
{
    //setStyleSheet("background-color:white;");
    weightResize(1,1);
}

weightWidget::~weightWidget()
{
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
    if(editImg_bak!=NULL)
    {
        delete editImg_bak;
        editImg_bak=NULL;
    }
}

void weightWidget::weightResize(int cols, int rows)
{
    //reinit settings
    weightWidth=cols;
    weightHeight=rows;
    weightPrimary = 0;
    weightSecondary = 0;
    pixelPrimary=0;
    pixelTotal = weightWidth*weightHeight;
    curWeight=50;

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
    if(editImg_bak!=NULL)
    {
        delete editImg_bak;
        editImg_bak=NULL;
    }
    srcImg = new QImage(weightWidth,weightHeight,QImage::Format_Grayscale8);
    srcImg->fill(Qt::black);
    editImg = new QImage(weightWidth,weightHeight,QImage::Format_RGB32);
    editImg->fill(QColor(mainColor.secondary));
    editImg_bak = new QImage(weightWidth,weightHeight,QImage::Format_RGB32);
    editImg_bak->fill(QColor(mainColor.secondary));

    changeMode(MODE_DEFAULT);//it run update() internally
}

bool weightWidget::weightReflash(EzCamH3AWeight cfg)
{
    if(cfg.height1!=weightHeight||cfg.width1!=weightWidth)
        return false;
    weightGenerate(cfg.width1,cfg.height1,cfg.h_start2,cfg.v_start2,cfg.width2,cfg.height2,cfg.weight,srcImg->bits());
    int cols,rows;
    for(rows=0;rows<weightHeight;rows++)
    {
        for(cols=0;cols<weightWidth;cols++)
        {
            *((unsigned int*)editImg->bits()+rows*weightWidth+cols)=*(srcImg->bits()+rows*weightWidth+cols)==weightPrimary?mainColor.primary:mainColor.secondary;
        }
    }
    *editImg_bak=*editImg;
    update();
    return true;
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

void weightWidget::weightGenerate()
{
    int total_pixels_1, total_pixels_2;
    int pixel_weight1, pixel_weight2;
    int max_weight,shift;
    int weight = curWeight;
    total_pixels_2 = pixelPrimary==0?1:pixelPrimary;
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

}

void weightWidget::changeMode(PAINT_MODE mode)
{
    curMode = mode;
    switch(mode)
    {
    case MODE_DEFAULT:
        setCursor(Qt::ArrowCursor);
        Editable=false;

        break;
    case MODE_CUSTOM:
        setCursor(Qt::CrossCursor);
        Editable=true;

        break;
    }
    update();
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

void weightWidget::format(QPoint pos, QRgb color)
{
    if(color==mainColor.primary&&editImg->pixel(pos)!=mainColor.primary)
        pixelPrimary++;
    else if(color==mainColor.secondary&&editImg->pixel(pos)!=mainColor.secondary)
        pixelPrimary--;
    editImg->setPixelColor(pos,QColor(color));
    update();
}

void weightWidget::resizeEvent(QResizeEvent *event)
{
    //    QWidget::resizeEvent(event);
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
        weightGenerate();
        int rows,cols;
        for(rows=0;rows<weightHeight;rows++)
        {
            for(cols=0;cols<weightWidth;cols++)
            {
                *(srcImg->bits()+rows*weightWidth+cols)=*((unsigned int*)editImg->bits()+rows*weightWidth+cols)==mainColor.primary?weightPrimary:weightSecondary;
            }
        }
        *editImg_bak=*editImg;
        changeMode(MODE_DEFAULT);
    }
    else if(event->button()==Qt::LeftButton)
    {
        qreal orignX = event->pos().x()/HRatio;
        qreal orignY = event->pos().y()/VRatio;
        QPoint orign(qFloor(orignX),qFloor(orignY));
        emit curPosInfo(orign,weightAt(orign));
        if(Editable)
        {
            lastClickPos=orign;
            flip(orign);
        }
    }
}

void weightWidget::mouseMoveEvent(QMouseEvent *event)
{
    if(event->buttons()&Qt::LeftButton&&Editable)
    {
        qreal orignX = event->pos().x()/HRatio;
        qreal orignY = event->pos().y()/VRatio;
        QPoint orign(qFloor(orignX),qFloor(orignY));
        if(orign.x()>=0&&orign.y()>=0&&orign.x()<weightWidth&&orign.y()<weightHeight&&orign!=lastMovePos)//in case out of range
            format(orign,editImg->pixel(lastClickPos));
        lastMovePos=orign;
    }
}

