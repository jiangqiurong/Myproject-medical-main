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
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "thread_pool.h"

#define ADDR "192.168.53.23"
#define MAX_SOCKET_SIZE 1024

// using namespace std;

//药物
#define N 100 // 假设最多有100个数据项 
typedef struct {  
    char name[100]; // 假设名字不会超过99个字符（为'\0'留一个位置）  
    int num;  
    double price;  
} Medicine;
int MedicineNums[7];

thread_pool *pool;
int tcpScoket[MAX_SOCKET_SIZE];
int socket_size;

pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

struct user_info
{
    char username[32];
    char t[32];     //日期
    char type[32];      //挂号或者购药
    char money[32];     //金额
    char state[32];     //发票状态
    int nums[6];        //药品数量
}user_send_info;

int change_medicines_txt()
{

    typedef struct {  
    char name[50];  
    int quantity;  
    double price;  
    } Medicine;

    FILE *file = fopen("MedicineInfo/medicine.txt", "r");  
    if (file == NULL) {  
        perror("无法打开文件 a.txt");  
        return EXIT_FAILURE;  
    }  
  
    Medicine medicines[6]; // 假设最多有6种药品  
    int i = 0;  
    char buffer[100]; // 用于读取文件的缓冲区  
  
    // 读取文件并存储数据  
    while (fgets(buffer, sizeof(buffer), file) != NULL && i < 6) {  
        sscanf(buffer, "%s %d %lf", medicines[i].name, &medicines[i].quantity, &medicines[i].price);  
        // 去除名字末尾的换行符（如果fgets包含了它）  
        medicines[i].name[strcspn(medicines[i].name, "\n")] = 0;  
        i++;  
    }  
  
    fclose(file);  
  
    // 假设的购买数量数组  
    int arr[6] = {0};  
    for(int i = 0; i < 6; i ++ )
        arr[i] = user_send_info.nums[i];
  
    // 检查并更新购买数量  
    for (int j = 0; j < 6; j++) {  
        if (arr[j] > medicines[j].quantity) {  
            printf("购买 %s 失败，库存不足。\n", medicines[j].name);  
            return 0;
        } else {  
            // 更新库存  
            medicines[j].quantity -= arr[j];  
            printf("成功购买 %d 个 %s，剩余库存 %d。\n", arr[j], medicines[j].name, medicines[j].quantity);  
        }  
    }  
  
    // 将更新后的库存写回 a.txt 文件  
    FILE *outFile = fopen("MedicineInfo/medicine.txt", "w"); // 注意：这里使用 "w" 模式会覆盖文件内容  
    if (outFile == NULL) {  
        perror("无法打开文件 a.txt 以写入");  
        return EXIT_FAILURE;  
    }  
  
    for (int k = 0; k < 6; k++) {  
        fprintf(outFile, "%s %d %.2lf\n", medicines[k].name, medicines[k].quantity, medicines[k].price);  
    }  
  
    fclose(outFile);  
  
    printf("更新后的库存已覆盖写在 a.txt 文件中。\n");
    return 1;
}


void GetMedicineInfo(Medicine *send_medicine)
{
    FILE *fp;  
    Medicine *medicine;  
    int i, count = 6; // 假设你知道要读取的条目数量  
  
    // 动态分配内存以存储结构体数组  
    medicine = (Medicine *)malloc(count * sizeof(Medicine));  
    if (medicine == NULL) {  
        perror("内存分配失败");  
        exit(EXIT_FAILURE);  
    }  
  
    // 打开文件  
    fp = fopen("MedicineInfo/medicine.txt", "r");  
    if (fp == NULL) {  
        perror("无法打开文件");  
        exit(EXIT_FAILURE);  
    }  
  
    // 读取数据  
    for (i = 0; i < count && fscanf(fp, "%99s %d %lf", medicine[i].name, &medicine[i].num, &medicine[i].price) == 3; ++i) {  
        // 检查是否成功读取了三个字段  
    }  
  
    // 关闭文件  
    fclose(fp);  
  
    // 验证结果  
    for (i = 0; i < count; ++i) {  
        memcpy(send_medicine[i].name, medicine[i].name, sizeof(medicine[i].name));
        send_medicine[i].num = medicine[i].num;
        send_medicine[i].price = medicine[i].price;
        printf("%s %d %d\n", medicine[i].name, medicine[i].num, medicine[i].price);
    } 
    puts(""); 
  
    // 释放内存  
    free(medicine);
}

int CheakUserInfo(char userinfo[])
{
    char inputUsername[50];  
    char inputPassword[50];  
    char fileUsername[50];  
    char filePassword[50];  
    char line[120];  
    FILE *file;  

    // 打开文件  
    file = fopen("./UserInfo/user.txt", "r");  
    if (file == NULL) {  
        perror("Error opening file");  
        return EXIT_FAILURE;  
    }  

    // 逐行读取文件并比对  
    while (fgets(line, 120, file)) {  
        // 去除行尾的换行符  
        line[strcspn(line, "\n")] = '\0';  

        // 分割用户名和密码  
        sscanf(userinfo, "%49s %49s", inputUsername, inputPassword);
        sscanf(line, "%49s %49s", fileUsername, filePassword);

        // 比对用户名和密码  
        if (strcmp(inputUsername, fileUsername) == 0 && strcmp(inputPassword, filePassword) == 0) {  
            fclose(file); // 成功登录后关闭文件  
            return 1;  
        }  
    }  

    // 如果循环结束还没有找到匹配项，则登录失败  
    fclose(file); // 关闭文件  
    return 0;
}

void registerUser(const char* userinfo) {  
    FILE *file;  
    char buffer[120 + 2]; // +2 for space and newline  
  
    // 打开文件以追加模式  
    file = fopen("./UserInfo/user.txt", "a");  
    if (file == NULL) {  
        perror("Error opening file");  
        exit(EXIT_FAILURE);  
    }  
  
    // 将用户名和密码写入缓冲区，并添加空格分隔符  
    snprintf(buffer, sizeof(buffer), "%s", userinfo);  
  
    // 将缓冲区内容写入文件  
    fputs(buffer, file);  
  
    // 关闭文件  
    fclose(file);  
  
    printf("注册成功！\n");  
}

void shuaxin_func(int tcpsocket)
{
    Medicine send_medicine[6];
    GetMedicineInfo(send_medicine);

    write(tcpsocket, send_medicine, sizeof(send_medicine));//发回去
}

void send_medicine_image(const char *path, int tcpsocket)
{
    char buffer[8096] = {0};
    FILE *file;
    ssize_t read_size;
    file = fopen(path, "rb");  
    if (file == NULL) {  
        perror("Unable to open file!");  
        exit(EXIT_FAILURE);  
    }
    while ((read_size = fread(buffer, 1, 8096, file)) > 0) {  
        printf("%s\n", path);
        send(tcpsocket, buffer, read_size, 0);  
    } 
    // 关闭文件  
    usleep(500000);
    char c = 'a';
    send(tcpsocket, &c, 1, 0);
    fclose(file);
}

void write_to_file() { 

    //根据user_send_info去给node赋值
    struct node {  
    char username[32];  
    char type[32];  
    char t[32];  
    char money[32];  
    char state[32];  
    };
    struct node *node = (struct node*)malloc(sizeof(struct node));

    strcpy(node->money, user_send_info.money);
    strcpy(node->state, user_send_info.state);
    strcpy(node->type, user_send_info.type);
    strcpy(node->username, user_send_info.username);

    char timeStr[20];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", t);
    printf("当前时间是: %s\n", timeStr);

    strcpy(node->t, timeStr);
    

    FILE *fp;  
    char filename[64];  
  
    // 构造文件名  
    snprintf(filename, sizeof(filename), "%s.txt", node->username);  
  
    // 尝试以追加模式打开文件  
    fp = fopen(filename, "a");  
    if (fp == NULL) {  
        // 如果打开文件失败，打印错误信息  
        perror("Error opening file for appending");  
        return;  
    }  
  
    // 写入数据到文件，使用换行符分隔每条记录（如果需要的话）  
    fprintf(fp, "%s %s %s %s\n", node->type, node->t, node->money, node->state);  
  
    // 关闭文件  
    fclose(fp);  
}

struct node {    
    char username[32];  // 注意：这个字段在文件中没有对应的数据，我们假设它是文件名或其他固定值  
    char type[32];    
    char t1[32];
    char t2[32];    
    char money[32];    
    char state[32];    
} send_msg[10];

int init_sendlog(const char *c)
{
    char str[32] = {0};
    int idx = 0;
    for(; c[idx]; idx ++ )
        str[idx] = c[idx];
    str[idx ++ ] = '.'; str[idx ++ ] = 't'; str[idx ++ ] = 'x'; str[idx ++ ] = 't';

    FILE *file = fopen(str, "r"); 
    memset(send_msg, 0, sizeof(send_msg)); 
    if (file == NULL) {  
        perror("Error opening file");  
        return 1;  
    }  
  
    int i = 0;  
    
    while (fscanf(file, "%31s %31s %31s %31s %31s", send_msg[i].type, send_msg[i].t1, send_msg[i].t2, send_msg[i].money, send_msg[i].state) == 5 && i < 10) {  
        // 假设username字段是固定的，这里我们将其设置为文件名  
        strcpy(send_msg[i].username, str);  
        i++;  
    }  
  
    fclose(file);  
  
    // 打印读取的数据以验证  
    for (int j = 0; j < i; j++) {  
        printf("Username: %s, Type: %s, T1: %s %s, Money: %s, State: %s\n",  
               send_msg[j].username, send_msg[j].type, send_msg[j].t1, send_msg[j].t2, send_msg[j].money, send_msg[j].state);  
    }
}

void replace_string_in_buffer(char *buffer, size_t size, const char *old_str, const char *new_str) {  
    char *pos = buffer;  
    while ((pos = strstr(pos, old_str)) != NULL && ((size_t)(pos - buffer) + strlen(old_str)) < size) {  
        // 确保有足够的空间来替换字符串  
        if ((size_t)(pos - buffer) + strlen(new_str) < size) {  
            strcpy(pos, new_str); // 直接替换（注意：这要求new_str长度不大于old_str）  
            pos += strlen(new_str); // 移动指针到替换后的字符串之后  
        } else {  
            // 如果空间不足，则停止替换（实际情况下可能需要更复杂的错误处理）  
            break;  
        }  
    }  
}

#define MAX_LINES 10  
#define MAX_LINE_LENGTH 1024 
#define INVOICE_PENDING "未开票"
#define INVOICE_COMPLETED "已开票"

int change_fapiao_info(const char *c)
{
    // char filename[64];
    // snprintf(filename, sizeof(filename), "%s.txt", username);

    // FILE *file = fopen(filename, "r");
    // if (!file) {
    //     perror("Unable to open file");
    //     return EXIT_FAILURE;
    // }

    // char buffer[MAX_LINES][MAX_LINE_LENGTH];
    // int line_count = 0;
    // while (fgets(buffer[line_count], MAX_LINE_LENGTH, file) && line_count < MAX_LINES) {
    //     replace_string_in_buffer(buffer[line_count], MAX_LINE_LENGTH, INVOICE_PENDING, INVOICE_COMPLETED);
    //     line_count++;
    // }
    // fclose(file);

    // // Write modified data back to the file
    // file = fopen(filename, "w");
    // if (!file) {
    //     perror("Unable to open file for writing");
    //     return EXIT_FAILURE;
    // }

    // for (int i = 0; i < line_count; i++) {
    //     fputs(buffer[i], file);
    // }
    // fclose(file);

    // return 1;
    char str[32] = {0};
    int idx = 0;
    for( ; c[idx]; idx ++ )
        str[idx] = c[idx];
    
    str[idx ++ ] = '.'; str[idx ++ ] = 't'; str[idx ++ ] = 'x'; str[idx ++ ] = 't';

    char buffer[MAX_LINES][MAX_LINE_LENGTH];  
    FILE *file = fopen(str, "r");  
    if (!file) {  
        perror("无法打开文件");  
        return EXIT_FAILURE;  
    }  
  
    int line_count = 0;  
    while (fgets(buffer[line_count], MAX_LINE_LENGTH, file) && line_count < MAX_LINES) {  
        // 替换字符串"未开票"为"已开票"  
        char *pos = strstr(buffer[line_count], "未开票");  
        if (pos) {  
            // 确保不会覆盖原字符串的其他部分（虽然在这个例子中不会）  
            size_t len = strlen("已开票");  
            memmove(pos + len, pos + strlen("未开票"), strlen(pos + strlen("未开票")) + 1); // 移动剩余部分  
            memcpy(pos, "已开票", len); // 复制新字符串  
        }  
        // 注意：这里不需要手动添加换行符，因为fgets已经保留了它们（如果行末有的话）  
        line_count++;  
    }  
  
    fclose(file); // 关闭文件以读取  
  
    // 重新以写模式打开文件  
    file = fopen(str, "w");  
    if (!file) {  
        perror("无法打开文件以写入");  
        return EXIT_FAILURE;  
    }  
  
    // 写回修改后的内容  
    for (int i = 0; i < line_count; i++) {  
        fputs(buffer[i], file); // 包括换行符一起写出  
    }  
  
    fclose(file); // 关闭文件  
}

void *RcvMsgTask(void *arg)
{
    int tcpsocket = (int)arg;

    //先读取按钮，看是要登录还是要注册
    //登录成功发1，登录失败发2，注册成功发3，注册失败发4
    char username[32] = {0};
    while(1)
    {
        char msg[128] = {0};
        int ret_val = read(tcpsocket, msg, sizeof(msg));
        if(ret_val <= 0)
        {
            perror("rcv error");
            close(tcpsocket);
            return ;
        }
        char temp2[16] = "denglu";
        char temp1[16] = "zhuce";
        char temp4[16] = "logout";
        if(strcmp(msg, temp1) == 0)
        {
            //接收账号以及密码
            memset(msg, 0, sizeof(msg));
            read(tcpsocket, msg, sizeof(msg));         
            if(!CheakUserInfo(msg))
            {
                char buf[32] = {"register success"};
                msg[strlen(msg)] = '\n';
                registerUser(msg);
                write(tcpsocket, buf, strlen(buf) + 1);                
            }
            else
            {
                char buf[32] = {"register fail"};
                write(tcpsocket, buf, strlen(buf) + 1);
            }
        }
        else if(strcmp(msg, temp2) == 0)  
        {
            memset(msg, 0, sizeof(msg));
            read(tcpsocket, msg, sizeof(msg));
            if(CheakUserInfo(msg))  //登录成功，返回标识，表示登录成功
            {
                char buf[32] = {"login success"};
                write(tcpsocket, buf, strlen(buf) + 1);
                
                //记录username
                for(int i = 0; msg[i] != ' '; i ++ )
                    username[i] = msg[i];

                break;
            }
            else        //登录失败，返回0，
            {
                char buf[32] = {"login fail"};
                write(tcpsocket, buf, strlen(buf) + 1);
            }
        }
        
    }

    while(1)
    {
        
    //    int ret_val = send(tcpsocket, buf, strlen(buf) + 1, MSG_NOSIGNAL);
        char buf[128] = {0};
        int ret_val = read(tcpsocket, buf, sizeof(buf));
        if(ret_val <= 0)
        {
            perror("Rcv Msg error");
            close(tcpsocket); 
            pthread_exit(NULL);
        //    break;
        }
        
        if(strcmp(buf, "goumai") == 0)             
        {
            pthread_mutex_lock(&mutex);
            read(tcpsocket, &user_send_info, sizeof(user_send_info));        
            if(change_medicines_txt())
            {
                puts("购买成功!");
                char temp[16] = {"success"};
                write(tcpsocket, temp, sizeof(temp));
                
                write_to_file();
            }
            else
            {
                puts("购买失败!");
                char temp[16] = {"fail"};
                write(tcpsocket, temp, sizeof(temp));
            }
            pthread_mutex_unlock(&mutex);

        }
        else if(strcmp(buf, "guahao") == 0)
        {
            puts("挂号成功!");
            read(tcpsocket, &user_send_info, sizeof(user_send_info));
            write_to_file();
        }
        else if(strcmp(buf, "shuaxin") == 0)
        {
            puts("刷新成功!");
            shuaxin_func(tcpsocket);
        }
        else if(strcmp(buf, "daying") == 0)        //daying，直接发username过来
        {
            puts("打印成功!");
            printf("%s\n", username);
            init_sendlog(username);                        //生成发票信息
            write(tcpsocket, send_msg, sizeof(send_msg));   //发回去显示发票信息

        }
        else            //打印发票
        {
            // 接收到username
            // 修改发票信息
            int c = change_fapiao_info(username);
        }
    }    
}

int main(int argc, char const *argv[])
{
    //创建线程池
    pool = malloc(sizeof(thread_pool));
    init_pool(pool, 0);
    // 创建tcp套接字
    int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0)
    {
        perror("tcp socke error");
        exit(0);
    }
    
    // 设置好本地的地址信息（IP+PORT）
    struct sockaddr_in ServerAddr = {
        .sin_family = AF_INET ,
        .sin_port = htons(65001),
        .sin_addr.s_addr = INADDR_ANY
    };

    // 把地址信息与套接字进行绑定 （待连接套接字）
    if(bind( tcp_socket , (struct sockaddr*)&ServerAddr , 
                        sizeof(ServerAddr)))
    {
        perror("Bind ");
        exit(0);
    }
    perror("Bind ");
              

    // 设置监听状态（待连接套接字 --> 监听套接字）
    // 处于监听状态下的 套接字拥有等待连接的功能
    if(listen( tcp_socket , 128 ))
    {
        perror("Listen ");
        exit(0);
    }
    perror("Listen ");
    // 等待客户端发起连接请求 （连接成功增加一个已连接套接字）
    //先登录成功。但是登录注册的逻辑可以在收发数据开始去做 
    //  
    while(1)
    {
        struct sockaddr_in FromAddr ;
        socklen_t addrLen  = sizeof(FromAddr);

        tcpScoket[socket_size] = accept( tcp_socket , (struct sockaddr* )&FromAddr  , &addrLen );
        //从线程池中添加新的线程，一个客户端应该用两个线程来处理
        if (tcpScoket[socket_size] < 0)
        {
            perror("accept error");
            exit(0);
        }
        add_thread(pool, 1);
        add_task(pool, RcvMsgTask, (void *)tcpScoket[socket_size]);
        
        socket_size ++ ;

        printf("新客户端1:[%s:%d]\n" , 
                inet_ntoa(FromAddr.sin_addr) , 
                ntohs(FromAddr.sin_port)
                );
    }
    return 0;
}

