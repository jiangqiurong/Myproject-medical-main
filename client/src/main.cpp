#include <iostream>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include <string>
#include "../inc/lcddevice.hpp"
#include "../inc/tsevent.hpp"
#include "../inc/client.hpp"
#include "../inc/login.hpp"
#include "../inc/font.hpp"
#include "../inc/yemian1.hpp"
#include<semaphore.h>

#include <sstream>  
#include <iomanip> // 用于 std::fixed 和 std::setprecision

using namespace std;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
class Client *client;

extern "C"
{
    #include "../inc/font.h"
}

void drawhome(class LcdDevice *lcd)
{
    class PicImage *home = new PicImage("bmp/home.bmp");
    class PicImage *guahao = new PicImage("bmp/guahao.bmp");
    class PicImage *daying = new PicImage("bmp/daying.bmp");
    class PicImage *goumai = new PicImage("bmp/goumai.bmp");
    class PicImage *jieshao = new PicImage("bmp/jieshao.bmp");
    class PicImage *qwe = new PicImage("bmp/bh.bmp");

    lcd->drawBmp(home, 0, 0);
    lcd->drawBmp(guahao, 108, 180);
    lcd->drawBmp(goumai, 268, 180);
    lcd->drawBmp(daying, 108, 340);
    lcd->drawBmp(jieshao, 268, 340);

    delete home;
    delete guahao;
    delete daying;
    delete goumai;
    delete jieshao;
}

int get_home_num(Point p)
{
    if(p.x >= 80 && p.x <= 200 && p.y >= 140 && p.y <= 270) return 1;
    if(p.x >= 240 && p.x <= 360 && p.y >= 140 && p.y <= 270) return 2;
    if(p.x >= 80 && p.x <= 200 && p.y >= 300 && p.y <= 430) return 3;
    if(p.x >= 240 && p.x <= 360 && p.y >= 300 && p.y <= 430) return 4;
    if(p.x >= 0 && p.x <= 40 && p.y >= 0 && p.y <= 40) return 5; 
}

struct yaowu_info{
    string name;                //服务器发来
    int num;                    //服务器发来
    double price;               //服务器发来
    int image_x;
    int image_y;
    int font_x;
    int font_y;                 
    int number_x;               //
    class PicImage *yao;        //服务器发来，然后本地保存，随后打开
    int price_x;
    int price_y;
    int num_x;
    int num_y;
    string price_string;
    string num_string;
}medicine[7];

typedef struct {  
    char name[100]; // 假设药名字不会超过99个字符（为'\0'留一个位置）  
    int num;  
    double price;  
} Medicine;

std::string doubleToStringWithPrecision(double value, int precision) {  
    std::ostringstream oss;  
    oss << std::fixed << std::setprecision(precision) << value;  
    return oss.str();  
}

int medicine_count[6] = {0};
Point show_num_font[6] = {{90, 140}, {290, 140}, {510, 140}, 
            {90, 320}, {290, 320}, {510, 320}};


bool rein = false;
bool rein1 = false;

struct user_info
{
    char username[32];
    char t[32];     //日期
    char type[32];      //挂号或者购药
    char money[32];     //金额
    char state[32];     //发票状态
    int nums[6];        //药品数量
}user_send_info;

void init_info(double money)
{
    for(int i = 0; i < 6; i ++ )
        user_send_info.nums[i] = medicine_count[i];

    string str = "购药";
    strcpy(user_send_info.type, str.c_str());

    str = "未开票";
    strcpy(user_send_info.state, str.c_str());

    str = doubleToStringWithPrecision(money, 2);
    strcpy(user_send_info.money, str.c_str());

    strcpy(user_send_info.username, username);
    //事件发到服务器后，服务器自己填上
}

void yemian33(class LcdDevice *lcd, class TsDev *ts)
{
    class PicImage *yaowuzhifu = new PicImage("yaowu/yaowuzhifu.bmp");
    lcd->drawBmp(yaowuzhifu, 0, 0);

    MyFont font1(22, TTF1);
    MyFont font2(32, TTF1);

    Point temp[6] = {{120,90}, {120, 130}, {120, 170}, {120, 210}, {120, 250}, {120, 290}};
    string str[6];
    int idx = 0;
    double sum = 0;

    for(int i = 0; i < 6; i ++ )
    {
        if(medicine_count[i])
        {
            str[i] = medicine[i + 1].name + " x " + to_string(medicine_count[i]) + " = " + 
            doubleToStringWithPrecision(medicine_count[i] * medicine[i + 1].price, 2);
            sum += medicine_count[i] * medicine[i + 1].price;
        }
        else str[i] = "";
    }
    for(int i = 0; i < 6; i ++ )
    {
        if(str[i] != "")
        {
            font1.FontPrint(temp[idx].x, temp[idx].y, str[i], 0, 0, 0);
            idx ++ ;
        }
    }

    string res = "扫描二维码付款";
    font2.FontPrint(370, 50, res, 0, 0, 0);
    res = doubleToStringWithPrecision(sum, 2) + " 元";
    font2.FontPrint(405, 95, res, 255, 0, 0);
    res = "返回";
    font1.FontPrint(20, 24, res, 255, 255, 255);
    
    while(1)
    {
        Point p = ts->readXY();

        if(p.x <= 80 && p.y <= 80)
        {
            return ;
        }
        else if(p.x >= 400 && p.x <= 550 && p.y >= 140 && p.y <= 280)
        {
            //确定支付，发信息给服务器, 发medicine_count数组, 看看txt中数量是否足够。
            //不够则提示购买失败。否则提示购买成功。随后显示页面4
            //先发送 "goumai", 进入条件
            char temp[16] = {"goumai"};
            write(client->Get_Socket(), temp, sizeof(temp));
            //发送大结构体
            init_info(sum);
            write(client->Get_Socket(), &user_send_info, sizeof(user_send_info));
            
            char buf[16] = {0};     
            read(client->Get_Socket(), buf, sizeof(buf));
            if(strcmp(buf, "success") == 0)
            {
                puts("success");
                //跳转到购买成功页面
                lcd->drawColor(0xffffff);
                string str = "购买成功, 请到7栋3楼药室拿药。";
                MyFont font(33, TTF1);
                font.FontPrint(200, 210, str, 0, 0, 0);
                Point p = ts->readXY();
                rein1 = true;
                return ;
            }
            else if(strcmp(buf, "fail") == 0)
            {
                puts("fail");
                //提示购买失败，商品数量不足。
                lcd->drawColor(0xffffff);
                string str = "购买失败, 药物数量不足。";
                MyFont font(33, TTF1);
                font.FontPrint(220, 210, str, 0, 0, 0);
                Point p = ts->readXY();
                return ;
            }
            else puts("王德法??");
        }
    }
}

void yemian3(class LcdDevice *lcd, class TsDev *ts)
{
    //先从服务器获得一下数据
    char buf[16] = {"shuaxin"};
    write(client->Get_Socket(), buf, sizeof(buf));

    Medicine temp[6] = {0};
    read(client->Get_Socket(), temp, sizeof(temp));
    for(int i = 0; i < 6; i ++ )
    {
        medicine[i + 1].num = temp[i].num;
        medicine[i + 1].price = temp[i].price;
        medicine[i + 1].name.assign( temp[i].name );
        printf("%s %d %.2lf\n", temp[i].name, temp[i].num, temp[i].price);
    }
    string s1 = "感冒药"; string s2 = "退烧药"; string s3 = "处方药";
    string s4 = "止疼药"; string s5 = "抗过敏药"; string s6 = "安眠药";

    class PicImage *yaowu = new PicImage("bmp/yaowu.bmp");
    class PicImage *yao1;
    if(medicine[1].name == s1) yao1 = new PicImage("yaowu/yao1.bmp");
    else yao1 = new PicImage("yaowu/yao11.bmp");
    
    class PicImage *yao2;
    if(medicine[2].name == s2) yao2 = new PicImage("yaowu/yao2.bmp");
    else yao2 = new PicImage("yaowu/yao22.bmp");

    class PicImage *yao3;
    if(medicine[3].name == s3) yao3 = new PicImage("yaowu/yao3.bmp");
    else yao3 = new PicImage("yaowu/yao33.bmp");

    class PicImage *yao4;
    if(medicine[4].name == s4) yao4 = new PicImage("yaowu/yao4.bmp");
    else yao4 = new PicImage("yaowu/yao44.bmp");
    
    class PicImage *yao5;
    if(medicine[5].name == s5) yao5 = new PicImage("yaowu/yao5.bmp");
    else yao5 = new PicImage("yaowu/yao55.bmp");

    class PicImage *yao6;
    if(medicine[6].name == s6) yao6 = new PicImage("yaowu/yao6.bmp");
    else yao6 = new PicImage("yaowu/yao66.bmp");

    // puts("到这了?");

    MyFont font(20, TTF1);
    MyFont font1(33, TTF1);
    // string str1 = "感冒药";
    // string str2 = "退烧药";
    // string str3 = "处方药";
    // string str4 = "止疼药";
    // string str5 = "抗过敏药";
    // string str6 = "安眠药";
    string str7 = "购买";
    string str8 = "刷新";
    string str9 = "返回";
    
    //价格及数量显示处理
    MyFont font_medicineinfo(15, TTF1);
    medicine[1].price_x = 25; medicine[1].price_y = 35;
    medicine[2].price_x = 225; medicine[2].price_y = 35;
    medicine[3].price_x = 445; medicine[3].price_y = 35;
    medicine[4].price_x = 25; medicine[4].price_y = 215;
    medicine[5].price_x = 225; medicine[5].price_y = 215;
    medicine[6].price_x = 445; medicine[6].price_y = 215;
    for(int i = 1; i <= 6; i ++ )
    {
        string str = "价格:";
        medicine[i].price_string = doubleToStringWithPrecision(medicine[i].price, 2);
        medicine[i].price_string = str + medicine[i].price_string;
        medicine[i].num_string = to_string(medicine[i].num);
        medicine[i].num_string = " 数量:" + medicine[i].num_string;
        medicine[i].price_string += medicine[i].num_string;
    }
    
    //数字处理

    medicine[1].yao = yao1; //medicine[1].name = str1;
    medicine[2].yao = yao2; //medicine[2].name = str2;
    medicine[3].yao = yao3; //medicine[3].name = str3;
    medicine[4].yao = yao4; //medicine[4].name = str4;
    medicine[5].yao = yao5; //medicine[5].name = str5;
    medicine[6].yao = yao6; //medicine[6].name = str6;

    medicine[1].font_x = 60; medicine[1].font_y = 175;
    medicine[1].image_x = 60; medicine[1].image_y = 50;
    medicine[2].font_x = 270; medicine[2].font_y = 175;
    medicine[2].image_x = 270; medicine[2].image_y = 50;
    medicine[3].font_x = 480; medicine[3].font_y = 175;
    medicine[3].image_x = 480; medicine[3].image_y = 50;
    medicine[4].font_x = 60; medicine[4].font_y = 355;
    medicine[4].image_x = 60; medicine[4].image_y = 230;
    medicine[5].font_x = 270; medicine[5].font_y = 355;
    medicine[5].image_x = 270; medicine[5].image_y = 230;
    medicine[6].font_x = 480; medicine[6].font_y = 355;
    medicine[6].image_x = 480; medicine[6].image_y = 230;

    while(1)
    {
        lcd->drawBmp(yaowu, 0, 0);          //先刷页面，再刷信息
        MyFont font_count(20, TTF1);
        for(int i = 0; i < 6; i ++ )
        {
            string count = to_string(medicine_count[i]);
            font_count.FontPrint(show_num_font[i].x, show_num_font[i].y, count, 0,0,255);
        }

        for(int i = 1; i <= 6; i ++ )
        {
            lcd->drawBmp(medicine[i].yao, medicine[i].image_x, medicine[i].image_y);
            font.FontPrint(medicine[i].font_x, medicine[i].font_y, medicine[i].name, 0, 0, 0);
            font_medicineinfo.FontPrint(medicine[i].price_x, medicine[i].price_y, medicine[i].price_string
                , 0, 0, 0);
            
        }
        font1.FontPrint(650, 305, str7, 255, 255, 255);
        font1.FontPrint(650, 45, str8, 255, 255, 255);
        font1.FontPrint(650, 170, str9, 255, 255, 255);

        Point p = ts->readXY();
        
        if(p.x >= 620 && p.y >= 20 && p.y <= 100)
        {
            //刷新
            //write(client->Get_Socket(), buf, sizeof(buf));    不用这样，重新进一下这个函数即可
            // puts("刷新!!!!!!!!");
            rein = true;
            //需要把上次买药的数量清空
            for(int i = 0; i < 6; i ++ ) medicine_count[i] = 0;
            return ;
        }
        else if(p.x >= 620 && p.y >= 150 && p.y <= 230)
        {
            //返回
            // puts("返回没??");
            return ;
        }
        else if(p.x >= 620 && p.y >= 280 && p.y <= 360)
        {
            //购买
            int cheak = 0;
            for(int i = 0; i < 6; i ++ ) cheak += medicine_count[i];
            if(cheak) yemian33(lcd, ts);
            
            if(rein1)
            {
                memset(medicine_count, 0, sizeof(medicine_count));
                return ;
            }
            //返回
        }
        else       //购买数量显示
        {
            if(p.x <= 60 && p.y >= 130 && p.y <= 180)
            {
                //1-
                if(medicine_count[0] > 0) medicine_count[0] -- ;
            }
            else if(p.x >= 120 && p.x <= 180 && p.y >= 130 && p.y <= 180)
            {
                //1+
                if(medicine_count[0] < medicine[1].num) medicine_count[0] ++ ;
            }
            else if(p.x >= 210 && p.x <= 260 && p.y >= 130 && p.y <= 180)
            {
                //2-
                if(medicine_count[1] > 0) medicine_count[1] -- ;
            }
            else if(p.x >= 330 && p.x <= 380 && p.y >= 130 && p.y <= 180)
            {
                //2+
                if(medicine_count[1] < medicine[2].num) medicine_count[1] ++ ;
            }
            else if(p.x >= 430 && p.x <= 480 && p.y >= 130 && p.y <= 180)
            {
                //3-
                if(medicine_count[2] > 0) medicine_count[2] -- ;
            }
            else if(p.x >= 550 && p.x <= 590 && p.y >= 130 && p.y <= 180)
            {
                //3+
                if(medicine_count[2] < medicine[3].num) medicine_count[2] ++ ;
            }
            else if(p.x <= 60 && p.y >= 310 && p.y <= 360)
            {
                //4-
                if(medicine_count[3] > 0) medicine_count[3] -- ;
            }
            else if(p.x >= 120 && p.x <= 180 && p.y >= 310 && p.y <= 360)
            {
                //4+
                if(medicine_count[3] < medicine[4].num) medicine_count[3] ++ ;
            }
            else if(p.x >= 210 && p.x <= 260 && p.y >= 310 && p.y <= 360)
            {
                //5-
                if(medicine_count[4] > 0) medicine_count[4] -- ;
            }
            else if(p.x >= 330 && p.x <= 380 && p.y >= 310 && p.y <= 360)
            {
                //5+
                if(medicine_count[4] < medicine[5].num) medicine_count[4] ++ ;
            }
            else if(p.x >= 430 && p.x <= 480 && p.y >= 310 && p.y <= 360)
            {
                //6-
                if(medicine_count[5] > 0) medicine_count[5] -- ;
            }
            else if(p.x >= 550 && p.x <= 590 && p.y >= 310 && p.y <= 360)
            {
                //6+
                if(medicine_count[5] < medicine[6].num) medicine_count[5] ++ ;
            }
        }

    }
    
}

struct node {    
    char username[32]; 
    char type[32];    
    char t1[32];
    char t2[32];    
    char money[32];    
    char state[32];    
} send_msg[10];

void yemian4(class LcdDevice *lcd, class TsDev *ts)
{
    // puts("??????????");
    memset(send_msg, 0, sizeof(send_msg));
    read(client->Get_Socket(), send_msg, sizeof(send_msg));
    //根据send_msg进行绘制
    for(int i = 0; i < 10; i ++ )
    {
        printf("%s %s %s %s %s\n", send_msg[i].type, send_msg[i].t1, send_msg[i].t2, send_msg[i].money, 
                    send_msg[i].state);
    }

    class PicImage *fapiao = new PicImage("bmp/fapiao.bmp");
    lcd->drawBmp(fapiao, 0, 0);
    MyFont font(25, TTF1);
    string str1 = "返回";
    string str2 = "打印";

    MyFont font1(20, TTF1);
    string str3 = "类别";
    string str4 = "发票日期";
    string str5 = "金额";
    string str6 = "发票状态";
    font1.FontPrint(65, 50, str3, 255,255,255);
    font1.FontPrint(190, 50, str4, 255,255,255);
    font1.FontPrint(380, 50, str5, 255,255,255);
    font1.FontPrint(490, 50, str6, 255,255,255);
    
    // string s1(send_msg[0].type);
    // string s2(send_msg[0].t1);
    // string s3(send_msg[0].t2);
    // string s4(send_msg[0].money);
    // string s5(send_msg[0].state);
    int cnt = 0;
    string s[10][5];
    for(int i = 0; i < 10; i ++ )
    {
        if(strlen(send_msg[i].money) == 0) break;
        s[i][0] = send_msg[i].type;
        s[i][1] = send_msg[i].t1;
        s[i][2] = send_msg[i].t2;
        s[i][3] = send_msg[i].money;
        s[i][4] = send_msg[i].state;
        cnt = i;
    }
    cnt ++ ;
    string line[10];
    for(int i = 0; i < cnt; i ++ )
    {
        string space12 = "            ";
        string space6 = "      ";
        string space2 = "  ";
        line[i] = s[i][0] + space6 + s[i][1] + space2 + s[i][2] + space6 + s[i][3] + space12 + s[i][4];
    }
    int step = 0;
    for(int i = 0; i < cnt; i ++ )
    {
        string cheak = s[i][4];
        if(cheak == "已开票")
            font1.FontPrint(65, 90 + step, line[i], 0, 255, 0);
        else
            font1.FontPrint(65, 90 + step, line[i], 0,0,0);
        step += 30;
    }

    font.FontPrint(670, 53, str1, 255,255,255);
    font.FontPrint(670, 366, str2, 255,255,255);

    while(1)
    {
        Point p = ts->readXY();
        if(p.x >= 620 && p.y <= 100)
        {
            //返回
            return ;
        }
        else if(p.x >= 620 && p.y >= 330)
        {
            //打印
            printf("username = %s\n", username);
            write(client->Get_Socket(), username, sizeof(username));
            lcd->drawColor(0xffffff);
            MyFont font3(33, TTF1);
            string str = "发票打印成功, 请注意取走证件";
            font3.FontPrint(255, 355, str, 0,0,0);
            Point P = ts->readXY();
            return ;
        }
    }
}

void yemian5(class LcdDevice *lcd, class TsDev *ts)
{
    lcd->drawColor(0xffffff);
    string str1 = "广东省人民医院（广东省医学科学院）创建于1946年，其前身为广州中央医院。";
    string str2 = "经过几代人的共同努力，广东省人民医院已成为集医教研于一体的大型现代化三";
    string str3 = "级甲等综合型医院，在国内外享有盛誉。2019年国家三级公立医院绩效考核获评";
    string str4 = "最高等级A++，位列全国第10；在最新的三级公立医院绩效考核中，省医排名和";
    string str5 = "分数继续双提升，全国第17名，稳居A+前列，连续五年广东第二名。荣获2022全";
    string str6 = "国医疗机构最佳雇主“公立医院10强”和2022最佳雇主医疗机构“最受大学生欢";
    string str7 = "迎10强”。";

    MyFont font(20, TTF1);
    font.FontPrint(50, 50, str1, 0,0,0);
    font.FontPrint(50, 100, str2, 0,0,0);
    font.FontPrint(50, 150, str3, 0,0,0);
    font.FontPrint(50, 200, str4, 0,0,0);
    font.FontPrint(50, 250, str5, 0,0,0);
    font.FontPrint(50, 300, str6, 0,0,0);
    font.FontPrint(50, 350, str7, 0,0,0);

    Point p = ts->readXY();
}

string time_str;
char tempture[128] = {0};
char weather[128] = {0};
string tmp;
string wea;


void *get_weatherinfo(void *arg)
{
    class Client *client1 = (class Client*)arg;
    char tempture[128] = {0};
    char weather[128] = {0};
    read(client1->Get_Socket(), tempture, sizeof(tempture));
    read(client1->Get_Socket(), weather, sizeof(weather));

    string s1 = weather; s1 = "今日天气:" + s1;
    string s2 = tempture; s2 = "今日温度:" + s2;
    tmp = s2;
    wea = s1;

    while(1)
    {
        char timemsg[62] = {0};
        read(client1->Get_Socket(), timemsg, sizeof(timemsg));
        
        pthread_mutex_lock(&mutex);
        time_str = timemsg;
        time_str = "当前时间:" + time_str;  //获取时间
        time_str.erase(time_str.length() - 3, 3);
        pthread_mutex_unlock(&mutex);
    }
}

int main(void)
{
	// string str = "阿莫西林胶囊";
    // //显示
    // MyFont test;
    // test.FontPrint( 300 , 100 , str); // 把数据str 填入到画板中
    // sleep(3);
    class TsDev *ts = new TsDev("/dev/input/event0");   //创建触摸屏
    class LcdDevice *lcd = new LcdDevice("/dev/fb0");      //创建屏幕，并初始化
    client = new Client(65001);                //阻塞连接

    login(ts, lcd, client->Get_Socket());

    //登录完成，连接65000端口，获取天气以及时间
    class Client *client1 = new Client(65000);
    pthread_t pid;
    pthread_create(&pid, NULL, get_weatherinfo, (void*)client1);
    //主页面
    // cout<< __LINE__ <<endl;
    // MyFont font(25, TTF1);
    MyFont font(25,TTF1);
    // cout<< __LINE__ <<endl;
    string s1 = "挂号";
    string s2 = "买药";
    string s3 = "发票查询";
    string s4 = "医院信息";
    MyFont font1(16,TTF1);
    string s5 = "请注意:初次就诊,除社保卡/医保卡外,需提供";
    string s6 = "本人身份证进行资费卡办理";
    class PicImage *yiyuan = new PicImage("bmp/yiyuan.bmp");
    class PicImage *logo = new PicImage("bmp/logo.bmp");

    MyFont weather_font(16, TTF1);
    // cout<< __LINE__ <<endl;
    while(1)
    {
        // cout<< __LINE__ <<endl;
        drawhome(lcd);
        // cout<< __LINE__ <<endl;
        font1.FontPrint(70, 100, s5, 0, 0, 255);
        font1.FontPrint(120, 120, s6, 0, 0, 255);
        font.FontPrint(115, 155, s1, 255, 255, 255);
        font.FontPrint(90, 310, s3, 255, 255, 255);
        font.FontPrint(275, 155, s2, 255, 255, 255);
        font.FontPrint(252, 310, s4, 255, 255, 255);
        lcd->drawBmp(yiyuan, 425, 235);
        lcd->drawBmp(logo, 50, 10);

        cout << wea << endl << tmp << endl << time_str << endl;
        weather_font.FontPrint(450, 105, wea, 0,0,0);
        weather_font.FontPrint(450, 145, tmp, 0,0,0);
        weather_font.FontPrint(450, 185, time_str, 0,0,0);
        
        

        Point p = ts->readXY();
        int num = get_home_num(p);
        
        switch (num)
        {
            case 1: { over = false; yemian2(lcd, ts, client); break;}
            case 2: {
                yemian3(lcd, ts);
                while(rein){
                    rein = false;
                    puts("刷新");
                    yemian3(lcd, ts);
                }
                rein1 = false;
                break;
                }
            case 3: {
                char temp[16] = {"daying"};
                write(client->Get_Socket(), temp, sizeof(temp));
                yemian4(lcd, ts);
                break;
                }
            case 4: {
                yemian5(lcd, ts); 
                
                break;}

            case 5: {
                delete lcd;
                delete ts;
                class TsDev *ts = new TsDev("/dev/input/event0");   //创建触摸屏
                class LcdDevice *lcd = new LcdDevice("/dev/fb0");      //创建屏幕，并初始化
                client = new Client(65001);                //阻塞连接

                login(ts, lcd, client->Get_Socket());
                            
                break;
                }
            default:
                break;
        }   
    }
    
    //delete font;
    delete lcd;
    delete ts;
    
    return 0;
}
