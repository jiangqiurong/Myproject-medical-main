#ifndef _TSEVENT_H
#define _TSEVENT_H

#include <stdio.h>
#include <linux/input.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
typedef struct Point{
    int x,y;
} Point;
class TsDev{
private:
    int fd;
public:
    TsDev(const char *filename);
    ~TsDev();
    Point readXY();
};
//创建触摸屏
struct TsDev* createTs(const char *tsname);
//销毁触摸屏
bool destroyTs(struct TsDev* ts);
//读取坐标值
Point readXY(struct TsDev* ts);


#endif
