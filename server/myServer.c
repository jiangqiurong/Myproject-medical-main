// #include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <netdb.h>
#include <pthread.h>
#include "cJSON.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

#define ADDR "192.168.53.23"
#define RECV_BUF_SIZE 4096  
#define JSON_BUF_SIZE 2048

char Temperature[100];
char Weather[100];

// using namespace std;

void * SendWeather(void * arg )
{
    struct hostent *p;
    p = gethostbyname("apis.juhe.cn");

    // 获得服务器的IP地址
    char HostIP [16] = {0};
    for(int i=0; p->h_addr_list[i] != NULL; i++)
    {
        snprintf( HostIP , 16 ,"%s", inet_ntoa(*(struct in_addr*)((p->h_addr_list)[i])));
    }
//    printf("HostIP:[%s]\n" , HostIP);


    // 创建TCP套接字
    int weather_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (weather_socket < 0)
    {
        perror("socket error");
        exit(0);
    }  
  
    struct sockaddr_in serverAddr;  
    memset(&serverAddr, 0, sizeof(serverAddr));  
    serverAddr.sin_family = AF_INET;  
    serverAddr.sin_port = htons(80);  
    serverAddr.sin_addr.s_addr = inet_addr(HostIP);  
  
    if (connect(weather_socket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {  
        perror("connect error");  
        return NULL;  
    }  
  
    char *request = "GET /simpleWeather/query?key=6fac59850e3db171f09fc24c5ba4729c&city=%E5%B9%BF%E5%B7%9E HTTP/1.1\r\nHost: apis.juhe.cn\r\n\r\n";
    //char *request = "GET /simpleWeather/query?city=%E8%8B%8F%E5%B7%9E&key=251518e073ef6c3c9504dd286c3f6a86 HTTP/1.1\r\nHost: apis.juhe.cn\r\n\r\n";
    int ret_val = write(weather_socket, request, strlen(request));  
    if (ret_val < 0) {  
        perror("write error");  
        return NULL;  
    }  
  
    char recvMsgBuf[RECV_BUF_SIZE] = {0};  

    ret_val = read( weather_socket ,  recvMsgBuf , sizeof(recvMsgBuf) );
    //printf("rcvmsg:\n%s", recvMsgBuf);

    char *body_start = strstr(recvMsgBuf, "\r\n\r\n");  
    if (body_start != NULL) {  
        body_start += 4; // 跳过"\r\n\r\n"  
    
        // 现在body_start指向响应体的开始  
        // 处理响应体...  
        //printf("Response body:\n%s\n", body_start);  
    } else {  
        printf("Response headers do not end with CRLF CRLF.\n");  
    }
    body_start = body_start + 3;
    //printf("body_start:\n%s", body_start);

    cJSON *json = cJSON_Parse(body_start);  
    if (!json) {  
        puts("ERROR!!!!!!!!!!!!");
         return NULL;  
    } 
  
    // 提取JSON中的数据  
    //puts("提取出来的::::::::::::::::::::\n");
    const cJSON *result = cJSON_GetObjectItemCaseSensitive(json, "result");  
    if (result) {  
        const cJSON *future = cJSON_GetObjectItemCaseSensitive(result, "future");  
        if (cJSON_IsArray(future)) {  
            int future_size = cJSON_GetArraySize(future);  
            //printf("Future weather forecast:\n");  
            for (int i = 0; i < future_size; i++) {  
                const cJSON *day = cJSON_GetArrayItem(future, i);  
                if (day) {  
                    const cJSON *date = cJSON_GetObjectItemCaseSensitive(day, "date");  
                    const cJSON *temperature = cJSON_GetObjectItemCaseSensitive(day, "temperature");  
                    const cJSON *weather = cJSON_GetObjectItemCaseSensitive(day, "weather");  
                    const cJSON *wid_day = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItemCaseSensitive(day, "wid"), "day");  
                    const cJSON *wid_night = cJSON_GetObjectItemCaseSensitive(cJSON_GetObjectItemCaseSensitive(day, "wid"), "night");  
                    const cJSON *direct = cJSON_GetObjectItemCaseSensitive(day, "direct");  
  
                    if (cJSON_IsString(date) && (date->valuestring != NULL)) {
                        printf("Date: %s\n", date->valuestring);  
                    }  
                    if (cJSON_IsString(temperature) && (temperature->valuestring != NULL)) {
                        if(i == 0)
                        {
                            for(int i = 0; temperature->valuestring[i]; i ++ )
                                Temperature[i] = temperature->valuestring[i];
                        }  
                        printf("Temperature: %s\n", temperature->valuestring);  
                    }  
                    if (cJSON_IsString(weather) && (weather->valuestring != NULL)) {
                        if(i == 0)
                        {
                            for(int i = 0; weather->valuestring[i]; i ++ )
                                Weather[i] = weather->valuestring[i];
                        }  
                        printf("Weather: %s\n", weather->valuestring);  
                    }  
                    if (cJSON_IsString(wid_day) && (wid_day->valuestring != NULL)) {  
                        printf("Day wid: %s\n", wid_day->valuestring);  
                    }  
                    if (cJSON_IsString(wid_night) && (wid_night->valuestring != NULL)) {  
                        printf("Night wid: %s\n", wid_night->valuestring);  
                    }  
                    if (cJSON_IsString(direct) && (direct->valuestring != NULL)) {  
                        printf("Wind direction: %s\n", direct->valuestring);  
                    }  
                    printf("\n");  
                }  
            }  
        }  
    }
    else puts("NNNNNNNNNNNNNNNNNNNNN!"); 
  
    // 释放cJSON对象  
    puts("温度:");
    printf("%s\n", Temperature);
    puts("天气:");
    printf("%s\n", Weather);

    cJSON_Delete(json); 
    close(weather_socket);  
}

void * SendCurTime(void *arg)
{
    int tcpScoket = *(int*)arg;

    //处理Msg
    //write(tcpScoket, RecvMsgBuf, sizeof(RecvMsgBuf));
    //先把天气发过去
    
    write(tcpScoket, Temperature, sizeof(Temperature));
    write(tcpScoket, Weather, sizeof(Weather));
    sleep(1);
    
    while (1)
    {  
        char timeStr[20];
        time_t now = time(NULL);
        struct tm *t = localtime(&now);
        strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", t);
        printf("当前时间是: %s\n", timeStr);

        sleep(1);
        
        pthread_mutex_lock(&mutex);
        int ret_val = write( tcpScoket , timeStr , sizeof(timeStr) );
        pthread_mutex_unlock(&mutex);
        if(ret_val <= 0)
        {
            perror("send Msg error");
            close(tcpScoket); 
            exit(0);
        }
    }
}


int main()
{
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0)
    {
        perror("tcp socke error");
        exit(0);
    }
    
    // 设置好本地的地址信息（IP+PORT）
    struct sockaddr_in ServerAddr = {
        .sin_family = AF_INET ,
        .sin_port = htons(65000),
        .sin_addr.s_addr = INADDR_ANY
    };

    // 把地址信息与套接字进行绑定 （待连接套接字）
    if(bind( tcp_socket , (struct sockaddr*)&ServerAddr , 
                        sizeof(ServerAddr)))
    {
        perror("Bind ");
        exit(0);
    }
//    perror("Bind ");
              

    // 设置监听状态（待连接套接字 --> 监听套接字）
    // 处于监听状态下的 套接字拥有等待连接的功能
    if(listen( tcp_socket , 128 ))
    {
        perror("Listen ");
        exit(0);
    }
//    perror("Listen ");
    

    // 等待客户端发起连接请求 （连接成功增加一个已连接套接字）
    struct sockaddr_in FromAddr ;
    socklen_t addrLen  = sizeof(FromAddr);

    int AcceptScoket = accept( tcp_socket , (struct sockaddr* )&FromAddr  , &addrLen );
    if (AcceptScoket < 0)
    {
        perror("accept error");
        exit(0);
    }

    pthread_t tid;
    pthread_create(&tid, NULL, SendWeather, (void*)&AcceptScoket);

    sleep(1);
    pthread_create(&tid, NULL, SendCurTime, (void*)&AcceptScoket);

    pthread_exit(NULL);
}


