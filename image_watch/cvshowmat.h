#ifndef CVSHOWMAT_H
#define CVSHOWMAT_H
#include "mainwindow.h"
#include <cv.h>
#include <highgui.h>
#include <vector>
#include <memory>

class cvShowMat
{
public:
    cvShowMat(char *name,const IplImage *);
    class MainWindow *w;
};

extern std::vector<cvShowMat> vec;
void cvShow(char *picture_name, IplImage *pImage);
#endif // CVSHOWMAT_H
