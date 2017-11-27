#include "cvshowmat.h"
std::vector<cvShowMat> vec;

void cvShow(char *picture_name, IplImage *pImage)
{
//    for(int i=0;i<2;i++)
//    {
//    //        char str[10];
//        char *str = (char *)malloc(sizeof(char) * 10);
//        sprintf(str,"%d",i);
//        cvShowMat show(str,pSrcImage);
//        vec.push_back(show);
//    }

    int count = vec.size();
    for (int i = 0; i < count;i++)
    {
        MainWindow *w = vec[i].w;

        char *name = w->name ;
        if(picture_name == name)
        {
            w->clear();
            w->pImage = pImage;
            w->Initialize();
            return ;
        }
    }

    cvShowMat show(picture_name,pImage);
    vec.push_back(show);

}
cvShowMat::cvShowMat(char *name,const IplImage *pSrcImage)
{
//    MainWindow w;

    this->w = new MainWindow;
    this->w->name = name;
    this->w->pImage = pSrcImage;
    this->w->Initialize();
    this->w->show();

}
