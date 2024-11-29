#ifndef __FONT_HPP__
#define __FONT_HPP__
#include <iostream>

extern "C" {
    #include "font.h"
    #include <unistd.h>
}

using namespace std;

class MyFont
{
private:
    font *f;
    bitmap *bm;
    int fd_lcd ;
    u8 * map ;


public:
    MyFont( int Pixel = 25 , string ttf = "/usr/share/fonts/alimamafangyuantivf-thin.ttf" )
    // MyFont( int Pixel = 30 , string ttf = "../lib/font/simfang.ttf" )
    {
        //打开字体	 可以根据自己的喜好从Windows中选择字体注意必须是ttf格式
	    f = fontLoad( const_cast<char *>( ttf.c_str() ));
	  
        if (f == NULL) {
        perror("fontLoad failed");
        cout << "Font path: " << ttf << endl;
        return;
        }

        //字体大小的设置
        fontSetSize(f,Pixel);

        init_lcd("/dev/fb0");

        bm = new bitmap;
        bm->byteperpixel = 4 ;
        bm->height = 400 ;
        bm->width = 800 ;
        bm->map = map ;

        // cout << "Font loaded and initialized successfully!" << endl;

    }

    ~MyFont()
    {
        fontUnload(f) ;
        munmap(map,800*480*4);
        close(fd_lcd);
	    cout << __LINE__ << endl;
    }

    //初始化Lcd
    void init_lcd(const char *device)
    {
   
        //1打开设备
        fd_lcd = open(device, O_RDWR);
        if(fd_lcd < 0)
        {
            cout << "open device error" << endl;
            exit(0);
        }
        
        //映射
        map = (u8*)mmap(NULL,800*480*4,PROT_READ|PROT_WRITE,MAP_SHARED,fd_lcd,0);

        return ;
    }


    //将字体写到点阵图上
	void FontPrint(int x, int y ,string msg, int r, int g, int b)
    {
        fontPrint(f,bm , x , y , const_cast<char *>( msg.c_str()) , getColor(0,r,g,b) ,0) ;
    }
    
    void Show_font_to_lcd( struct LcdDevice_font* lcd , int x ,int y )
    {
        show_font_to_lcd(lcd->mp ,x , y ,bm);
    }


};





#endif