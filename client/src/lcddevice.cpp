#include "../inc/lcddevice.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/fb.h>  //vim /usr/include/linux/fb.h 


//创建lcd
LcdDevice::LcdDevice(const char *lcdname){
    printf("lcd初始化\n");  
    // struct LcdDevice *lcd = (struct LcdDevice*)malloc(sizeof(struct LcdDevice));
    // if(NULL == lcd) return NULL;
    //打开设备文件
    lcdfd = open(lcdname, O_RDWR);
    if(lcdfd < 0){
        perror("lcd open");
        //free(lcdfd);
    }
    //从驱动获取设备信息 --宽， 高， 色深
    struct fb_var_screeninfo  vinfo;
    int ret = ioctl(lcdfd, FBIOGET_VSCREENINFO, &vinfo);
    if(ret < 0)
    {
        perror("ioctl err");
    }
    w = vinfo.xres;
    h = vinfo.yres;
    bytepix = vinfo.bits_per_pixel/8;

    //映射
    mptr = (unsigned int*)mmap(NULL, w*h*bytepix,
                    PROT_READ|PROT_WRITE,MAP_SHARED,lcdfd, 0);
    if(mptr == MAP_FAILED)
    {
        perror("mmap err");
    }
    printf("lcd初始化完成\n");
}
//销毁lcd
LcdDevice::~LcdDevice(){
     printf("lcd销毁\n");
     //1.释放映射空间
     munmap(mptr, w*h*bytepix);
     //2.关闭设备
     close(lcdfd);
}

//在lcd绘制图片
void LcdDevice::drawBmp(class PicImage *image,int x, int y)
{
    //5.把数据显示在lcd上
    int image_w = image->Get_W();
    int image_h = image->Get_H();
    int image_bytepix = image->Get_Bytepix();
    unsigned char *image_map = image->Get_Map();

    unsigned int *p = mptr;
    p = p+y*w+x;
    unsigned char *q = image_map;
    for(int i=0; i<image_h; ++i)
    {
        for(int j=0; j<image_w; ++j)
        {
            unsigned int argbdata = 0;
            argbdata = (0xFF << 24) | (q[2+j*3] << 16) | (q[1+j*3] << 8) | q[0+j*3];
            p[j] = argbdata;
        }
        q += image_w*image_bytepix;
        p += w;
    }
}

//绘制颜色
void LcdDevice::drawColor(unsigned int color){
    for(int i=0; i< w*h; ++i)
    {
        mptr[i] = color;
    }
}

//直线
void LcdDevice::draw_line(int x0, int y0, int x1, int y1, unsigned int color)
{
    
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while(1)
    {
        mptr[y0 * w + x0] = color;
        if(x0 == x1 && y0 == y1) break;
        
        int e2 = 2 * err;
        if(e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if(e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
    
}

//圆形
void LcdDevice::draw_circle(int x0, int y0, int radius, unsigned int color)
{
    
    int x = radius;
    int y = 0;
    int err = 0;
    while(x >= y)
    {
        mptr[(y0 + y) * w + (x0 + x)] = color;
        mptr[(y0 + y) * w + (x0 - x)] = color;
        mptr[(y0 - y) * w + (x0 + x)] = color;
        mptr[(y0 - y) * w + (x0 - x)] = color;
        mptr[(y0 + x) * w + (x0 + y)] = color;
        mptr[(y0 + x) * w + (x0 - y)] = color;
        mptr[(y0 - x) * w + (x0 + y)] = color;
        mptr[(y0 - x) * w + (x0 - y)] = color;

        if(err <= 0)
        {
            y ++;
            err += 2 * y + 1;
        }
        if(err > 0)
        {
            x --;
            err -= 2 * x + 1;
        }
    }
    
}