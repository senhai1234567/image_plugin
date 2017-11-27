#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTimer>
#include <QLabel>
#include <cv.h>
#include <highgui.h>
#include <QMouseEvent>
#include <QPainter>

enum depth//跟第一个定义不同的是，此处的标号DAY省略，这是允许的。
{
    cv_8u,
    cv_32f,
    cv_64f,
} ; //变量workday的类型为枚举型enum DAY

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    const IplImage *pImage;

    void Initialize(void);
    void clear();
    char *name;

private:
    Ui::MainWindow *ui;

    void Initialize_Timers(int delay);
    void Initialize_Window(char *);
    void Initialize_SrcImage(const CvArr *,CvMat**,QRectF* );
    void Initialize_Statusbar();
    inline void Initialize_Zoom();
    void Initialize_ScreenImage(CvMat **pShow_8U,CvMat **pTemp,CvMat **pShow,CvMat **pGray2Rgb,CvMat **pTemp_8U);

    void Initialize_Image(const IplImage *pSrcImage);
    void Initialize_LabelImage(CvMat *pShow,QImage *LabelImage);
    void Initialize_Param();
    void Initialize_mouse();

    //在图像里面添加黑线
    void cvAddOneLine(uchar *pSrcData, int width, int Src_step,int width_stride ,int rgb ,int iLine_width);
    void cvAddRowsLine(uchar *pSrcData,int sum_rows, int Src_width, int Src_step, float uint_height ,int cn ,int iLine_width);
    void cvAddColsLine(uchar *pSrcData,int sum_cols, int Src_height, int Src_step, float uint_width ,int cn ,int iLine_width);
    void cvAddLine(CvArr * srcarr,qreal uint_width ,qreal uint_height,int iLine_width);
    void cvAddOneColLine(uchar *pSrcData, int width, int Src_step,int width_stride ,int rgb ,int iLine_width);

    //因为qt里面没有专门用于灰度图像的qpainter，所以先转到RGB 空间，然后再转回来
    void cvGray2Rgb(const CvMat *pGray, CvMat *pRgb);
    void cvRgb2Gray(CvMat *pRgb, CvMat *pGray);

//    //设置文字大小
    void QtSetFont(QPainter *painter,float fUint_pixel_col,float fUint_pixel_row);

    void QtDrawText(CvMat *img, QPainter *painter,int height, int width, float fUint_pixel_col,float fUint_pixel_row,QPen *pen);

    void QtUpdateStatusBar(CvMat *pShow,QRectF *CutOutRect, QPointF pt);

    /***********************************
     * 能够实现在 Label控件 上显示输入图像
     * 输入图像为灰度或者彩色
    LabelImage                  :输入图像
    ***********************************/
    void QtShowLabelImage(CvMat *LabelImage);

    inline QPointF Show2CutOut(QPointF pt, QRectF *CutOutRect);

    template <class T>
    void QtGetImagePexel(CvMat *image,QPointF pt,T *B,T *G,T *R);
    template <class T >
    void QtGetImagePexel(CvMat *image,QPointF pt,T *gray);

    void Float2String(float gray, QString *Gray);
    void Int2String(int gray, QString *Gray);
    void Double2String(double gray, QString *Gray);

    void QRgb2String(QRgb rgb, QString *R, QString *G, QString *B);
    inline void Scale_Pixel();
    void cvGetSubImage(const CvMat *,CvMat *,QRectF *);
    void cvGetROI(CvMat *,CvMat *,QRectF *,qreal uint_width,qreal uint_height);
    void CheckRect(QRectF *);
    void CheckMove(QRectF *);
    bool CheckZoom(QRectF *);

    template <class T>void cvGetPixel(const CvMat *img,int x, int y,T *r,T *g, T *b);
    void QtSetSquare(CvMat *img, QPainter *painter,int x, int y,
                     float width, float height, QPen *pen);
    void QtSetSquare_Indexed8(const CvMat *img, QPainter *painter,int x, int y,
                     float width, float height, QPen *pen);

    template <class T>
    void QtSetSquareText(QPainter *painter, int x, int y,
              float width, float Height, T Rgb, QPen *pen, QColor color);

    virtual void mousePressEvent(QMouseEvent *event);//按下
    virtual void mouseReleaseEvent(QMouseEvent *event);//松开
    virtual void mouseMoveEvent(QMouseEvent *event);//移动
//    virtual void mouseDoubleClickEvent(QMouseEvent *event);//双击
    virtual void wheelEvent(QWheelEvent *event);//滚轮
    void CvMatToQImage(CvMat *img,QImage *image);

    CvMat *pSrc;
    CvMat *pShow_Copy;

    QTimer *update_timer;

    QRectF CutOutRect;//扣图的大小,位置
    QRectF ScreenRect;//坐标位置


    qreal uint_width;
    qreal uint_height;
    qreal iuint_width;
    qreal iuint_height;

    QPointF last_pt;

    // 鼠标事件包含了全局坐标与本地坐标，在此只需要本地坐标即可
    QPointF mouse_locate;
    QPointF mouse_global_position;
    QPointF mouse_move;//鼠标移动次数

    //for mouse
    bool mouse_down_[2];
    bool scale_flag;
    bool task_update;
    bool ready_flag;
    bool mouse_move_statue;
    int zoom_num;//每次鼠标的缩放次数
    qreal zoom;//总的缩放大小
    int zoom_sum;//记录总的缩放次数，理论上可以通过zoom来检查。但是zoom是双精度型，double型进行运算时，可能会出现数据截留现象，导致数据的小数位出问题

    double Src_width ;
    double Src_height;
    int img_cn;
    int img_depth;
    int image_endx ;
    int image_endy ;

public Q_SLOTS:
    void Loop();


};

#endif // MAINWINDOW_H
