#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "image.h"
const double wheel_scale = 1.1;
const double iwheel_scale = 1 / 1.1;
const float squres_max_number = 17;

const int image_w = 520;//显示图像的大小
const int image_h = 290;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

//    IplImage *pSrcImage = cvLoadImage("fog.png",0);

//    this->name = "fog.png";
//    Initialize_Image(pSrcImage);

//    Initialize_Window("pSrcImage");
//    Initialize_Timers(10);
//    Initialize_Statusbar();
//    Initialize_Label();
//    Initialize_Zoom();
//    Initialize_mouse();

//    CutOutRect.setRect(0.0,0.0,pSrcImage->width,pSrcImage->height);
////    CutOutRect.setRect(0.0,0.0,3.6,4.8);
//    Scale_Pixel();
//    cvGetSubImage(this->pSrc,this->pTemp,&CutOutRect);

//    cvGetROI(this->pTemp,this->pShow,&CutOutRect);
//    cvCopy(this->pShow,this->pShow);
//    QtShowLabelImage(this->pShow);//LabelImage 与 pShow 公用内存

//    Initialize_Param();
//    cvReleaseImage(&pSrcImage);
//    pSrcImage = NULL;
//    Initialize();
}

MainWindow::~MainWindow()
{
    delete ui;
}

/**************************************
 *函数功能                   ：针对uchar型数据找出 最大，最小值
 * pSrc                     ：图像首地址
 * width                    ：图像宽度
 * height                   ：图像高度
 * step                     ：行步长
 * cn                       ：图像通道数
 * rgb                      ：rgb位置
 * dmax                     ：最大值
 * dmin                     ：最小值
**************************************/
void cvMaxMin_char(uchar*pSrc,int width, int height,int step,int cn,int rgb,double *dmax,double *dmin)
{
    for(int i= 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if( *(pSrc + i * step + j * cn + rgb) > *dmax)
                *dmax = *(pSrc + i * step + j * cn + rgb);
            if( *(pSrc + i * step + j * cn + rgb) < *dmin)
                *dmin = *(pSrc + i * step + j * cn + rgb);
        }
    }
}

/**************************************
 *函数功能                   ：针对float，double型数据找出 最大，最小值
 * pSrc                     ：图像首地址
 * width                    ：图像宽度
 * height                   ：图像高度
 * cn                       ：图像通道数
 * rgb                      ：rgb位置
 * dmax                     ：最大值
 * dmin                     ：最小值
**************************************/
template<class T> void cvMaxMin_(T*pSrc,int width, int height,int cn,int rgb,double *dmax,double *dmin)
{
    for(int i= 0; i < height; i++)
    {
        for(int j = 0; j < width; j++)
        {
            if( *(pSrc + i * width + j * cn + rgb) > static_cast<T>(*dmax))
                *dmax = static_cast<double>(*(pSrc + i * width + j * cn + rgb));
            if( *(pSrc + i * width + j * cn + rgb) < static_cast<T>(*dmin))
                *dmin = static_cast<double>(*(pSrc + i * width + j * cn + rgb));
        }
    }
}

/**************************************
 *函数功能                   ：找出图像的最大值，最小值
 * dmax                     ：最大值
 * dmin                     ：最小值
**************************************/
void cvMaxMin(CvMat *pTemp,double *dmax,double *dmin)
{
   int cn =  CV_MAT_CN(pTemp->type);
   int depth = CV_MAT_DEPTH(pTemp->type);

   int width = pTemp->width;
   int height = pTemp->height;
   int step = pTemp->step;

   assert(cn == 1 || cn == 3);
   assert(depth == CV_32F || depth == CV_64F || depth == CV_8U);
   if(depth == CV_8U)
   {
       if(cn == 1)
       {
           cvMaxMin_char(pTemp->data.ptr,width,height,step,cn,0,dmax,dmin);
       }
       if(cn == 3)
       {
           cvMaxMin_char(pTemp->data.ptr,width,height,step,cn,0,dmax,dmin);
           cvMaxMin_char(pTemp->data.ptr,width,height,step,cn,1,dmax,dmin);
           cvMaxMin_char(pTemp->data.ptr,width,height,step,cn,2,dmax,dmin);
       }
   }
   if(depth == CV_32F)
   {
       if(cn == 1)
       {
           cvMaxMin_(pTemp->data.fl,width,height,cn,0,dmax,dmin);
       }
       if(cn == 3)
       {
           cvMaxMin_(pTemp->data.fl,width,height,cn,0,dmax,dmin);
           cvMaxMin_(pTemp->data.fl,width,height,cn,1,dmax,dmin);
           cvMaxMin_(pTemp->data.fl,width,height,cn,2,dmax,dmin);
       }
   }
   if(depth == CV_64F)
   {
       if(cn == 1)
       {
           cvMaxMin_(pTemp->data.db,width,height,cn,0,dmax,dmin);
       }
       if(cn == 3)
       {
           cvMaxMin_(pTemp->data.db,width,height,cn,0,dmax,dmin);
           cvMaxMin_(pTemp->data.db,width,height,cn,1,dmax,dmin);
           cvMaxMin_(pTemp->data.db,width,height,cn,2,dmax,dmin);
       }
   }
}

/*************************************
 * 函数功能                         ：初始化
 * 初始化图像
 * 初始化窗口
 * 初始化定时器
 * 初始化状态控件
 * 初始化label控件
 * 初始化鼠标事件
 * 初始化CutOutRect
 * 初始化显示图像与源图像的像素宽度比
 * 将原图需要抠取的部分（floor[x],floor[y],ceil[x + w],ceil[y + h]）
 *          放大到 (0,0,image_w * 3/ 2,image_h * 3/ 2) （该图像可能是float型，可能是double型
 *          也可能是小数哦）
 * 从(0,0,image_w * 3/ 2,image_h * 3/ 2) (cvGetSubImage) 截取
 *          宽为image_w，高为image_h 的图像（该图像可能是float型，可能是double型
 *          也可能是小数哦，该图像用于图片上显示像素值）
 * 将图像转换为8位图像（QImage只接受8位的数据）
 * 从8位的图像里抠出需要显示的图像区域
 * 初始化参数
***************************************/
void MainWindow::Initialize(void)
{
//    IplImage *pSrcImage = cvLoadImage("fog.png",0);

//    this->name = "fog.png";
    Initialize_Image(pImage);

//    Initialize_SrcImage(pImage,&this->pSrc,&this->CutOutRect);//初始化pSrc
    Image Image_all(pSrc);
    if(CV_MAT_CN(pSrc->type) == 1)
    {
        if(CV_MAT_DEPTH(pSrc->type) == CV_8U)
            pShow_Copy = cvCreateMat(Image_all.pShow->height,Image_all.pShow->width,CV_8UC1);
        if(CV_MAT_DEPTH(pSrc->type) == CV_32F)
            pShow_Copy = cvCreateMat(Image_all.pShow->height,Image_all.pShow->width,CV_32FC1);
        if(CV_MAT_DEPTH(pSrc->type) == CV_64F)
            pShow_Copy = cvCreateMat(Image_all.pShow->height,Image_all.pShow->width,CV_64FC1);
    }

    if(CV_MAT_CN(pSrc->type) == 3)
    {
        if(CV_MAT_DEPTH(pSrc->type) == CV_8U)
            pShow_Copy = cvCreateMat(Image_all.pShow->height,Image_all.pShow->width,CV_8UC3);
        if(CV_MAT_DEPTH(pSrc->type) == CV_32F)
            pShow_Copy = cvCreateMat(Image_all.pShow->height,Image_all.pShow->width,CV_32FC3);
        if(CV_MAT_DEPTH(pSrc->type) == CV_64F)
            pShow_Copy = cvCreateMat(Image_all.pShow->height,Image_all.pShow->width,CV_64FC3);
    }

    cvZero(pShow_Copy);

    Initialize_Window(this->name);
    Initialize_Timers(10);
    Initialize_Statusbar();

    Initialize_Zoom();
    Initialize_mouse();

    CutOutRect.setRect(0.0,0.0,pImage->width,pImage->height);
//    CutOutRect.setRect(0.0,0.0,3.6,4.8);
    Scale_Pixel();
    cvGetSubImage(this->pSrc,Image_all.pTemp,&CutOutRect);

    cvGetROI(Image_all.pTemp,Image_all.pShow,&CutOutRect,this->uint_width,this->uint_height);

    cvConvert(Image_all.pShow,pShow_Copy);

    double dmax = DBL_MIN,dmin =DBL_MAX;
    cvMaxMin(Image_all.pTemp,&dmax,&dmin);
    if(dmax <= 1)
        cvConvertScale(Image_all.pTemp,Image_all.pTemp_8U,255.0,0);
    else
        cvConvert(Image_all.pTemp,Image_all.pTemp_8U);

    cvGetROI(Image_all.pTemp_8U,Image_all.pShow_8U,&CutOutRect,this->uint_width,this->uint_height);
//    cvGetROI(this->pTemp,this->pShow_8U,&CutOutRect);
//    cvCopy(this->pShow_8U,this->pShow);
    QtShowLabelImage(Image_all.pShow_8U);//LabelImage 与 pShow 公用内存

    Initialize_Param();
//    cvReleaseImage(&pImage);
//    pImage = NULL;
}

//初始化定时器，并且设置定时中断时间 delay
void MainWindow::Initialize_Timers(int delay)
{
    update_timer = new QTimer;

    connect(update_timer,SIGNAL(timeout()),this,SLOT(Loop()));

    update_timer->start(delay);

}
//初始化鼠标追踪，鼠标左击，鼠标右击
void MainWindow::Initialize_mouse()
{
    // 鼠标追踪
    this->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);
    mouse_down_[0] = 0;
    mouse_down_[1] = 0;
}

/*************************************
 * 主循环过程如下
 * 是否需要更新(task_update == true)
 *      是否更新完成(ready_flag == true)
 *          不需要更新(task_update = false)
 *          没有更新完成(ready_flag = false)
 *          检查是否存在缩放，并且计算缩放之后的抠图大小(CheckZoom)
 *          检查是否存在平移，并且计算平移后的抠图大小(CheckMove)
 *          检查矩阵是否超过原图的范围，如果超过了，就纠正(CheckRect)
 *          计算需要显示的图像像素与源图像的像素大小的比例(Scale_Pixel)
 *          从原图需要抠取的部分（floor[x],floor[y],ceil[x + w],ceil[y + h]）放大到 (0,0,image_w * 3/ 2,image_h * 3/ 2) （该图像可能是float，double，小数）
 *          从(0,0,image_w * 3/ 2,image_h * 3/ 2) (cvGetSubImage) 找出 最后的显示图像（宽为image_w，高为image_h）（该图像可能是float，double，小数）
 *          将图像转换为8位图像（QImage只接受8位的数据，可能要做必要的放缩）
 *          是否达到需要显示像素的大小
 *              图像通道数为3
 *                  给图像添加黑色分割线
 *                  设置字体大小，颜色
 *                  在图像上添加像素值
 *              图像通道数为1
 *                  给图像添加黑色分割线
 *                  将灰色图像转换为彩色图像（QImage 只处理彩色图像）
 *                  设置字体大小，颜色
 *                  在图像上添加像素值
 *                  将彩色图像转换为灰度图像
 *          抠出需要显示的图片
 *          显示图片
 * 鼠标是否更新了
 *      显示鼠标位置的像素值
*************************************/
void MainWindow::Loop()
{
    Image Image_all(pSrc);
    if(task_update == true)//需要更新
    {
        if(ready_flag == true)//已经完成了，可以开始了下一轮的处理
        {
            task_update = false;
            ready_flag = false;//未完成标志
            CheckZoom(&CutOutRect);
            CheckMove(&CutOutRect);
            CheckRect(&CutOutRect);
            Scale_Pixel();

            cvGetSubImage(pSrc,Image_all.pTemp,&CutOutRect);

            cvGetROI(Image_all.pTemp,Image_all.pShow,&CutOutRect,this->uint_width,this->uint_height);

            double dmax = DBL_MIN,dmin =DBL_MAX;
            cvMaxMin(Image_all.pTemp,&dmax,&dmin);

            if(dmax <= 1)
                cvConvertScale(Image_all.pTemp,Image_all.pTemp_8U,255.0,0);
            else
                cvConvert(Image_all.pTemp,Image_all.pTemp_8U);
//            cvGetROI(Image_all.pTemp_8U,Image_all.pShow_8U,&CutOutRect);

            cvGetROI(Image_all.pTemp,Image_all.pShow,&CutOutRect,this->uint_width,this->uint_height);

            cvConvert(Image_all.pShow,pShow_Copy);

//            cvMaxMin(Image_all.pShow,&dmax,&dmin);

            //给图片添加文字和线框
            if(CutOutRect.height() < static_cast<double>(squres_max_number) && CutOutRect.width() < static_cast<double>(squres_max_number))
            {

                if(img_cn == 3)
                {
                    cvAddLine(Image_all.pTemp_8U,uint_width ,uint_height,1);

                    QPainter painter(Image_all.pTemp_8U_label); //为这个QImage构造一个QPainter
                    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

                    //改变画笔和字体
                    QPen pen = painter.pen();

                    int Amplifyimg_width = static_cast<int>(ceil(CutOutRect.width() + CutOutRect.x()) - floor(CutOutRect.x()));
                    int Amplifyimg_height = static_cast<int>(ceil(CutOutRect.height() + CutOutRect.y()) - floor(CutOutRect.y()));
                    //设置字体大小  公式为 12 * MIN(fUint_pixel_col,fUint_pixel_row) / 32
                    QtSetFont(&painter, static_cast<float>(uint_width * 0.9), static_cast<float>(uint_height *0.9));

                    //在图片上添加像素值
                    QtDrawText(Image_all.pTemp, &painter,Amplifyimg_height, Amplifyimg_width , static_cast<float>(uint_width), static_cast<float>(uint_height), &pen);

                    //for debug
//                    IplImage *pDstImage = cvCreateImage({pTemp->width ,pTemp->height },IPL_DEPTH_8U,CV_MAT_CN(pTemp->type));
//                    cvConvert(pTemp, pDstImage);    //深拷贝
//                    cvShowImage("pGray2Rgb",pDstImage);
//                //    cvReleaseMat(&pDst);
//                    cvReleaseImage(&pDstImage);

                }

                if(img_cn == 1)
                {
                    //注意：因为Qimage里面没有单独对灰度处理的函数（我没有找到），所以我就转换到了rgb空间进行处理
                    cvAddLine(Image_all.pTemp_8U,uint_width ,uint_height,1);
                    cvGray2Rgb(Image_all.pTemp_8U,Image_all.pGray2Rgb_8U);

//                    cvGray2Rgb(this->pTemp_8U,this->pGray2Rgb_8U);
//                    cvAddLine(this->pGray2Rgb_8U,uint_width ,uint_height,1);

                    QPainter painter(Image_all.pGray2Rgb_8U_label); //为这个QImage构造一个QPainter
                    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);

                    //改变画笔和字体
                    QPen pen = painter.pen();

                    int Amplifyimg_width = static_cast<int>(ceil(CutOutRect.width() + CutOutRect.x()) - floor(CutOutRect.x()));
                    int Amplifyimg_height =  static_cast<int>(ceil(CutOutRect.height() + CutOutRect.y()) - floor(CutOutRect.y()));

                    //设置字体大小  公式为 12 * MIN(fUint_pixel_col,fUint_pixel_row) / 32
                    QtSetFont(&painter, static_cast<float>(uint_width * 0.9) , static_cast<float>(uint_height * 0.9));

                    //在图片上添加像素值
                    QtDrawText(Image_all.pTemp, &painter,Amplifyimg_height, Amplifyimg_width ,static_cast<float>(uint_width), static_cast<float>(uint_height), &pen);

                    cvRgb2Gray(Image_all.pGray2Rgb_8U,Image_all.pTemp_8U);

                    //for debug
//                    IplImage *pDstImage = cvCreateImage({pGray2Rgb->width ,pGray2Rgb->height },IPL_DEPTH_8U,CV_MAT_CN(pGray2Rgb->type));
//                    cvConvert(pGray2Rgb, pDstImage);    //深拷贝
//                    cvShowImage("pGray2Rgb",pDstImage);
//                //    cvReleaseMat(&pDst);
//                    cvReleaseImage(&pDstImage);
                }

            }



            cvGetROI(Image_all.pTemp_8U,Image_all.pShow_8U,&CutOutRect,this->uint_width,this->uint_height);

            QtShowLabelImage(Image_all.pShow_8U);
            std::cout <<"（"<< CutOutRect.x() << "," <<CutOutRect.y()<<","
                     <<CutOutRect.width() << "," <<CutOutRect.height()
                     <<")" <<std::endl;
            ready_flag = true;
        }
    }
    else//不需要更新
    {

    }


    if( mouse_move_statue == true)
    {
        mouse_move_statue = false;
        QtUpdateStatusBar(this->pShow_Copy,&this->CutOutRect,mouse_locate);
    }
}

/*****************************
 * 函数功能                     ：初始化窗口
 * str                         ：窗口名字
 * 设置窗口大小
******************************/
void MainWindow::Initialize_Window(char *str)
{
   QString qstr = QString::fromStdString(str);
   this->setWindowTitle(qstr);
   this->resize(static_cast<int>(ScreenRect.width()),static_cast<int>(ScreenRect.height()) + ui->mainToolBar->height());
}

/*****************************
 * 函数功能                     ：初始化源图像（复制图像），初始化抠图区域
 * srcarr                      ：需要显示的图像
 *  pDst                       ：图像备份
 * CutOutRect                  ：初始化抠图区域
******************************/
void MainWindow::Initialize_SrcImage(const CvArr *srcarr,CvMat** pDst,QRectF *CutOutRect)
{
    const CvMat *pSrc = static_cast<const CvMat*>(srcarr);
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
    if(depth == CV_8U)
        img_depth = CV_8U;
    if(depth == CV_32F)
        img_depth = CV_32F;
    if(depth == CV_64F)
        img_depth = CV_64F;

    Src_width = img_width;
    Src_height = img_height;

    img_cn =cn;
    CutOutRect->setRect(0,0,img_width,img_height);

    if( cn == 1 )
    {
        if(img_depth == CV_8U)
            *pDst = cvCreateMat(img_height,img_width,CV_8UC1);
        if (img_depth == CV_32F)
            *pDst = cvCreateMat(img_height,img_width,CV_32FC1);
        if (img_depth == CV_64F)
            *pDst = cvCreateMat(img_height,img_width,CV_64FC1);
    }

    if( cn == 3 )
    {
        if(img_depth == CV_8U)
            *pDst = cvCreateMat(img_height,img_width,CV_8UC3);
        if (img_depth == CV_32F)
            *pDst = cvCreateMat(img_height,img_width,CV_32FC3);
        if (img_depth == CV_64F)
            *pDst = cvCreateMat(img_height,img_width,CV_64FC3);
    }
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
void MainWindow::Initialize_ScreenImage(CvMat **pShow_8U,CvMat **pTemp,CvMat **pShow,CvMat **pGray2Rgb_8U,CvMat **pTemp_8U)
{

    CV_Assert(img_cn == 3 || img_cn == 1);
    if(img_cn == 3)
    {
        *pShow_8U = cvCreateMat(image_h,image_w,CV_8UC3);
        *pTemp_8U = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC3);
        if(img_depth == CV_8U)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC3);
            *pShow = cvCreateMat(image_h,image_w,CV_8UC3);
        }
        else if (img_depth == CV_32F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_32FC3);
            *pShow = cvCreateMat(image_h,image_w,CV_32FC3);
        }
        else if (img_depth == CV_64F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_64FC3);
            *pShow = cvCreateMat(image_h,image_w,CV_64FC3);

        }
    }
    if(img_cn == 1)
    {
        *pShow_8U = cvCreateMat(image_h,image_w,CV_8UC1);
        *pTemp_8U = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC1);

        if(img_depth == CV_8U)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC1);
            *pShow = cvCreateMat(image_h,image_w,CV_8UC1);

        }
        else if (img_depth == CV_32F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_32FC1);
            *pShow = cvCreateMat(image_h,image_w,CV_32FC1);
        }
        else if (img_depth == CV_64F)
        {
            *pTemp = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_64FC1);
            *pShow = cvCreateMat(image_h,image_w,CV_64FC1);
        }
        *pGray2Rgb_8U = cvCreateMat(image_h * 5 / 3, image_w * 5 / 3 ,CV_8UC3);
    }

    ScreenRect.setRect(0,0,image_w,image_h);
}

/*****************************
 * 函数功能                     ：初始化状态栏
 * 状态栏信息显示
 * 状态栏的大小调节
******************************/
void MainWindow::Initialize_Statusbar()
{
    QStatusBar* pStatusBar = ui->statusBar;
    pStatusBar->showMessage(tr("Bar is Ok"));
    pStatusBar->resize(static_cast<int>(ScreenRect.width()),17);
}

/*****************************
 * 函数功能                     ：显示图像的像素与源图像的像素的缩放比例
******************************/
void MainWindow::Scale_Pixel()
{
    uint_width = ScreenRect.width() / CutOutRect.width();
    uint_height = ScreenRect.height() / CutOutRect.height();
    iuint_width = 1.0 / uint_width;
    iuint_height = 1.0 / uint_height;
}

/*****************************
 * 函数功能                     ：根据CutOutRect的大小，截取源图像区域，然后放大到(0,0,image_w * 3/ 2,image_h * 3/ 2)
******************************/
void MainWindow::cvGetSubImage(const CvMat *pSrc,CvMat *pDst,QRectF *CutOutRect)
{
    int Src_step = pSrc->step;
    int Src_cn = CV_MAT_CN(pSrc->type);
    int Src_depth = CV_MAT_DEPTH((pSrc->type));

    /*******************************************************88
     * 图像必须是单通道或者三通道
     * cutout_width，cutout_height 的像素个数不能小于 3
     * 子区域的大小不能超过 图像的范围
     ***********************************************************/
    CV_Assert(Src_depth == CV_8U || Src_depth == CV_32F || Src_depth == CV_64F);
    CV_Assert(Src_cn == 3 || Src_cn == 1);
    CV_Assert(CutOutRect->width() > 3.0 && CutOutRect->height() > 3.0);
    CV_Assert(CutOutRect->x() - floor(CutOutRect->x()) >= 0);
    CV_Assert(CutOutRect->y() - floor(CutOutRect->y()) >= 0);
//    CV_Assert(CutOutRect->x() + CutOutRect->width() <= image_w);
//    CV_Assert(CutOutRect->y() + CutOutRect->height() <= image_h);

    CvPoint origin,end;
    origin.x = static_cast<int>(CutOutRect->x());
    origin.y = static_cast<int>(CutOutRect->y());
    end.x = static_cast<int>(ceil(CutOutRect->x() + CutOutRect->width()));
    end.y = static_cast<int>(ceil(CutOutRect->y() + CutOutRect->height()));

    int Dst_step = pDst->step;


    int height = static_cast<int>((end.y - origin.y) * uint_height);
    int width = static_cast<int>((end.x - origin.x) * uint_width);

    if(Src_cn == 1)
    {
        if(img_depth == CV_8U)
        {
            uchar *pSrcData = pSrc->data.ptr;
            uchar *pDstData = pDst->data.ptr;
            for(int i = 0, rows = origin.y; i < height; i++, pDstData += Dst_step)
            {
                for(int j = 0, cols = origin.x; j < width; j++)
                {

                    *(pDstData + j * Src_cn) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * Src_step + (static_cast<int>(j * iuint_width) + cols) * Src_cn) ;
                }
            }
        }

        if(img_depth == CV_32F)
        {
            float *pSrcData = pSrc->data.fl;
            float *pDstData = pDst->data.fl;
            for(int i = 0, rows = origin.y; i < height; i++, pDstData += pDst->width)
            {
                for(int j = 0, cols = origin.x; j < width; j++)
                {

                    *(pDstData + j * Src_cn) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * pSrc->width + (static_cast<int>(j * iuint_width) + cols) * Src_cn) ;
                }
            }
        }

        if(img_depth == CV_64F)
        {
            double *pSrcData = pSrc->data.db;
            double *pDstData = pDst->data.db;
            for(int i = 0, rows = origin.y; i < height; i++, pDstData += pDst->width)
            {
                for(int j = 0, cols = origin.x; j < width; j++)
                {

                    *(pDstData + j * Src_cn) =
                            *(pSrcData + ( static_cast<int>(i * iuint_height) + rows) * pSrc->width + ( static_cast<int>(j * iuint_width) + cols) * Src_cn) ;
                }
            }
        }
    }

    if(Src_cn == 3)
    {
        if(img_depth == CV_8U)
        {
            uchar *pSrcData = pSrc->data.ptr;
            uchar *pDstData = pDst->data.ptr;
            for(int i = 0, rows = origin.y; i < height; i++, pDstData += Dst_step)
            {
                for(int j = 0, cols = origin.x; j < width; j++)
                {

                    *(pDstData + j * Src_cn) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * Src_step + (static_cast<int>(j * iuint_width) + cols) * Src_cn) ;
                    *(pDstData + j * Src_cn + 1) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * Src_step + (static_cast<int>(j * iuint_width) + cols) * Src_cn + 1) ;
                    *(pDstData + j * Src_cn + 2) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * Src_step + (static_cast<int>(j * iuint_width) + cols) * Src_cn + 2) ;
                }
            }
        }

        if(img_depth == CV_32F)
        {
            float *pSrcData = pSrc->data.fl;
            float *pDstData = pDst->data.fl;
            for(int i = 0, rows = origin.y; i < height; i++, pDstData += pDst->width * Src_cn)
            {
                for(int j = 0, cols = origin.x; j < width; j++)
                {

                    *(pDstData + j * Src_cn) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * pSrc->width *  Src_cn+ (static_cast<int>(j * iuint_width) + cols) * Src_cn) ;
                    *(pDstData + j * Src_cn + 1) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * pSrc->width *  Src_cn + (static_cast<int>(j * iuint_width) + cols) * Src_cn + 1) ;
                    *(pDstData + j * Src_cn + 2) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * pSrc->width *  Src_cn + (static_cast<int>(j * iuint_width) + cols) * Src_cn + 2) ;
                }
            }
        }

        if(img_depth == CV_64F)
        {
            double *pSrcData = pSrc->data.db;
            double *pDstData = pDst->data.db;
            for(int i = 0, rows = origin.y; i < height; i++, pDstData += pDst->width * Src_cn)
            {
                for(int j = 0, cols = origin.x; j < width; j++)
                {

                    *(pDstData + j * Src_cn) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * pSrc->width *  Src_cn + (static_cast<int>(j * iuint_width) + cols) * Src_cn) ;
                    *(pDstData + j * Src_cn + 1) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * pSrc->width *  Src_cn + (static_cast<int>(j * iuint_width) + cols) * Src_cn + 1) ;
                    *(pDstData + j * Src_cn + 2) =
                            *(pSrcData + (static_cast<int>(i * iuint_height) + rows) * pSrc->width *  Src_cn + (static_cast<int>(j * iuint_width) + cols) * Src_cn + 2) ;
                }
            }
        }
    }
}

/*****************************
 * 函数功能                     ：根据CutOutRect以及缩放比例，提取图像中的ROI区域
******************************/
void MainWindow::cvGetROI(CvMat *pSrc,CvMat *pDst,QRectF *CutOutRect,qreal uint_width,qreal uint_height)
{
    int cn = CV_MAT_CN(pSrc->type);
    int depth = CV_MAT_DEPTH(pSrc->type);


    int Src_width = pSrc->width;
    int Src_height = pSrc->height;
    int Src_step = pSrc->step;

    int Dst_width = pDst->width;
    int Dst_height = pDst->height;
    int Dst_step = pDst->step;
    int Dst_cn = CV_MAT_CN(pDst->type);
    int Dst_depth = CV_MAT_DEPTH(pDst->type);
    CV_Assert(Dst_height < Src_height);
    CV_Assert(Dst_width < Src_width);
    CV_Assert(cn == 1 || cn == 3);
    assert(Dst_cn == cn );
    assert(Dst_depth == depth );
    assert(depth == CV_8U || depth == CV_32F || depth == CV_64F);

    CvPoint2D64f origin;
    origin.x = CutOutRect->x() - static_cast<int>(CutOutRect->x());
    origin.y = CutOutRect->y() - static_cast<int>(CutOutRect->y());

    origin.x *= uint_width;
    origin.y *= uint_height;



    if(cn == 1)
     {
        if(depth == CV_8U)
        {
            uchar *pDstData = pDst->data.ptr;
            uchar *pSrcData = pSrc->data.ptr + static_cast<int>(origin.y) * Src_step +static_cast<int>(origin.x) * cn;
             for(int i = 0; i < Dst_height; i++, pDstData+= pDst->width * cn, pSrcData += Src_step)
             {
                 for(int j = 0; j < Dst_width; j++)
                 {
                     *(pDstData + j * cn) = pSrcData[j * cn];
                 }
             }
        }

        if(depth == CV_32F)
        {
            float *pDstData = pDst->data.fl;
            float *pSrcData = pSrc->data.fl + static_cast<int>(origin.y) * pSrc->width * cn + static_cast<int>(origin.x) * cn;
             for(int i = 0; i < Dst_height; i++, pDstData+= pDst->width * cn, pSrcData += pSrc->width * cn)
             {
                 for(int j = 0; j < Dst_width; j++)
                 {
                     *(pDstData + j * cn) = pSrcData[j * cn];
                 }
             }
        }

        if(depth == CV_64F)
        {
            double *pDstData = pDst->data.db;
            double *pSrcData = pSrc->data.db + static_cast<int>(origin.y) * pSrc->width * cn + static_cast<int>(origin.x) * cn;
             for(int i = 0; i < Dst_height; i++, pDstData+= pDst->width * cn, pSrcData += pSrc->width * cn)
             {
                 for(int j = 0; j < Dst_width; j++)
                 {
                     *(pDstData + j * cn) = pSrcData[j * cn];
                 }
             }
        }
     }

    if(cn == 3)
    {
        if(depth == CV_8U)
        {
            uchar *pDstData = pDst->data.ptr;
            uchar *pSrcData = pSrc->data.ptr + static_cast<int>(origin.y) * Src_step + static_cast<int>(origin.x) * cn;
            for(int i = 0; i < Dst_height; i++, pDstData+= pDst->width * cn, pSrcData += Src_step)
            {
                for(int j = 0; j < Dst_width; j++)
                {
                    *(pDstData + j * cn) = pSrcData[j * cn + 0];
                    *(pDstData + j * cn + 1) = pSrcData[j * cn + 1];
                    *(pDstData + j * cn + 2) = pSrcData[j * cn + 2];
                }
            }
        }

        if(depth == CV_32F)
        {
            float *pDstData = pDst->data.fl;
            float *pSrcData = pSrc->data.fl + static_cast<int>(origin.y) * pSrc->width * cn + static_cast<int>(origin.x) * cn;
            for(int i = 0; i < Dst_height; i++, pDstData+= pDst->width * cn, pSrcData += pSrc->width * cn)
            {
                for(int j = 0; j < Dst_width; j++)
                {
                    *(pDstData + j * cn) = pSrcData[j * cn + 0];
                    *(pDstData + j * cn + 1) = pSrcData[j * cn + 1];
                    *(pDstData + j * cn + 2) = pSrcData[j * cn + 2];
                }
            }
        }

        if(depth == CV_64F)
        {
            double *pDstData = pDst->data.db;
            double *pSrcData = pSrc->data.db + static_cast<int>(origin.y) * pSrc->width * cn + static_cast<int>(origin.x) * cn;
            for(int i = 0; i < Dst_height; i++, pDstData+= pDst->width * cn, pSrcData += pSrc->width * cn)
            {
                for(int j = 0; j < Dst_width; j++)
                {
                    *(pDstData + j * cn) = pSrcData[j * cn + 0];
                    *(pDstData + j * cn + 1) = pSrcData[j * cn + 1];
                    *(pDstData + j * cn + 2) = pSrcData[j * cn + 2];
                }
            }
        }
    }

    IplImage *pDstImage = cvCreateImage({pDst->width ,pDst->height },IPL_DEPTH_8U,CV_MAT_CN(pDst->type));
    cvConvert(pDst, pDstImage);    //深拷贝

    cvShowImage("pDstImage",pDstImage);
//    cvReleaseMat(&pDst);
    cvReleaseImage(&pDstImage);

}

/*****************************
 * 函数功能                     ：初始化放大倍数，以及缩放的次数（缩放次数是为了纠正放大倍数导致的错误，存在数据截留现象）
******************************/
inline void MainWindow::Initialize_Zoom()
{
    zoom = 1;
    zoom_num = 0;
    zoom_sum = 0;
}

/*****************************
 * 函数功能                     ：将显示图像上面的点转换到源图像上的点
 * pt                          ：显示图像上的点
 * CutOutRect                  ：抠图的矩形区域
******************************/
inline QPointF MainWindow::Show2CutOut(QPointF pt, QRectF *CutOutRect)
{
    qreal CutOutPtx = pt.x() * iuint_width + CutOutRect->x();
    qreal CutOutPty = pt.y() * iuint_height + CutOutRect->y();
    return {CutOutPtx,CutOutPty};
}

/*****************************
 * 函数功能                     ：检查放大倍数，并且求得相应的矩阵区域
 * CutOutRect                  ：抠图的矩形区域
******************************/
bool MainWindow::CheckZoom(QRectF *CutOutRect)
{

    qreal xp;
    qreal yp;
    qreal width;
    qreal height;
    qreal power;
    qreal x;
    qreal y;
    QPointF mouse_postion = Show2CutOut(mouse_global_position, CutOutRect);
    if(zoom_num == 0)
        return true;

    //如果缩小了，就放缩到相应的大小。如果放大了，就放大到对应的大小
    if(zoom_num < 0)
    {
        zoom *= pow(iwheel_scale,-zoom_num);
        while(zoom < 1)
        {
            zoom *= wheel_scale;
            zoom_num ++;
        }
    }
    else
    {
        zoom *= pow(wheel_scale,zoom_num);
        while(zoom > Src_width / 3 ||  zoom > Src_height /3)
        {
            zoom *= iwheel_scale;
            zoom_num --;
        }
    }

    if(zoom_num == 0)
        return true;

    //计算每次放缩的矩形区域
    if(zoom_num < 0)
    {
        xp = CutOutRect->x();
        yp = CutOutRect->y();
        width = CutOutRect->width();
        height = CutOutRect->height();
        power = pow(wheel_scale,-zoom_num);

        /******************************
         * 缩小公式如下
         * 新的坐标 = （1.1的n次方 * 原坐标位置 - （1.1的n次方 - 1) * 鼠标位置）
        **********************************/
        x = power * xp - (power -1) * mouse_postion.x();
        y = power * yp - (power -1) * mouse_postion.y();
        width *= power;
        height *= power;
        CutOutRect->setRect(x,y,width,height);
    }

    if(zoom_num > 0)
    {
        xp = CutOutRect->x();
        yp = CutOutRect->y();
        width = CutOutRect->width();
        height = CutOutRect->height();
        power = pow(wheel_scale,zoom_num);
//        x = power * xp - (power -1) * mouse_postion.x();
//        y = power * yp - (power -1) * mouse_postion.y();
        /******************************
         * 放大公式如下
         * 新的坐标 = （（1.1的n次方 -1) * 坐标位置 + 原坐标位置 ） / 1.1的n次方
        **********************************/
        x = (1 * xp + (power -1) * mouse_postion.x()) / power;
        y = (1 * yp + (power -1) * mouse_postion.y()) / power;
        width /= power;
        height /= power;
        CutOutRect->setRect(x,y,width,height);
    }

    //对缩放边界进行处理
    zoom_sum += zoom_num;
    //当然可以通过查表方式解决这个问题
    if( zoom_sum < 1)//当没有缩放次数时，CutOutRect 应该是初始化的值 （因为数据截留的原因，导致和原始值不一致）
    {
        zoom = 1;
        CutOutRect->setRect(0,0,Src_width,Src_height);
    }

    zoom_num =0;
    return true;
}

/*********************************
 * 函数功能                     ：检查移动后的矩阵
 * CutOutRect                  ：抠图的矩形区域
 * 同时将移动的位移置零
**********************************/
void MainWindow::CheckMove(QRectF *CutOutRect)
{
    if(mouse_down_[0] == true)
        CutOutRect->setRect(CutOutRect->x() - mouse_move.x() * iuint_width ,
                            CutOutRect->y() - mouse_move.y() * iuint_height ,
                            CutOutRect->width(),
                            CutOutRect->height());

    mouse_move.setX(0);
    mouse_move.setY(0);
}

/*********************************
 * 函数功能                     ：检查矩阵
 * CutOutRect                  ：抠图的矩形区域
 * 检查矩阵是否超过规定的矩形区域，如果超过了，就调节矩形区域
**********************************/
void MainWindow::CheckRect(QRectF *CutOutRect)
{
    if(CutOutRect->x() + CutOutRect->width()> pSrc->width)
        CutOutRect->setRect(pSrc->width - CutOutRect->width(),CutOutRect->y(),CutOutRect->width(),CutOutRect->height());
    if(CutOutRect->y() + CutOutRect->height()> pSrc->height)
        CutOutRect->setRect(CutOutRect->x(),pSrc->height - CutOutRect->height(),CutOutRect->width(),CutOutRect->height());
    if(CutOutRect->x() < 0)
        CutOutRect->setRect(0,CutOutRect->y(),CutOutRect->width(),CutOutRect->height());
    if(CutOutRect->y() < 0)
        CutOutRect->setRect(CutOutRect->x(),0,CutOutRect->width(),CutOutRect->height());


    CV_Assert(CutOutRect->x() >= 0);
    CV_Assert(CutOutRect->y() >= 0);
    CV_Assert(CutOutRect->x() + CutOutRect->width() <= pSrc->width);
    CV_Assert(CutOutRect->y() + CutOutRect->height()<= pSrc->height);

}

/*********************************
 * 函数功能                     ：鼠标按键事件
 * 如果按下左键，就记录左键的标志位，同时改变鼠标的形状
**********************************/
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if( event->buttons() == Qt::LeftButton )
    {
        //如果鼠标按下的是左键
        //则改变鼠标形状为手掌，表示拖动状态。
        QCursor cursor1;//创建光标对象
        cursor1.setShape(Qt::OpenHandCursor);//设置光标形态
        setCursor(cursor1);//使用手掌光标
        mouse_down_[0] = true;
    }

    if( event->buttons() == Qt::RightButton )
        mouse_down_[1] = true;
}

/*********************************
 * 函数功能                     ：鼠标释放事件
 * 将对应的按键信息清零
**********************************/
void MainWindow::mouseReleaseEvent(QMouseEvent*event)
{
    //拖动完成后，光标恢复默认形状
    QCursor cursor;//创建光标对象
    cursor.setShape(Qt::ArrowCursor);//设置光标形态
    setCursor(cursor);
    //或者直接用自带恢复鼠标指针形状的函数为：QApplication::restoreOverrideCursor();
    //但是需要前面设置哪个是默认的光标形状，用这个函数setOverrideCursor()函数

    if (event->button() == Qt::LeftButton)
        mouse_down_[0] = false;

    if (event->button() == Qt::RightButton)
        mouse_down_[1] = false;
}

//void MainWindow::QtShowMessege(QImage *image,QPointF pt)
//{

////    image_endx = image->width() + image_startx;
////    image_endy = image->height() + image_starty;


////    if(image_startx <= pt.x() && pt.x() < image_endx &&
////            image_starty <= pt.y() && pt.y() < image_endy )
////    {
////        QRgb rgb = image->pixel(QPoint(pt.x(),pt.y()));
////        int r = qRed(rgb);
////        int g = qGreen(rgb);
////        int b = qBlue(rgb);

////        QString R = QString::number(r, 10);
////        QString G = QString::number(g, 10);
////        QString B = QString::number(b, 10);

////    //        cv::Point2i point= {((pt.x() - image_startx) + 0.5) / uint_width ,((pt.y() - image_starty)
////    //                        + 0.5)/ uint_height};
////        cv::Point2i point= {pt.x() - image_startx,pt.y() - image_starty};

////        QString x = QString::number(point.x, 10);
////        QString y = QString::number(point.y, 10);

//////        char s1[40];                        // str的长度最少为3，因为10虽然两位，但还有一个结束符
//////        sprintf(s1,"%d",point.x);
//////        char s2[40];                        // str的长度最少为3，因为10虽然两位，但还有一个结束符
//////        sprintf(s2,"%d", point.y);

////        QString string1 = "(x=" + x + ",v=" + y + ") ~ R:" + R + " G:" + G + " B:" + B;

////    //        std::cout << "(x=" ;
////    //        std::cout << s1;
////    //        std::cout << ",v=" ;
////    //        std::cout <<s2 ;
////    //        std::cout <<")";
////    //        std::cout <<  "r=" << (int)r <<  ",g=" << (int)g << ",b=" << (int)b <<std::endl;
////        ui->statusBar->showMessage(string1);
////    }
////    else
////    {
////       ui->statusBar->clearMessage();
////    }
//}

/*********************************
 * 函数功能                       ：从图像中获得对应的像素值
 * image                         ：对应的图像位置
 * gray                         ：对应位置的灰度值
**********************************/
template <class T >
void MainWindow::QtGetImagePexel(CvMat *image,QPointF pt,T *gray)
{

    int cn = CV_MAT_CN(image->type);
    int depth = CV_MAT_DEPTH(image->type);
    assert(cn == 1);
    if(pt.x() >= image->width)
        pt.setX(image->width - 1);
    if(pt.y() >= image->height)
        pt.setY(image->height - 1);
    CV_Assert(static_cast<int>(pt.x()) <= image->width);
    CV_Assert(static_cast<int>(pt.y()) <= image->height);

    assert(depth == CV_8U ||depth == CV_32F || depth == CV_64F );

    if(depth == CV_8U)
    {
        assert(sizeof(T) == 1);
        uchar *pSrcData = image->data.ptr;
        *gray = *(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->step);
    }
    if(depth == CV_32F)
    {
        assert(sizeof(T) == 4);
        float *pSrcData = image->data.fl;
        *gray = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn));
    }
    if(depth == CV_64F)
    {
        double *pSrcData = image->data.db;
        assert(sizeof(T) == 8);
        *gray = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn));
    }
//    if(depth == CV_8U)
//    {
//        gray = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->step);
//    }
//    if(depth == CV_32F)
//    {
//        gray = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn);
//    }
//    if(depth == CV_64F)
//    {
//        gray = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn);

//    }
////    *rgb = image->pixelIndex(QPoint(pt.x(),pt.y()));
//    *rgb = gray;
}

/*********************************
 * 函数功能                       ：从图像中获得对应的像素
 * image                         ：对应的图像位置
 * B，G，R                        ：对应位置的像素
**********************************/
template <class T>
void MainWindow::QtGetImagePexel(CvMat *image,QPointF pt,T *B,T *G,T *R)
{
    int cn = CV_MAT_CN(image->type);
    int depth = CV_MAT_DEPTH(image->type);
    assert(cn == 3);
    if(pt.x() >= image->width)
        pt.setX(image->width - 1);
    if(pt.y() >= image->height)
        pt.setY(image->height - 1);
    assert(static_cast<int>(pt.x()) <= image->width);
    assert(static_cast<int>(pt.y()) <= image->height);
    assert(static_cast<int>(pt.x()) >= 0);
    assert(static_cast<int>(pt.y()) >= 0);

    assert(depth == CV_8U ||depth == CV_32F || depth == CV_64F );
    if(depth == CV_8U)
    {
        assert(sizeof(T) == 1);
        uchar *pSrcData = image->data.ptr;
        *R = *(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->step);
        *G = *(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->step + 1) ;
        *B = *(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->step + 2);
    }
    if(depth == CV_32F)
    {
        assert(sizeof(T) == 4);
        float *pSrcData = image->data.fl;
        *R = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn));
        *G = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn + 1)) ;
        *B = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn + 2));
    }
    if(depth == CV_64F)
    {
        double *pSrcData = image->data.db;
        assert(sizeof(T) == 8);
        *R = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn));
        *G = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn + 1)) ;
        *B = static_cast<T>(*(pSrcData + static_cast<int>(pt.x()) * cn + static_cast<int>(pt.y()) *  image->width * cn + 2));
    }
//    *rgb = image->pixelIndex(QPoint(pt.x(),pt.y()));
}

//void MainWindow::QtGetImagePexel(CvMat *image,QPointF pt,QRgb *rgb)
//{
//    int cn = CV_MAT_CN(image->type);
//    int depth = CV_MAT_DEPTH(image->type);
//    assert(cn == 3);
//    if(pt.x() >= image->width)
//        pt.setX(image->width - 1);
//    if(pt.y() >= image->height)
//        pt.setY(image->height - 1);
//    CV_Assert((int)pt.x() <= image->width);
//    CV_Assert((int)pt.y() <= image->height);
//    int B;
//    int G;
//    int R;
//    if(depth == CV_8U)
//    {
//        R = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->step);
//        G = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->step + 1) ;
//        B = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->step + 2);
//    }
//    if(depth == CV_32F)
//    {
//        R = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn);
//        G = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn + 1) ;
//        B = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn + 2);
//    }
//    if(depth == CV_64F)
//    {
//        R = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn);
//        G = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn + 1) ;
//        B = *(image->data.ptr + (int)pt.x() * cn + (int)pt.y() *  image->width * cn + 2);
//    }
////    *rgb = image->pixelIndex(QPoint(pt.x(),pt.y()));
//    *rgb = qRgb(R,G,B);
//}

//void MainWindow::QtGetImagePexel(QImage *image,QPointF pt,int *rgb)
//{
//    if(pt.x() >= image->width())
//        pt.setX(image->width() - 1);
//    if(pt.y() >= image->height())
//        pt.setY(image->height() - 1);
//    CV_Assert(pt.x() <= image->width());
//    CV_Assert(pt.y() <= image->height());
//    *rgb = image->pixelIndex(QPoint(pt.x(),pt.y()));
//}

//void MainWindow::QtGetImagePexel(QImage *image,QPointF pt,QRgb *rgb)
//{
//    if(pt.x() >= image->width())
//        pt.setX(image->width() - 1);
//    if(pt.y() >= image->height())
//        pt.setY(image->height() - 1);
//    CV_Assert(pt.x() <= image->width());
//    CV_Assert(pt.y() <= image->height());
//    *rgb = image->pixel(QPoint(pt.x(),pt.y()));
//}


/*********************************
 * 函数功能                       ：整型转换为Qstring型
 * Gray                         ：需要转换的值
**********************************/
void MainWindow::Int2String(int gray, QString *Gray)
{
    *Gray = QString::number(gray, 10);
}

/*********************************
 * 函数功能                       ：float型转换为Qstring型
 * gray                         ：需要转换的值
**********************************/
void MainWindow::Float2String(float gray, QString *Gray)
{
//    *Gray = QString::number(gray, 10);
    *Gray = QString::number(static_cast<double>(gray), 'f', 3);
}

/*********************************
 * 函数功能                       ：double型转换为Qstring型
 * gray                         ：需要转换的值
**********************************/
void MainWindow::Double2String(double gray, QString *Gray)
{
//    *Gray = QString::number(gray, 10);
    *Gray = QString::number(gray, 'd', 3);
}

/*********************************
 * 函数功能                        ：QRgb型转换为Qstring型
 * R，G，B                         ：需要转换的数据
**********************************/
void MainWindow::QRgb2String(QRgb rgb, QString *R, QString *G, QString *B)
{
    int r = qRed(rgb);
    int g = qGreen(rgb);
    int b = qBlue(rgb);

    *R = QString::number(r, 10);
    *G = QString::number(g, 10);
    *B = QString::number(b, 10);
}

/*********************************
 * 函数功能                             ：更新状态栏的信息
 * CutOutRect                         ：抠图的矩形区域
 * pt                                  ：鼠标的位置
**********************************/
void MainWindow::QtUpdateStatusBar(CvMat *pShow,QRectF *CutOutRect, QPointF pt)
{
    QString strShow;

    QPointF CutOut_pt = Show2CutOut(pt,CutOutRect);
    std::cout << "CutOut_pt->x() = " << CutOut_pt.x() << ",";
    std::cout << "CutOut_pt->y() = " << CutOut_pt.y() << std::endl;

//    ui->statusBar->showMessage(strShow);

    int cn = CV_MAT_CN(pShow->type);
    int depth = CV_MAT_DEPTH(pShow->type);

    int ir;
    int ig;
    int ib;
    int igray;

    float fr;
    float fg;
    float fb;
    float fgray;

    double dr;
    double dg;
    double db;
    double dgray;
    if(cn == 3)
    {
        if(depth == CV_8U)
        {
//            QtGetImagePexel(pShow,pt,&rgb);
            QtGetImagePexel(pShow,pt,(uchar*)&ir,(uchar*)&ig,(uchar*)&ib);
            QString R,G,B;
//            QRgb2String(rgb,&B,&G,&R);
            Int2String(ir,&R);
            Int2String(ig,&G);
            Int2String(ib,&B);

            strShow = QString("(x=%1,y=%2)")
                             .arg(static_cast<int>(CutOut_pt.x())).arg(static_cast<int>(CutOut_pt.y()));
            strShow += " ~" ;
            strShow += "R:" ;
            strShow += R;
            strShow += " G:" ;
            strShow += G;
            strShow += " B:" ;
            strShow += B;
        }

        if(depth == CV_32F)
        {
            QtGetImagePexel(pShow,pt,&fr,&fg,&fb);
            QString R,G,B;
            Float2String(fr,&R);
            Float2String(fg,&G);
            Float2String(fb,&B);

            strShow = QString("(x=%1,y=%2)")
                             .arg(static_cast<int>(CutOut_pt.x())).arg(static_cast<int>(CutOut_pt.y()));
            strShow += " ~" ;
            strShow += "R:" ;
            strShow += R;
            strShow += " G:" ;
            strShow += G;
            strShow += " B:" ;
            strShow += B;
        }

        if(depth == CV_64F)
        {
            QtGetImagePexel(pShow,pt,&dr,&dg,&db);
            QString R,G,B;
            Double2String(dr,&R);
            Double2String(dg,&G);
            Double2String(db,&B);

            strShow = QString("(x=%1,y=%2)")
                             .arg(static_cast<int>(CutOut_pt.x())).arg(static_cast<int>(CutOut_pt.y()));
            strShow += " ~" ;
            strShow += "R:" ;
            strShow += R;
            strShow += " G:" ;
            strShow += G;
            strShow += " B:" ;
            strShow += B;
        }
    }

    if(cn == 1)
    {
        if(depth == CV_8U)
        {
            QtGetImagePexel(pShow,pt,(uchar*)&igray);
            QString Gray;
            Int2String(igray,&Gray);

            strShow = QString("(x=%1,y=%2)")
                             .arg(static_cast<int>(CutOut_pt.x())).arg(static_cast<int>(CutOut_pt.y()));
            strShow += " ~" ;
            strShow += "G:" ;
            strShow += Gray;
        }

        if(depth == CV_32F)
        {
            QtGetImagePexel(pShow,pt,&fgray);
            QString Gray;
            Float2String(fgray,&Gray);

            strShow = QString("(x=%1,y=%2)")
                             .arg(static_cast<int>(CutOut_pt.x())).arg(static_cast<int>(CutOut_pt.y()));
            strShow += " ~" ;
            strShow += "G:" ;
            strShow += Gray;
        }

        if(depth == CV_64F)
        {
            QtGetImagePexel(pShow,pt,&dgray);
            QString Gray;
            Double2String(dgray,&Gray);

            strShow = QString("(x=%1,y=%2)")
                             .arg(static_cast<int>(CutOut_pt.x())).arg(static_cast<int>(CutOut_pt.y()));
            strShow += " ~" ;
            strShow += "G:" ;
            strShow += Gray;
        }
    }
    ui->statusBar->showMessage(strShow);
}
//void MainWindow::QtUpdateStatusBar(QImage *pShow,QRectF *CutOutRect, QPointF pt)
//{
//    QString strShow;
//    int gray;
//    QRgb rgb;

//    QPointF CutOut_pt = Show2CutOut(pt,CutOutRect);
//    std::cout << "CutOut_pt->x() = " << CutOut_pt.x() << ",";
//    std::cout << "CutOut_pt->y() = " << CutOut_pt.y() << std::endl;

////    ui->statusBar->showMessage(strShow);

//    int cn = pShow->format();
//    if(cn == QImage::Format_RGB888)
//    {
//        QtGetImagePexel(pShow,pt,&rgb);
//        QString R,G,B;
//        QRgb2String(rgb,&B,&G,&R);

//        strShow = QString("(x=%1,y=%2)")
//                         .arg((int)CutOut_pt.x()).arg((int)CutOut_pt.y());
//        strShow += " ~" ;
//        strShow += "R:" ;
//        strShow += R;
//        strShow += " G:" ;
//        strShow += G;
//        strShow += " B:" ;
//        strShow += B;
//    }

//    if(cn == QImage::Format_Indexed8)
//    {

//        QtGetImagePexel(pShow,pt,&gray);
//        QString Gray;
//        Int2String(gray,&Gray);

//        strShow = QString("(x=%1,y=%2)")
//                         .arg((int)CutOut_pt.x()).arg((int)CutOut_pt.y());
//        strShow += " ~" ;
//        strShow += "G:" ;
//        strShow += Gray;
//    }

//    ui->statusBar->showMessage(strShow);
//}

/*********************************
 * 函数功能                             ：鼠标移动事件
 * 记录鼠标移动的距离以及鼠标的坐标
**********************************/
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    mouse_move_statue = true;

    mouse_locate.setX(event->pos().x() - ui->label->x());
    mouse_locate.setY(event->pos().y() - ui->mainToolBar->height());//利用控件的相对位置

    QPointF Diff_pt = mouse_locate - last_pt;
    last_pt = mouse_locate;


    if(mouse_down_[0] == true)
    {
        task_update = true;
//        if(task_update == false)
//        {
//            task_update = true;
//            mouse_move.setX(0);
//            mouse_move.setY(0);
//        }
//        else
        {
            mouse_move.operator+=(Diff_pt);
        }
    }
    else
    {
        mouse_move.setX(0);
        mouse_move.setY(0);
    }
}

/****************************************
 * 函数功能                     ：滚轮事件
 * 每次鼠标往上滚，对应的值加一，往下滚则减一
*******************************************/
void MainWindow::wheelEvent(QWheelEvent*event)
{
    task_update =true;
    scale_flag = true;
    float num = (event->delta() / 120.0f);

    mouse_global_position.setX(event->pos().x() - ui->label->x());
    mouse_global_position.setY(event->pos().y() - ui->mainToolBar->height());//利用控件的相对位置

    if(mouse_global_position.x() >= 0 || mouse_global_position.y() >= 0)
    {
        QString strShow = QString("当前坐标：X:%1 Y:%2")
                        .arg(mouse_global_position.x()).arg(mouse_global_position.y());
            ui->statusBar->showMessage(strShow);
    }

    zoom_num += num;
}



/****************************************
 * 函数功能                     ：初始化图像
*******************************************/
void MainWindow::Initialize_Image(const IplImage *pSrcImage)
{
    Initialize_SrcImage(pSrcImage,&this->pSrc,&this->CutOutRect);//初始化pSrc
    ScreenRect.setRect(0,0,image_w,image_h);

}


/***********************************
 * 在 Label控件 上显示输入图像
 * 输入图像为灰度或者彩色
 * LabelImage                  :输入图像
***********************************/
void MainWindow::QtShowLabelImage(CvMat *LabelImage)
{
    QImage *pImage;
    uchar *imgData=LabelImage->data.ptr;
    int cn = CV_MAT_CN(LabelImage->type);
    if(cn == 3)
    {
        cvCvtColor(LabelImage,LabelImage,CV_BGR2RGB);//这个是颜色空间转换
        pImage=new QImage(imgData,LabelImage->width,LabelImage->height,((LabelImage->width * CV_MAT_CN(LabelImage->type)) ),
                      QImage::Format_RGB888);
    }

    if(cn == 1)
    {
        pImage=new QImage(imgData,LabelImage->width,LabelImage->height,((LabelImage->width * CV_MAT_CN(LabelImage->type)) ),
                          QImage::Format_Indexed8);
    }

    ui->label->setPixmap(QPixmap::fromImage(*pImage));
    ui->label->resize(ui->label->pixmap()->size());
    ui->label->show();
    ui->label->setAttribute(Qt::WA_TransparentForMouseEvents,true);
    delete pImage;
}

/***********************************
 * 初始化Param
 * task_update          ：任务更新标志位（ture 表示需要更新，false 表示不需要更新）
 * ready_flag           ：任务是否更新完成（ture 表示更新完成，false 表示更新未完成）
***********************************/
void MainWindow::Initialize_Param()
{
    task_update = false;
    ready_flag = true;
}

/**************************************************
 * 添加一条线
 * srcarr                           ：行的首地址
 * width                            ：行的宽度
 * Src_step                         ：每行的步长
 * width_stride                     ：每个元素之间的步长
 * rgb                              ：RGB通道
 * iLine_width                      ：线宽
*****************************************************/
void MainWindow::cvAddOneLine(uchar *pSrcData, int width, int Src_step,int width_stride ,int rgb ,int iLine_width)
{
    for(int k = 0; k < iLine_width; k++)
    {
        for(int j =0; j < width; j++)
        {
            *(pSrcData + k * Src_step + j * width_stride + rgb) = 0;
        }
    }
}

/**************************************************
 * 给图像添加行线
 * srcarr                           ：输入图像
 * sum_rows                         ：画线的行数
 * Src_width                        ：输入图像的宽度
 * Src_step                         ：输入图像每行的步长
 * uint_height                      ：每行之间的距离
 * cn                               ：通道数
 * iLine_width                      ：线宽
*****************************************************/
void MainWindow::cvAddRowsLine(uchar *pSrcData,int sum_rows, int Src_width, int Src_step, float uint_height ,int cn ,int iLine_width)
{
    if(cn == 3)
    {
        //这里不能表示为int(uint_height * Src_step)，会出大问题,出现
//        for(int i = 0; i < sum_rows; i++,pSrcData += (uint_height) * Src_step)
        for(int i = 0; i < sum_rows; i++)
        {
            cvAddOneLine(pSrcData + int(i * uint_height) * Src_step,Src_width,Src_step,cn,0,iLine_width);
            cvAddOneLine(pSrcData + int(i * uint_height) * Src_step,Src_width,Src_step,cn,1,iLine_width);
            cvAddOneLine(pSrcData + int(i * uint_height) * Src_step,Src_width,Src_step,cn,2,iLine_width);
        }
    }
    if(cn == 1)
    {
        //这里不能表示为int(uint_height * Src_step)，会出大问题,出现
//        for(int i = 0; i < sum_rows; i++,pSrcData += (uint_height) * Src_step)
        for(int i = 0; i < sum_rows; i++)
        {
            cvAddOneLine(pSrcData + int(i * uint_height) * Src_step,Src_width,Src_step,cn,0,iLine_width);
        }
    }
}


/**************************************************
 * 添加单列线
 * srcarr                           ：列的首地址
 * width                            ：列的宽度
 * Src_step                         ：每列的步长
 * width_stride                     ：每个元素之间的步长
 * rgb                              ：RGB通道
 * iLine_width                      ：线宽
*****************************************************/
void MainWindow::cvAddOneColLine(uchar *pSrcData, int width, int Src_step,int width_stride ,int rgb ,int iLine_width)
{
    for(int k = 0; k < iLine_width; k++)
    {
        for(int j =0; j < width; j++)
        {
            *(pSrcData + k * Src_step + j * width_stride + rgb) = 0;
        }
    }
}

/**************************************************
 * 给图像添加列线
 * srcarr                           ：输入图像
 * sum_cols                         ：画线的列数
 * Src_height                       ：输入图像的高度
 * Src_step                         ：输入图像每列的步长
 * uint_width                ：每列之间的距离
 * cn                               ：通道数
 * iLine_width                      ：线宽
*****************************************************/
void MainWindow::cvAddColsLine(uchar *pSrcData,int sum_cols, int Src_height, int Src_step, float uint_width ,int cn ,int iLine_width)
{
    if(cn == 3)
    {
        //这里不能表示为int(uint_height * Src_step)，会出大问题,出现
//        for(int i = 0; i < sum_cols; i++,pSrcData += (cn) * uint_width)
        for(int i = 0; i < sum_cols; i++)
        {
            cvAddOneLine(pSrcData + static_cast<int>(i * uint_width) * cn,Src_height,cn,Src_step,0,iLine_width);
            cvAddOneLine(pSrcData + static_cast<int>(i * uint_width) * cn,Src_height,cn,Src_step,1,iLine_width);
            cvAddOneLine(pSrcData + static_cast<int>(i * uint_width) * cn,Src_height,cn,Src_step,2,iLine_width);
        }
    }

    if(cn == 1)
    {
        //这里不能表示为int(uint_height * Src_step)，会出大问题,出现
//        for(int i = 0; i < sum_cols; i++,pSrcData += (cn) * uint_width)
        for(int i = 0; i < sum_cols; i++)
        {
           cvAddOneLine(pSrcData + static_cast<int>(i * uint_width) * cn,Src_height,cn,Src_step,0,iLine_width);
        }
    }
}

/**************************************************
 * 给图像添加行线和列线
 * srcarr                           ：输入图像
 * uint_width                ：子图像宽度对应到显示图像的单位像素数
 * uint_height               ：子图像高度对应到显示图像的单位像素数
 * iLine_width                      ：
*****************************************************/
void MainWindow::cvAddLine(CvArr * srcarr,qreal uint_width ,qreal uint_height,int iLine_width)
{
    CvMat *pSrc = (CvMat *)srcarr,src_stub;
    {
        pSrc = cvGetMat(srcarr,&src_stub);
        if(cvGetErrStatus()<0)
            cvError(CV_StsBackTrace,0,"inner function failed",__FILE__,__LINE__);
    }

    int Src_width = pSrc->width;
    int Src_height = pSrc->height;
    int Src_step = pSrc->step;
    int cn = CV_MAT_CN(pSrc->type);
    int Src_depth = CV_MAT_DEPTH((pSrc->type));

    CV_Assert(Src_width >= uint_width * 3);
    CV_Assert(Src_height >= uint_height * 3);
    CV_Assert(Src_depth == CV_8U);
    CV_Assert(cn == 3 || cn == 1);


    int square_number_width = Src_width / uint_width;
    int square_number_height = Src_height / uint_height;

    uchar *pSrcData = pSrc->data.ptr;

    //画行线
    cvAddRowsLine(pSrcData,square_number_height, Src_width, Src_step, uint_height ,cn ,iLine_width);

    //画列线
    pSrcData = pSrc->data.ptr;//必须对首地址重新赋值
    cvAddColsLine(pSrcData,square_number_width, Src_height, Src_step, uint_width,cn ,iLine_width);

    IplImage *pDstImage = cvCreateImage({Src_width,Src_height},IPL_DEPTH_8U,CV_MAT_CN(pSrc->type));
    cvConvert(pSrc, pDstImage);    //深拷贝
//    cvShowImage("AddLine",pDstImage);
}

///******************************************
// * 图像矩形区域添加文字
// * img                      ：输入图像
// * x                        ：图像的宽位置
// * y                        ：图像的高位置
// * r                        ：红色分量
// * g                        ：绿色分量
// * b                        ：蓝色分量
//*********************************************/
//void MainWindow::QtGetPixel(QImage img,int x, int y,int *Gray)
//{
//    *Gray = img.pixelIndex({x, y});


//    CV_Assert(*Gray <= 255);
//}

/******************************************
 * 图像矩形区域添加文字
 * img                      ：输入图像
 * x                        ：图像的宽位置
 * y                        ：图像的高位置
 * r                        ：红色分量
 * g                        ：绿色分量
 * b                        ：蓝色分量
*********************************************/
template <class T>
void MainWindow::cvGetPixel(const CvMat *img,int x, int y,T *r,T *g, T *b)
{
//    QRgb Rgb = img.pixel({x, y});


//    *r = qRed(Rgb);
//    *g = qGreen(Rgb);
//    *b = qBlue(Rgb);

//    CV_Assert(*r <= 255);
//    CV_Assert(*g <= 255);
//    CV_Assert(*b <= 255);

    int depth = CV_MAT_DEPTH(img->type);
    int cn = CV_MAT_CN(img->type);
    assert(depth == CV_8U ||depth == CV_32F || depth == CV_64F );
    if(depth == CV_8U)
    {
        assert(sizeof(T) == 1);
        uchar *pSrcData = img->data.ptr;
        *r = *(pSrcData + x * cn + y *  img->step);
        *g = *(pSrcData + x * cn + y *  img->step + 1) ;
        *b = *(pSrcData + x * cn + y *  img->step + 2);
    }
    if(depth == CV_32F)
    {
        assert(sizeof(T) == 4);
        float *pSrcData = img->data.fl;
        *r = static_cast<T>(*(pSrcData + x * cn + y *  img->width * cn));
        *g = static_cast<T>(*(pSrcData + x * cn + y *  img->width * cn + 1)) ;
        *b = static_cast<T>(*(pSrcData + x * cn + y *  img->width * cn + 2));
    }
    if(depth == CV_64F)
    {
        double *pSrcData = img->data.db;
        assert(sizeof(T) == 8);
        *r = static_cast<T>(*(pSrcData + x * cn + y *  img->width * cn));
        *g = static_cast<T>(*(pSrcData + x * cn + y *  img->width * cn + 1)) ;
        *b = static_cast<T>(*(pSrcData + x * cn + y *  img->width * cn + 2));
    }


}

/******************************************
 * 图像矩形区域添加文字,并且设置文字颜色
 * painter                  ：画图工具
 * x                        ：起点 宽
 * y                        ：起点 高
 * width                    ：矩形宽度
 * Height                   ：矩形高度
 * Rgb                      ：显示的像素值
 * pen                      ：画笔
 * color                    ：颜色
*********************************************/
template <class T>
void MainWindow::QtSetSquareText(QPainter *painter, int x, int y, float width,
                                 float Height, T Rgb, QPen *pen, QColor color)
{
    QString RGB;


    if(sizeof(T) == 1)//T 是char，float，还是double
        RGB = QString::number(static_cast<double>(Rgb));
    if(Rgb > 1)
    {
        if(sizeof(T) == 4)
            RGB = QString::number(static_cast<double>(Rgb), 'f', 1);
        if(sizeof(T) == 8)
            RGB = QString::number(static_cast<double>(Rgb), 'd', 1);
    }

    if(Rgb <= 1)
    {
        if(sizeof(T) == 4)
            RGB = QString::number(static_cast<double>(Rgb), 'f', 3);
        if(sizeof(T) == 8)
            RGB = QString::number(static_cast<double>(Rgb), 'd', 3);
    }
    pen->setColor(color);
    painter->setPen(*pen);
    painter->drawText(x, y,static_cast<int>(width),static_cast<int>(Height) , Qt::AlignCenter,RGB);
}


///******************************************
// * 图像矩形区域添加文字,并且设置文字颜色
// * painter                  ：画图工具
// * x                        ：起点 宽
// * y                        ：起点 高
// * width                    ：矩形宽度
// * Height                   ：矩形高度
// * Rgb                      ：显示的像素值
// * pen                      ：画笔
// * color                    ：颜色
//*********************************************/
//void MainWindow::QtSetSquareText(QPainter *painter, int x, int y, float width, float Height, int Rgb, QPen *pen, QColor color)
//{
//    QString RGB = QString::number(Rgb);
//    pen->setColor(color);
//    painter->setPen(*pen);
//    painter->drawText(x, y,width,Height , Qt::AlignCenter,RGB);
//}

/******************************************
 * 设置字体大小，形状
 * painter                  ：画图工具
 * fUint_pixel_col          ：单位列中的像素个数
 * fUint_pixel_row          ：单位行中的像素个数
*********************************************/
void MainWindow::QtSetFont(QPainter *painter,float fUint_pixel_col,float fUint_pixel_row)
{
    //设置字体
    QFont font = painter->font();
//    int font_size = (float)12 * MIN(fUint_pixel_col,fUint_pixel_row) / 32;
    int font_size = static_cast<int>(12 + static_cast<double>(MIN(fUint_pixel_col,fUint_pixel_row) - 34) /2.2) ;
    std::cout << font_size << std::endl;
    std::cout << MIN(fUint_pixel_col,fUint_pixel_row) << std::endl;
    font.setBold(false);//细字体
    font.setPixelSize(font_size);//改变字体大小
    painter->setFont(font);
}

/******************************************
 * 画出每个矩形框上面的文字
 * img                      ：输入图像
 * painter                  ：画图工具
 * x                        ：矩形列的位置
 * y                        ：矩形行的位置
 * width                    ：矩形的宽度（单位宽中的像素个数）
 * height                   ：矩形的高度（单位高中的像素个数）
*********************************************/
void MainWindow::QtSetSquare_Indexed8(const CvMat *img, QPainter *painter,int x, int y, float width, float height, QPen *pen)
{
    int r;
    int g;
    int b;

    float fr;
    float fg;
    float fb;

    double dr;
    double dg;
    double db;

    int cn = CV_MAT_CN(img->type);
    int depth = CV_MAT_DEPTH(img->type);
    assert(cn == 1);
    {
        if(depth == CV_8U)
        {
            cvGetPixel(img,static_cast<int>(width * float(x + 0.5)), static_cast<int>(height * float(y + 0.5)),(uchar*)&r,(uchar*)&g,(uchar*)&b);

            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y),
                            width, height, (uchar)g, pen, Qt::black);
        }

        if(depth == CV_32F)
        {

            cvGetPixel(img,static_cast<int>(width * float(x + 0.5)), static_cast<int>(height * float(y + 0.5)),&fr,&fg,&fb);

            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y),
                            width, height, fg, pen, Qt::black);
        }

        if(depth == CV_64F)
        {

            cvGetPixel(img,static_cast<int>(width * float(x + 0.5)), static_cast<int>(height * float(y + 0.5)),&dr,&dg,&db);

            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y),
                            width, height, dg, pen, Qt::black);
        }
    }


}

/******************************************
 * 画出每个矩形框上面的文字
 * img                      ：输入图像
 * painter                  ：画图工具
 * x                        ：矩形列的位置
 * y                        ：矩形行的位置
 * width                    ：矩形的宽度（单位宽中的像素个数）
 * height                   ：矩形的高度（单位高中的像素个数）
*********************************************/
void MainWindow::QtSetSquare(CvMat *img, QPainter *painter,int x, int y, float width, float height, QPen *pen)
{
    int r;
    int g;
    int b;

    float fr;
    float fg;
    float fb;

    double dr;
    double dg;
    double db;
    int cn = CV_MAT_CN(img->type);
    int depth = CV_MAT_DEPTH(img->type);
    assert(cn == 3);
    {
        if(depth == CV_8U)
        {
            cvGetPixel(img,static_cast<int>(width * float(x + 0.5)), static_cast<int>(height * float(y + 0.5)),(uchar*)&r,(uchar*)&g,(uchar*)&b);

            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y - height * 1/3),
                            width, height, (uchar)r, pen, Qt::red);
            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y),
                            width, height, (uchar)g, pen, Qt::green);
            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y + height * 1/3),
                            width, height, (uchar)b, pen, Qt::blue);
        }

        if(depth == CV_32F)
        {
            cvGetPixel(img,static_cast<int>(width * float(x + 0.5)), static_cast<int>(height * float(y + 0.5)),&fr,&fg,&fb);

            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y - height * 1/3),
                            width, height, fr, pen, Qt::red);
            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y),
                            width, height, fg, pen, Qt::green);
            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y + height * 1/3),
                            width, height, fb, pen, Qt::blue);
        }

        if(depth == CV_64F)
        {
            cvGetPixel(img,static_cast<int>(width * float(x + 0.5)), static_cast<int>(height * float(y + 0.5)),&dr,&dg,&db);

            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y - height * 1/3),
                            width, height, dr, pen, Qt::red);
            QtSetSquareText(painter, static_cast<int>(x * width),static_cast<int>(height * y),
                            width, height, dg, pen, Qt::green);
            QtSetSquareText(painter, static_cast<int>(x * width), static_cast<int>(height * y + height * 1/3),
                            width, height, db, pen, Qt::blue);
        }
    }
}

/******************************************
 * 在图像上添加文字
 * img                      ：输入图像
 * painter                  ：画图工具
 * height                   ：矩形块的个数（行方向）
 * width                    ：矩形块的个数（列方向）
 * fUint_pixel_col          ：单位列中的像素个数
 * fUint_pixel_row          ：单位行中的像素个数
*********************************************/
void MainWindow::QtDrawText(CvMat *img, QPainter *painter,int height, int width, float fUint_pixel_col,float fUint_pixel_row,QPen *pen)
{
    CV_Assert(height * fUint_pixel_row <= img->height);
    CV_Assert(width * fUint_pixel_col <= img->width);

    if(img_cn == 3)//这里不能改成cn
    {
        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
                QtSetSquare(img, painter,j , i, fUint_pixel_col, fUint_pixel_row, pen);
            }
        }
    }

    if(img_cn == 1)//这里不能改成cn
    {
        for(int i = 0; i < height; i++)
        {
            for(int j = 0; j < width; j++)
            {
//                painter->drawText(150, 150,150,150 , Qt::AlignCenter,"RGB");
//                QtSetSquare_Indexed8(img, painter,5 , 5, 50, 50, pen);
                QtSetSquare_Indexed8(img, painter,j , i, fUint_pixel_col, fUint_pixel_row, pen);
            }
        }
    }
}

/******************************************
 * 函数功能                      ：将灰度转换为RGB色彩空间
 * pRgb                         ：彩色图像
 * pGray                        ：灰度图像
*********************************************/
void MainWindow::cvRgb2Gray(CvMat *pRgb, CvMat *pGray)
{
    int gray_cn = CV_MAT_CN(pGray->type);
    int gray_width = pGray->width;
    int gray_height = pGray->height;
    int gray_step = pGray->step;

    int rgb_cn = CV_MAT_CN(pRgb->type);
    int rgb_width = pRgb->width;
    int rgb_height = pRgb->height;
    int rgb_step = pRgb->step;

    CV_Assert(gray_cn == 1 && rgb_cn == 3);
    CV_Assert(rgb_width == gray_width);
    CV_Assert(gray_height == rgb_height);

    uchar *pGrayData = pGray->data.ptr;
    uchar *pRgbData = pRgb->data.ptr;

    for(int i = 0; i < rgb_height; i++,pGrayData += gray_step, pRgbData += rgb_step)
    {
        for(int j = 0; j < rgb_width; j++)
        {
             pGrayData[j] = pRgbData[rgb_cn * j ];
        }
    }
}

/******************************************
 * 函数功能                      ：将灰度转换为RGB色彩空间
 * pGray                        ：灰度图像
 * pRgb                         ：彩色图像
*********************************************/
void MainWindow::cvGray2Rgb(const CvMat *pGray, CvMat *pRgb)
{
    int gray_cn = CV_MAT_CN(pGray->type);
    int gray_width = pGray->width;
    int gray_height = pGray->height;
    int gray_step = pGray->step;

    int rgb_cn = CV_MAT_CN(pRgb->type);
    int rgb_width = pRgb->width;
    int rgb_height = pRgb->height;
    int rgb_step = pRgb->step;

    CV_Assert(gray_cn == 1 && rgb_cn == 3);
    CV_Assert(rgb_width == gray_width);
    CV_Assert(gray_height == rgb_height);

    uchar *pGrayData = pGray->data.ptr;
    uchar *pRgbData = pRgb->data.ptr;

    for(int i = 0; i < rgb_height; i++,pGrayData += gray_step, pRgbData += rgb_step)
    {
        for(int j = 0; j < rgb_width; j++)
        {
            pRgbData[rgb_cn * j ] = pGrayData[j];
            pRgbData[rgb_cn * j + 1] = pGrayData[j];
            pRgbData[rgb_cn * j + 2] = pGrayData[j];
        }
    }
}

/******************************************
 * 函数功能                      ：清楚内存空间
*********************************************/
void MainWindow::clear()
{
    cvReleaseMat(&pSrc);
    pSrc = NULL;

    cvReleaseMat(&pShow_Copy);
    pShow_Copy = NULL;
//    cvReleaseMat(&pTemp);
//    pTemp = NULL;

//    cvReleaseMat(&pTemp_8U);
//    pTemp_8U = NULL;

//    cvReleaseMat(&pShow);
//    pShow = NULL;

//    cvReleaseMat(&pShow_8U);
//    pShow_8U = NULL;

//    if(img_cn == 1)
//    {
//        cvReleaseMat(&pGray2Rgb_8U);
//        pGray2Rgb_8U = NULL;

//        delete pGray2Rgb_8U_label;
//        pGray2Rgb_8U_label = NULL;
//    }

//    delete pShow_8U_label;
//    pShow_8U_label = NULL;

//    delete pTemp_8U_label;
//    pTemp_8U_label = NULL;

    delete update_timer;
    update_timer = NULL;
}
