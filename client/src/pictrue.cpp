#include "../inc/pictrue.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

//创建一个图片

PicImage::PicImage(const char *filename){
    //打开图片， 读取头， 读取数据，
    int picfd = open(filename,  O_RDONLY);
    BitHeader  bmpheader;
    read(picfd, &bmpheader,  54);

    //初始化图片宽高
    w = bmpheader.biWidth;
    h = bmpheader.biHeight;
    bytepix = bmpheader.biBitCount/8;

    int  num = (bmpheader.biWidth%4) ;//文件存储时候要补充的字节数
    int  pixnum = bmpheader.biWidth*bmpheader.biBitCount/8; //一行像素的字节数
    unsigned char filelinebuffer[num+ pixnum];  //从文件读取一行数据的空间

    //申请堆空存储像素数据
    map = new unsigned char[pixnum*bmpheader.biHeight];
    for(int i=bmpheader.biHeight-1; i>=0; --i){
        //读取一行数据
        read(picfd, filelinebuffer, num+ pixnum);
        memcpy(map+pixnum*i, filelinebuffer, pixnum); //把一行行像素拷贝到rgbdata中
    }
}


PicImage::~PicImage(){
    delete map;
}

// struct PicImage*  createBmp(const char *filename){
//     struct PicImage* image = (struct PicImage*)malloc(sizeof(struct PicImage));//申请结构体空间

//     //打开图片， 读取头， 读取数据，
//     int picfd = open(filename,  O_RDONLY);
//     BitHeader  bmpheader;
//     read(picfd, &bmpheader,  54);

//     //初始化图片宽高
//     image->w = bmpheader.biWidth;
//     image->h = bmpheader.biHeight;
//     image->bytepix = bmpheader.biBitCount/8;

//     int  num = (bmpheader.biWidth%4) ;//文件存储时候要补充的字节数
//     int  pixnum = bmpheader.biWidth*bmpheader.biBitCount/8; //一行像素的字节数
//     unsigned char filelinebuffer[num+ pixnum];  //从文件读取一行数据的空间

//     //申请堆空存储像素数据
//     image->map = (unsigned char*)malloc(pixnum*bmpheader.biHeight);
//     for(int i=bmpheader.biHeight-1; i>=0; --i){
//         //读取一行数据
//         read(picfd, filelinebuffer, num+ pixnum);
//         memcpy(image->map+pixnum*i, filelinebuffer, pixnum); //把一行行像素拷贝到rgbdata中
//     }

//     return image;
// }
//销毁一个图片
// bool  destroyBmp(struct PicImage* image){
//     if(image == NULL) return false;
//     free(image->map);
//     free(image);
//     return true;
// }
