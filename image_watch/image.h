#ifndef IMAGE_H
#define IMAGE_H
#include <QTimer>
#include <QLabel>
#include <cv.h>
#include <highgui.h>
#include <QMouseEvent>
#include <QPainter>

class Image
{
public:
    Image(CvMat  *pSrc);
    ~Image();
public:
//    CvMat *pSrc;
    CvMat *pTemp;       //方格中的像素值
    CvMat *pTemp_8U;    //显示方格，矩形框
    CvMat *pGray2Rgb_8U;//用于灰度的
    CvMat *pShow;  //显示状态栏像素值
    CvMat *pShow_8U;    //最后的图像显示

    //这个只定义头文件，所以只占头文件的内存。他们图像内存空间和CvMat的公用
    QImage *pShow_8U_label;//pShow_8U 公用内存空间
    QImage *pTemp_8U_label;//与pTemp 公用内存空间
    QImage *pGray2Rgb_8U_label;//

    void Initialize_SrcImage(const CvArr *srcarr,CvMat** pDst,QRectF *CutOutRect);
    void CvMat2QImage(CvMat *img,QImage **image);
    void Initialize_ScreenImage(CvMat  *pSrc,CvMat **pShow_8U,CvMat **pTemp,CvMat **pShow,CvMat **pGray2Rgb_8U,CvMat **pTemp_8U);
};

#endif // IMAGE_H
