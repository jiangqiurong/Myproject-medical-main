#include "../inc/yemian1.hpp"
#include "../inc/login.hpp"

using namespace std;

string keshi_memory[9];
int choice;
bool over = false;

int huiyuan=0;

struct user_info
{
    char username[32];
    char t[32];     //日期
    char type[32];      //挂号或者购药
    char money[32];     //金额
    char state[32];     //发票状态
    int nums[6];        //药品数量
}user_send_info1;


void init_info()
{
    // string str = "25.00";
    string str;
    if(huiyuan==2){
        str = "15.00";
    }else{
        str = "25.00";
    }
    strcpy(user_send_info1.money, str.c_str());
    strcpy(user_send_info1.username, username);

    
    if(keshi_memory[choice]=="神经内科"){
        str = "神经内科";
    }
    if(keshi_memory[choice]=="内分泌科"){
        str = "内分泌科";
    }
    if(keshi_memory[choice]=="胸心外科"){
        str = "胸心外科";
    }
    if(keshi_memory[choice]=="内分泌科"){
        str = "内分泌科";
    }
    if(keshi_memory[choice]=="消化科"){
        str = "消化科";
    }
    if(keshi_memory[choice]=="骨科"){
        str = "骨科";
    }
    if(keshi_memory[choice]=="皮肤科"){
        str = "皮肤科";
    }
    if(keshi_memory[choice]=="普外科"){
        str = "普外科";
    }
    strcpy(user_send_info1.type, str.c_str());
    
    str = "未开票";
    strcpy(user_send_info1.state, str.c_str());
}


void number_font()
{
    MyFont num_font(30, TTF1);
    string s1 = "1", s2 = "2", s3 = "3", s4 = "4";
    num_font.FontPrint(145, 50, s1, 255, 255, 255);
    num_font.FontPrint(292, 50, s2, 255, 255, 255);
    num_font.FontPrint(437, 53, s3, 255, 255, 255);
    num_font.FontPrint(585, 50, s4, 255, 255, 255);
}

void yemian23(class LcdDevice *lcd, class TsDev *ts, class Client *client)
{
    class PicImage *disibu = new PicImage("bmp/disibu.bmp");
    MyFont font(32, TTF1);
    string str = "确认挂号";
    string success = "挂号成功";
    MyFont font1(40, TTF2);

    MyFont font2(25, TTF3);
    string line1 = ", 您已经成功挂号";
    line1 = username + line1;

    string line2 = keshi_memory[choice];
    string temp = "上午门诊。";
    line2 += temp;
    temp = "排队号:15。";
    line2 += temp;

    string line3 = "就诊地址:2楼" + keshi_memory[choice] + "候诊区。";
    string line4 = "未携带门诊病历本的患者请在就诊前自行领取填写。";
    

    while(1)
    {
        lcd->drawBmp(disibu, 0, 0);
        number_font();
        font.FontPrint(530, 100, str, 0, 0, 0);
        font1.FontPrint(350, 205, success, 0, 0, 0);

        font2.FontPrint(220, 255, line1, 0, 0, 0);
        font2.FontPrint(220, 290, line2, 0, 0, 0);
        font2.FontPrint(220, 325, line3, 0, 0, 0);
        font2.FontPrint(220, 360, line4, 0, 0, 0);

        Point p = ts->readXY();
        if(p.x >= 700 && p.y >= 400)
        {
            //发guahao
            puts("挂号成功");
            char temp[32] = {"guahao"};
            write(client->Get_Socket(), temp, sizeof(temp));
            init_info();
            write(client->Get_Socket(), &user_send_info1, sizeof(user_send_info1));

            over = true;
            delete disibu;
            return ;
        }
    }
}

void yemian22(class LcdDevice *lcd, class TsDev *ts, class Client *client)
{
    class PicImage *disanbu = new PicImage("bmp/disanbu.bmp");
    MyFont font(32, TTF1);
    string str = "付款";
    string s1 = "扫描二维码付款";
    // string s2 = "25.00 元";
    string s2;
    if(huiyuan==2){
        s2 = "15.00 元";
    }else{
        s2 = "25.00 元";
    }
    MyFont font1(23, TTF1);


    while(1)
    {
        lcd->drawBmp(disanbu, 0, 0);
        number_font();
        font.FontPrint(412, 100, str, 0, 0, 0);
        font1.FontPrint(330, 170, s1, 0, 0, 0);
        font1.FontPrint(360, 200, s2, 255, 0, 0);
        
        Point p = ts->readXY();
        if(p.x >= 0 && p.x <= 100 && p.y >= 0 && p.y <= 80)
        {
            delete disanbu;
            return ;
        }
        else if(p.x >= 340 && p.y <= 480 && p.y >= 230 && p.y <= 470)
        {
            yemian23(lcd, ts, client);
            if(over)
            {
                delete disanbu;
                return ;
            }
        }
    }
}

void yemian21(class LcdDevice *lcd, class TsDev *ts, class Client *client)
{
    class PicImage *dierbu = new PicImage("bmp/dierbu.bmp");
    MyFont font(32, TTF1);
    string str = "费用确认";

    MyFont font1(35, TTF1);
    string s1 = "去支付";

    MyFont font2(28, TTF4);
    string s2 = "挂号类型: 普通号";
    string s3 = "挂号科室: "; s3 += keshi_memory[choice];

    MyFont font3(28, TTF1);
    // string s4 = "支付金额: 25.00元";
    string s4;
    if(huiyuan==2){
        s4 = "15.00 元";
    }else{
        s4 = "25.00 元";
    }

    while(1)
    {
        lcd->drawBmp(dierbu, 0, 0);
        number_font();
        font.FontPrint( 240 , 100 , str, 0, 0, 0);
        font1.FontPrint(350, 370, s1, 255, 255, 255);  

        font.FontPrint(330, 190, username, 0, 0, 0);
        font2.FontPrint(310, 230, s2, 0, 0, 0);
        font2.FontPrint(310, 260, s3, 0, 0, 0);
        font3.FontPrint(290, 310, s4, 0, 0, 0);
        

        Point p = ts->readXY();
        if(p.x >= 0 && p.x <= 100 && p.y >= 0 && p.y <= 80)
        {
            delete dierbu;
            return ;
        }
        else if(p.x >= 250 && p.x <= 550 && p.y >= 370 && p.y <= 440)
        {
            yemian22(lcd, ts, client);
            if(over)
            {
                delete dierbu;
                return ;
            }
        }

    }
}

void show_keshi_font()
{
    MyFont font(23, TTF1);
    string s1 = "神经内科"; keshi_memory[1] = s1;
    string s2 = "内分泌科"; keshi_memory[2] = s2;
    string s3 = "胸心外科"; keshi_memory[3] = s3;
    string s4 = "内分泌科"; keshi_memory[4] = s4;
    string s5 = "消化科";   keshi_memory[5] = s5;
    string s6 = "骨科";     keshi_memory[6] = s6;
    string s7 = "皮肤科";   keshi_memory[7] = s7;
    string s8 = "普外科";   keshi_memory[8] = s8;
    font.FontPrint(135, 203, s1, 255, 255, 255);
    font.FontPrint(275, 203, s2, 255, 255, 255);
    font.FontPrint(415, 203, s3, 255, 255, 255);
    font.FontPrint(555, 203, s4, 255, 255, 255);

    font.FontPrint(145, 298, s5, 255, 255, 255);
    font.FontPrint(295, 298, s6, 255, 255, 255);
    font.FontPrint(420, 298, s7, 255, 255, 255);
    font.FontPrint(570, 298, s8, 255, 255, 255);
    // font.FontPrint(250, 390, s1, 0, 0, 0);
    // font.FontPrint(250, 390, s1, 0, 0, 0);
}

void yemian2(class LcdDevice *lcd, class TsDev *ts, class Client *client)
{
    class PicImage *keshi = new PicImage("bmp/diyibu.bmp");
    MyFont font(32, TTF1);
    string str = "选择科室";

    while(1)
    {
        lcd->drawBmp(keshi, 0, 0);
        show_keshi_font();
        number_font();
        font.FontPrint( 102 , 100 , str, 0, 0, 0);

        bool flag = 0;
        Point p = ts->readXY();
        
        if(p.x >= 0 && p.x <= 100 && p.y >= 0 && p.y <= 80)
        {
            delete keshi;
            return ;
        }

        if(p.x >= 120 && p.x <= 240)
        {
            if(p.y >= 180 && p.y <= 250)
            {
                //1
                flag = 1;
                choice = 1;
            }
            else if(p.y >= 270 && p.y <= 340)
            {
                //5
                flag = 1;
                choice = 5;
            }
            // else if(p.y >= 370 && p.y <= 440)
            // {
            //     //9
            //     flag = 1;
            // }
        }
        else if(p.x >= 260 && p.x <= 380)
        {
            if(p.y >= 180 && p.y <= 250)
            {
                //2
                flag = 1;
                choice = 2;
            }
            else if(p.y >= 270 && p.y <= 340)
            {
                //6
                flag = 1;
                choice = 6;
            }
            // else if(p.y >= 370 && p.y <= 440)
            // {
            //     //10
            //     flag = 1;
            // }
        }
        else if(p.x >= 400 && p.x <= 520)
        {
            if(p.y >= 180 && p.y <= 250)
            {
                //3
                flag = 1;
                choice = 3;
            }
            else if(p.y >= 270 && p.y <= 340)
            {
                //7
                flag = 1;
                choice = 7;
            }
        }
        else if(p.x >= 540 && p.x <= 660)
        {
            if(p.y >= 180 && p.y <= 250)
            {
                //4
                flag = 1;
                choice = 4;
            }
            else if(p.y >= 270 && p.y <= 340)
            {
                //8
                flag = 1;
                choice = 8;
            }
        }
            
        if(flag)        //跳转
        {
            yemian21(lcd, ts, client);
            if(over)
            {
                delete keshi;
                return ;
            }
        }
    }
}