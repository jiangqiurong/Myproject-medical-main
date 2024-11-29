#ifndef _YEMIAN1_H_
#define _YEMIAN1_H_

#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "../inc/lcddevice.hpp"
#include "../inc/tsevent.hpp"
#include "../inc/client.hpp"
#include "../inc/login.hpp"
#include "../inc/font.hpp"

#define TTF1 "/usr/share/fonts/alimamafangyuantivf-thin.ttf"
#define TTF2 "/usr/share/fonts/alimamafangyuantivf-thin.ttf"
#define TTF3 "/usr/share/fonts/alimamafangyuantivf-thin.ttf"
#define TTF4 "/usr/share/fonts/alimamafangyuantivf-thin.ttf"
#define TTF5 "/usr/share/fonts/alimamafangyuantivf-thin.ttf"
// #define TTF1 "../lib/font/simfang.ttf"
// #define TTF2 "/usr/share/fonts/simfang.ttf"
// #define TTF3 "/usr/share/fonts/simfang.ttf"
// #define TTF4 "../lib/font/simfang.ttf"

extern string keshi_memory[9];
extern int choice;
extern bool over;

void yemian2(class LcdDevice *lcd, class TsDev *ts, class Client *client);
void show_keshi_font();
void yemian21(class LcdDevice *lcd, class TsDev *ts, class Client *client);
void yemian22(class LcdDevice *lcd, class TsDev *ts, class Client *client);
void yemian23(class LcdDevice *lcd, class TsDev *ts, class Client *client);
void number_font();

#endif
