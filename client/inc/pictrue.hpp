#ifndef _PICTRUE_H_
#define _PICTRUE_H_

#include <stdio.h>
#include <stdbool.h>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int  uint32;

#pragma pack(1) //设置1字节对齐
typedef struct BitFileHeader{
    uint16 bfType;//位图类别，根据不同的操作系统而不同，在Windows中，此字段的值总为‘BM’BMP
    uint32 bfSize;//BMP图像文件的大小
    uint16 bfReserved1; 
    uint16 bfReserved2;
    uint32 bfOffBits;//BMP图像数据的地址
    uint32 biSize; //表示本结构的大小
    uint32 biWidth; //位图的宽度
    uint32 biHeight; //位图的高度
    uint16 biPlanes; 
    uint16 biBitCount;//BMP图像的色深，即一个像素用多少位表示，常见有1、4、8、16、24和32，分别对应单色、16色、256色、16位高彩色、24位真彩色和32位增强型真彩色
    uint32 biCompression; //压缩方式，0表示不压缩，1表示RLE8压缩，2表示RLE4压缩，3表示每个像素值由指定的掩码决定
    uint32 biSizeImage; //BMP图像数据大小，必须是4的倍数，图像数据大小不是4的倍数时用0填充补足
    uint32 biXPelsPerMeter; //水平分辨率，单位像素/m
    uint32 biYPelsPerMeter; //垂直分辨率，单位像素/m
    uint32 biClrUsed; //BMP图像使用的颜色，0表示使用全部颜色，对于256色位图来说，此值为100h=256
    uint32 biClrImportant;//重要的颜色数，此值为0时所有颜色都重要，对于使用调色板的BMP图像来说，当显卡不能够显示所有颜色时，此值将辅助驱动程序显示颜色
} BitHeader;
#pragma pack(0)//设置为默认字节对齐

class PicImage{
private:
    int w;
    int h;
    int bytepix;
    unsigned char *map;
public:
    int Get_H()
    {
        return h;
    }
    int Get_W()
    {
        return w;
    }
    int Get_Bytepix()
    {
        return bytepix;
    }
    unsigned char* Get_Map()
    {
        return map;
    }
    PicImage(const char *filename);
    ~PicImage();

};


#endif//_PICTRUE_H_
