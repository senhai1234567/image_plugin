#include "image.h"

const int image_w = 520;//显示图像的大小
const int image_h = 290;

Image::Image(CvMat  *pSrc)
{
//    Initialize_SrcImage(pSrcImage,&this->pSrc,&this->CutOutRect);//初始化pSrc
    Initialize_ScreenImage(pSrc,&this->pShow_8U,&this->pTemp,&this->pShow,&this->pGray2Rgb_8U,&pTemp_8U);//初始化pShow，pTemp，pShow
    int cn = CV_MAT_CN(pSrc->type);

    CV_Assert(cn == 1 || cn ==3);

//    CvMat2QImage(this->pShow,&this->LabelImage);//初始化LabelImage  （与pShow 共一个内存）
    CvMat2QImage(this->pTemp_8U,&this->pTemp_8U_label);//pTemp_copy  （与pTemp 共一个内存）
    CvMat2QImage(this->pShow_8U,&this->pShow_8U_label);//pShow  pShow 共一个内存）
    if(cn ==1)
    {
        CvMat2QImage(this->pGray2Rgb_8U,&this->pGray2Rgb_8U_label);
    }
    cvZero(this->pShow_8U);
    cvZero(this->pTemp);
    cvZero(this->pShow);
    cvZero(this->pGray2Rgb_8U);
    cvZero(this->pTemp_8U);

}

/*****************************
 * 函数功能                     ：初始化源图像（复制图像），初始化抠图区域
 * srcarr                      ：需要显示的图像
 *  pDst                       ：图像备份
 * CutOutRect                  ：初始化抠图区域
******************************/
void Image::Initialize_SrcImage(const CvArr *srcarr,CvMat** pDst,QRectF *CutOutRect)
{
    CvMat *pSrc = (CvMat*)srcarr;
    CvMat src_stub;

    pSrc = cvGetMat(srcarr,&src_stub);

    if(cvGetErrStatus() < 0)
    {
        cvError(CV_StsBackTrace,0,"inner function failed",__FILE__,__LINE__);
    }

    int img_width = pSrc->width;
    int img_height = pSrc->height;
    int cn = CV_MAT_CN(pSrc->type);
    int depth = CV_MAT_DEPTH(pSrc->type);

    CV_Assert(cn == 3 || cn == 1);
    CV_Assert(depth == CV_8U || depth == CV_32F || depth == CV_64F);

    CutOutRect->setRect((qreal)0,(qreal)0,(qreal)img_width,(qreal)img_height);

    if(cn == 1 )
        if(depth == CV_8U)
            *pDst = cvCreateMat(img_height,img_width,CV_8UC1);
        if (depth == CV_32F)
            *pDst = cvCreateMat(img_height,img_width,CV_32FC1);
        if (depth == CV_64F)
            *pDst = cvCreateMat(img_height,img_width,CV_64FC1);

    if(cn == 3 )
        if(depth == CV_8U)
            *pDst = cvCreateMat(img_height,img_width,CV_8UC3);
        if (depth == CV_32F)
            *pDst = cvCreateMat(img_height,img_width,CV_32FC3);
        if (depth == CV_64F)
            *pDst = cvCreateMat(img_height,img_width,CV_64FC3);

    cvConvert(pSrc , *pDst);    //深拷贝


//    this->pSrc = pSrc;
}

/*****************************
 * 函数功能                     ：初始化图像
 * pShow_8U                    ：最后显示的图像
 * pTemp                       ：抠图区域的放大部分
 * pShow                       ：初始化抠图区域
 * pGray2Rgb_8U                ：灰度图要用到的三通道8U的图像（）
 * pTemp_8U                    ：pTemp 的 8U图像
******************************/
void Image::Initialize_ScreenImage(CvMat  *pSrc,CvMat **pShow_8U,CvMat **pTemp,CvMat **pShow,CvMat **pGray2Rgb_8U,CvMat **pTemp_8U)
{

    int cn = CV_MAT_CN(pSrc->type);
    int depth = CV_MAT_DEPTH(pSrc->type);

    CV_Assert(cn == 3 || cn == 1);
    if(cn == 3)
    {
        *pShow_8U = cvCreateMat(image_h,image_w,CV_8UC3);
        *pTemp_8U = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC3);
        if(depth == CV_8U)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC3);
            *pShow = cvCreateMat(image_h,image_w,CV_8UC3);
        }
        if (depth == CV_32F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_32FC3);
            *pShow = cvCreateMat(image_h,image_w,CV_32FC3);
        }
        if (depth == CV_64F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_64FC3);
            *pShow = cvCreateMat(image_h,image_w,CV_64FC3);

        }
    }
    if(cn == 1)
    {
        *pShow_8U = cvCreateMat(image_h,image_w,CV_8UC1);
        *pTemp_8U = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC1);

        if(depth == CV_8U)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC1);
            *pShow = cvCreateMat(image_h,image_w,CV_8UC1);

        }
        if (depth == CV_32F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_32FC1);
            *pShow = cvCreateMat(image_h,image_w,CV_32FC1);
        }
        if (depth == CV_64F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_64FC1);
            *pShow = cvCreateMat(image_h,image_w,CV_64FC1);
        }
        *pGray2Rgb_8U = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC3);
    }

//    ScreenRect.setRect(0,0,image_w,image_h);
}


/****************************************
 * 函数功能                     ：将CvMat 型图像转换为 image型
*******************************************/
void Image::CvMat2QImage(CvMat *img,QImage **image)
{

    uchar *imgData=img->data.ptr;
    int cn = CV_MAT_CN(img->type);
//    *image=new QImage(imgData,img->width,img->height,QImage::Format_RGB888);
    if(cn == 3)
    {
        *image=new QImage(imgData,img->width,img->height,((img->width * CV_MAT_CN(img->type)) ),
                      QImage::Format_RGB888);
    }
    if(cn == 1)
    {
        *image=new QImage(imgData,img->width,img->height,((img->width * CV_MAT_CN(img->type)) ),
                          QImage::Format_Indexed8);
    }
}

Image::~Image()
{
    int cn = CV_MAT_CN(pTemp->type);

    if(cn == 1)
    {
        cvReleaseMat(&pGray2Rgb_8U);
        pGray2Rgb_8U = NULL;

        delete pGray2Rgb_8U_label;
        pGray2Rgb_8U_label = NULL;
    }

    cvReleaseMat(&pTemp);
    pTemp = NULL;

    cvReleaseMat(&pTemp_8U);
    pTemp_8U = NULL;

    cvReleaseMat(&pShow);
    pShow = NULL;

    cvReleaseMat(&pShow_8U);
    pShow_8U = NULL;



    delete pShow_8U_label;
    pShow_8U_label = NULL;

    delete pTemp_8U_label;
    pTemp_8U_label = NULL;
}
