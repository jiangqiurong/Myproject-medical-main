#ifndef _LCDDEVICE_H_
#define _LCDDEVICE_H_


#include <stdio.h>
#include <stdbool.h>
#include "pictrue.hpp"

//打开设备open，/dev/fb0,  映射mmap
//把lcd设计的到特征属性封装在一个结构体里面
class LcdDevice{
private:
    int w;
    int h;
    int bytepix;
    int lcdfd;
    unsigned int *mptr;


public:
    LcdDevice(const char *lcdname);
    ~LcdDevice();   //释放mptr和lcdDevice
    void drawBmp(struct PicImage* image,int x, int y);
    void drawColor(unsigned int color);
    void draw_line(int x0, int y0, int x1, int y1, unsigned int color);
    void draw_circle(int x0, int y0, int radius, unsigned int color);
};

#endif//_LCDDEVICE_H_