#include "mainwindow.h"
#include <QApplication>

#include <cvshowmat.h>

using namespace std;

void cv8u2Float(IplImage *pSrcImage ,IplImage *pDstImage)
{
    int cn = pSrcImage->nChannels;
    int height = pSrcImage->height;
    int width = pSrcImage->width;

    uchar *pSrcData = (uchar *)pSrcImage->imageData;
    float *pDstData = (float *)pDstImage->imageData;
    if( cn == 1 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->widthStep ,pDstData += pDstImage->width)
        {
            for(int j = 0; j < width; j ++)
            {
                pDstData[j] = pSrcData[j] / 255.0;
            }
        }

    }

    if( cn == 3 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->widthStep ,pDstData += pDstImage->width * cn)
        {
            for(int j = 0; j < width; j ++)
            {

                pDstData[j * cn] = pSrcData[j * cn] / 255.0;
                pDstData[j * cn + 1] = pSrcData[j * cn + 1] / 255.0;
                pDstData[j * cn + 2] = pSrcData[j * cn + 2] / 255.0;
            }
        }

    }
}

void cvFloat28U(IplImage *pSrcImage ,IplImage *pDstImage)
{
    int cn = pSrcImage->nChannels;
    int height = pSrcImage->height;
    int width = pSrcImage->width;

    uchar *pDstData = (uchar *)pDstImage->imageData;
    float *pSrcData = (float *)pSrcImage->imageData;
    if( cn == 1 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->width ,pDstData += pDstImage->widthStep)
        {
            for(int j = 0; j < width; j ++)
            {
                pDstData[j] = pSrcData[j];
            }
        }

    }

    if( cn == 3 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->width * cn ,pDstData += pDstImage->widthStep)
        {
            for(int j = 0; j < width; j ++)
            {

                pDstData[j * cn] = pSrcData[j * cn];
                pDstData[j * cn + 1] = pSrcData[j * cn + 1];
                pDstData[j * cn + 2] = pSrcData[j * cn + 2];
            }
        }

    }
}

void cv8uToDouble(IplImage *pSrcImage ,IplImage *pDstImage)
{
    int cn = pSrcImage->nChannels;
    int height = pSrcImage->height;
    int width = pSrcImage->width;

    uchar *pSrcData = (uchar *)pSrcImage->imageData;
    double *pDstData = (double *)pDstImage->imageData;
    if( cn == 1 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->widthStep ,pDstData += pDstImage->width)
        {
            for(int j = 0; j < width; j ++)
            {
                pDstData[j] = pSrcData[j];
            }
        }

    }

    if( cn == 3 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->widthStep ,pDstData += pDstImage->width * cn)
        {
            for(int j = 0; j < width; j ++)
            {
                pDstData[j * cn] = pSrcData[j * cn];
                pDstData[j * cn + 1] = pSrcData[j * cn + 1];
                pDstData[j * cn + 2] = pSrcData[j * cn + 2];
            }
        }

    }
}

void cvDoubleTo8U(IplImage *pSrcImage ,IplImage *pDstImage)
{
    int cn = pSrcImage->nChannels;
    int height = pSrcImage->height;
    int width = pSrcImage->width;

    double *pSrcData = (double *)pSrcImage->imageData;
    uchar *pDstData = (uchar *)pDstImage->imageData;
    if( cn == 1 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->width * cn,pDstData += pDstImage->widthStep)
        {
            for(int j = 0; j < width; j ++)
            {
                pDstData[j] = pSrcData[j];
            }
        }

    }

    if( cn == 3 )
    {
        for(int i = 0; i < height; i++,pSrcData += pSrcImage->width * cn ,pDstData += pDstImage->widthStep)
        {
            for(int j = 0; j < width; j ++)
            {

                pDstData[j * cn] = pSrcData[j * cn];
                pDstData[j * cn + 1] = pSrcData[j * cn + 1];
                pDstData[j * cn + 2] = pSrcData[j * cn + 2];
            }
        }

    }
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//    vector<cvShowMat> v(20);
    IplImage *pSrcImage = cvLoadImage("fog.png",1);
    IplImage *pSrcImage1 = cvLoadImage("AddText.png",0);
//    IplImage *pSrcImage1 = cvLoadImage("fog.png",0);
//    cvShowMat show("123",pSrcImage);

//    cvShowImage("1",pSrcImage1);

//    cvShow("picture_name", pSrcImage);
    cvShow("picture_name", pSrcImage1);

    IplImage *pFloatImage = cvCreateImage({pSrcImage1->width,pSrcImage1->height},IPL_DEPTH_32F,pSrcImage1->nChannels);
    IplImage *pFloatResult = cvCreateImage({pSrcImage1->width,pSrcImage1->height},IPL_DEPTH_8U,pSrcImage1->nChannels);
    cv8u2Float(pSrcImage1,pFloatImage);
//    cvConvertScale(pFloatImage,pFloatResult,255.0,0);
//    cvFloat28U(pFloatImage,pFloatResult);
//    cvShowImage("Float ",pFloatResult);
    cvShow("Float", pFloatImage);

    IplImage *pDoubleImage = cvCreateImage({pSrcImage1->width,pSrcImage1->height},IPL_DEPTH_64F,pSrcImage1->nChannels);
    IplImage *pDoubleResult = cvCreateImage({pSrcImage1->width,pSrcImage1->height},IPL_DEPTH_8U,pSrcImage1->nChannels);
    cv8uToDouble(pSrcImage1,pDoubleImage);
//    cvDoubleTo8U(pDoubleImage,pDoubleResult);
//    cvShowImage("pDouble ",pDoubleResult);
    cvShow("pDouble", pDoubleImage);


    return a.exec();
}
