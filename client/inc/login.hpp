#ifndef _LOGIN_H_
#define _LOGIN_H_

#include "../inc/lcddevice.hpp"
#include "../inc/pictrue.hpp"
#include "../inc/tsevent.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

extern char username[32];
extern char password[32];


extern int huiyuan;

void login(class TsDev *ts, class LcdDevice *lcd, int tcpsocket);
int GetNum(class TsDev *ts);

#endif