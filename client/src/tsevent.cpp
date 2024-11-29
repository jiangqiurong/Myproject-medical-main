#include "../inc/tsevent.hpp"

//创建触摸屏
TsDev::TsDev(const char *tsname){
    fd = open(tsname, O_RDWR);
    if(fd < 0)
    {
        perror("open");
    }
}
//销毁触摸屏
TsDev::~TsDev(){
    close(fd);
}

//读取坐标值
Point TsDev::readXY(){
    Point p={0,0};
    if(fd < 0) return p;

    struct input_event evt;
    while(1){
        int size = read(fd, &evt, sizeof(evt));
        if(EV_KEY == evt.type && BTN_TOUCH == evt.code && evt.value>0){

        }
        if(EV_KEY == evt.type && BTN_TOUCH == evt.code && evt.value==0){
            break;
        }
        //x坐标
        if(EV_ABS == evt.type)
        {
            if(evt.code == ABS_X){
                p.x = evt.value*800/1024;
            }
            if(evt.code == ABS_Y)
            {
                p.y = evt.value*480/600;
            }
        }
    }
    return p;
}
