#include "../inc/login.hpp"

using namespace std;

char username[32];
char password[32];

int GetNum(class TsDev *ts)
{
    Point p = ts->readXY();
    int x = p.x, y = p.y;

    if(x >= 200 && x <= 295 && y >= 180 && y <= 250) return 1;
    if(x >= 296 && x <= 417 && y >= 180 && y <= 250) return 2;
    if(x >= 418 && x <= 520 && y >= 180 && y <= 250) return 3;
    if(x >= 200 && x <= 295 && y >= 251 && y <= 330) return 4;
    if(x >= 296 && x <= 417 && y >= 251 && y <= 330) return 5;
    if(x >= 418 && x <= 520 && y >= 251 && y <= 330) return 6;
    if(x >= 200 && x <= 295 && y >= 331 && y <= 400) return 7;
    if(x >= 296 && x <= 417 && y >= 331 && y <= 400) return 8;
    if(x >= 418 && x <= 520 && y >= 331 && y <= 400) return 9;

    if(x >= 200 && x <= 520 && y >= 20 && y <= 60) return 10;  //username
    if(x >= 200 && x <= 520 && y >= 80 && y <= 120) return 11; //password
    
    if(x >= 600 && x <= 700 && y >= 100 && y <= 210) return 12; //login
    if(x >= 600 && x <= 700 && y >= 230 && y <= 335) return 13; //register
    if(x >= 600 && x <= 700 && y >= 25 && y <= 80) return 14;   //删除
    if(x >= 600 && x <= 700 && y >= 360 && y <= 460) return 15;   //返回
    if(x >= 30 && x <= 165 && y >= 185 && y <= 245) return 16;   // 访客
    if(x >= 30 && x <= 165 && y >= 280 && y <= 345) return 17;   // 会员
    if(x >= 15 && x <= 110 && y >= 410 && y <= 450) return 18;   // 会员
}

void get_medicine_image(const char *path, int tcpsocket)
{
    char buffer[8096] = {0};
    FILE *fp;  
    ssize_t read_size;
    fp = fopen(path, "wb");  
    if (fp == NULL) {  
        // printf("Error opening file for writing\n");  
        cerr << "Error opening file for writing" << endl;
        close(tcpsocket);  
        return ;  
    }
    // 接收文件内容  
    while ((read_size = recv(tcpsocket, buffer, 8096, 0)) > 0) { 
        if(read_size == 1){
            // puts("11111111111");
            cout << "Received special signal" << endl;
            break;
        }
        fwrite(buffer, 1, read_size, fp);  
    }  
  
    // 检查是否是因为对方关闭了连接而退出循环  
    if (read_size < 0) {  
        // printf("recv failed\n");  
        cerr << "recv failed" << endl;
    }

    // 关闭文件
    // printf("%s\n", path);
    cout << "File saved at " << path << endl;
    fclose(fp);    
}

void login(class TsDev *ts, class LcdDevice *lcd, int tcpsocket)
{
    string username1 = "";
    string password1 = "";
    int flag = 0;
    class PicImage *jianpan = new PicImage("bmp/jianpan.bmp");
    class PicImage *shanchu = new PicImage("bmp/shanchu.bmp");
    class PicImage *yi = new PicImage("bmp/1.bmp");
    class PicImage *er = new PicImage("bmp/2.bmp");
    class PicImage *san = new PicImage("bmp/3.bmp");
    class PicImage *si = new PicImage("bmp/4.bmp");
    class PicImage *wu = new PicImage("bmp/5.bmp");
    class PicImage *liu = new PicImage("bmp/6.bmp");
    class PicImage *qi = new PicImage("bmp/7.bmp");
    class PicImage *ba = new PicImage("bmp/8.bmp");
    class PicImage *jiu = new PicImage("bmp/9.bmp");
    class PicImage *zhuce = new PicImage("bmp/zhuce.bmp");
    class PicImage *fangke = new PicImage("bmp/fangke.bmp");
    class PicImage *huiyuan1 = new PicImage("bmp/huiyuan1.bmp");
    class PicImage *huiyuan2 = new PicImage("bmp/huiyuan2.bmp");
    class PicImage *number[10] = {jianpan, yi, er, san, si, wu, liu, qi, ba, jiu};

    int c = 1;    
    lcd->drawBmp(jianpan, 0, 0);
    while(1)                        //登录注册功能
    {
        int num = GetNum(ts);
        
        if(num == 10)               
        {
            flag = 1;
        }
        else if(num == 11)
        {
            flag = 2;
        }
        else
        {
            if(num >= 1 && num <= 9)    //给账号或密码添加长度
            {
                if(flag == 1 && username1.size() <= 8)
                {
                    int step = username1.size();
                    username1 += (num + '0');
                    //画数字
                    lcd->drawBmp(number[num], 205 + step*30, 25);
                }
                else if(flag == 2 && password1.size() <= 8)
                {
                    int step = password1.size();
                    password1 += (num + '0');
                    //画数字bmp
                    lcd->drawBmp(number[num], 205 + step*30, 85);
                }
            }
            else if(num == 12)  //登录
            {
                if(!username1.size())
                {
                    puts("账号不能为空");
                }
                else if(!password1.size())
                {
                    puts("密码不能为空");
                }
                else
                {
                    //去服务器看看是否能匹配
                    //先发送一个"denglu"
                    char buf[16] = {"denglu"};
                    write(tcpsocket, buf, strlen(buf) + 1);
                    // printf("username : %s\n", username1);
                    // printf("password : %s\n", password1);
                    
                    int idx = 0;
                    char userinfo[32] = {0};
                    for(int i = 0; i < username1.size(); i ++, idx ++ )
                        userinfo[idx] = username1[i];
                    
                    userinfo[idx ++ ] = ' ';
                    for(int i = 0; i < password1.size(); i ++, idx ++ )
                        userinfo[idx] = password1[i];
                    
                    //发送
                    // printf("%s\n", userinfo);
                    write(tcpsocket, userinfo, sizeof(userinfo));
                    //接收
                    memset(userinfo, 0, sizeof(userinfo));
                    read(tcpsocket, userinfo, sizeof(userinfo));
                    if(strcmp(userinfo, "login success") == 0)
                    {
                        lcd->drawColor(0xffffff);
                        break ;
                    }
                    else if(strcmp(userinfo, "login fail") == 0)
                    {
                        puts("密码错误");
                        lcd->drawBmp(jianpan, 0, 0);
                        username1 = "";
                        password1 = "";
                    }
                    else {puts("???");}
                }
            }
            else if(num == 13 && c == 1)     //注册页面, c=1跳转注册页面，c = 2注册成功
            {
                username1 = "";
                password1 = "";
                lcd->drawBmp(zhuce, 0, 0);
                c ++ ;   
            }
            //返回
            else if(num == 15 && c == 2){
                lcd->drawBmp(jianpan, 0, 0);
                username1 = "";
                password1 = "";
                c = 1;      //重置状态
            }
            else if(num == 17){
                huiyuan=2;
                puts("是会员!!");
                lcd->drawBmp(huiyuan2, 0, 0);
                
                
            }
            else if (num==18)
            {
                lcd->drawBmp(huiyuan1, 0, 0);
            }
            else if(num == 16){
                huiyuan=1;
                puts("是访客!!");
                lcd->drawBmp(fangke, 0, 0);
            }
            else if(num == 13 && c == 2)    //再次点击注册
            {
                //看看账号和密码是否为0
                if(!username1.size() || !password1.size())
                {
                    //注册失败
                    puts("账号或密码不能为空");
                }
                else        //注册成功 
                {
                    //发送给服务器,跳转到登录页面
                    char temp[32] = {"zhuce"};
                    write(tcpsocket, temp, sizeof(temp));
                    char userinfo[32] = {0};
                    int idx = 0;
                    for(int i = 0; i < username1.size(); i ++, idx ++ )
                        userinfo[idx] = username1[i];
                    userinfo[idx ++ ] = ' ';
                    for(int i = 0; i < password1.size(); i ++ , idx ++ )
                        userinfo[idx] = password1[i];
                    
                    write(tcpsocket, userinfo, sizeof(userinfo));       //发过去
                    //{code};
                    //看看是否重复
                    memset(temp, 0, sizeof(temp));
                    read(tcpsocket, temp, sizeof(temp));    //读取服务器发来的信息

                    if(strcmp(temp, "register success") == 0)
                    {
                        puts("注册成功");
                        lcd->drawBmp(jianpan, 0, 0);
                        password1 = "";
                        username1 = "";
                        c = 1;      //重置状态
                    }
                    else if(strcmp(temp, "register fail") == 0)
                    {
                        puts("注册失败，已经存在用户");
                        lcd->drawBmp(zhuce, 0, 0);
                        password1 = "";
                        username1 = "";
                    }
                    else printf("%s\n", temp);
                }
            }
            else if(num == 14)     //根据flag进行删除
            {
                if(flag == 1)   //删除username
                {
                    int sz = username1.size();
                    if(sz > 0)
                    {
                        int step = sz - 1;
                        username1.erase(username1.end() - 1);
                        //画空白bmp
                        lcd->drawBmp(shanchu, 205 + step*30, 25);
                    }
                }
                else if(flag == 2)  //删除password
                {
                    int sz = password1.size();
                    if(sz > 0)
                    {
                        int step = sz - 1;
                        password1.erase(password1.end() - 1);
                        //画空白bmp
                        lcd->drawBmp(shanchu, 205 + step*30, 85);
                    }
                }
                // puts("delete success"); 
                // printf("flag = %d\n", flag);
            }
            else                    //点到屏幕外，把flag置0
            {
                puts("点到按键外！");
                // flag = 0;
            }
        }
    }
    
    // lcd->drawColor(0x0000ff);
    // class PicImage *s1 = new PicImage("yaowu/yao1.bmp");
    // class PicImage *s2 = new PicImage("yaowu/yao2.bmp");
    // class PicImage *s3 = new PicImage("yaowu/yao3.bmp");
    // class PicImage *s4 = new PicImage("yaowu/yao4.bmp");
    // class PicImage *s5 = new PicImage("yaowu/yao5.bmp");
    // class PicImage *s6 = new PicImage("yaowu/yao6.bmp");
    
    // lcd->drawColor(0x00ff00);
    // lcd->drawBmp(s1, 20, 20);
    // lcd->drawBmp(s2, 100, 100);
    // lcd->drawBmp(s3, 200, 200);
    // lcd->drawBmp(s4, 300, 100);
    // lcd->drawBmp(s5, 300, 200);
    // lcd->drawBmp(s6, 300, 300);



    // //接收药物展示图片
    // get_medicine_image("yaowu/yao1.bmp", tcpsocket);
    // get_medicine_image("yaowu/yao2.bmp", tcpsocket);
    // get_medicine_image("yaowu/yao3.bmp", tcpsocket);
    // get_medicine_image("yaowu/yao4.bmp", tcpsocket);
    // get_medicine_image("yaowu/yao5.bmp", tcpsocket);
    // get_medicine_image("yaowu/yao6.bmp", tcpsocket);
    // puts("???????????");
    strcpy(username, username1.c_str());
    strcpy(password, password1.c_str());

}