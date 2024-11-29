#include "../inc/client.hpp"

Client::Client(int dk)
{
    tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (tcp_socket < 0)
    {
        perror("tcp socket error");
    }

    // 设置服务器的地址信息
    struct sockaddr_in ServerAddr = {0};
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_port = htons(dk);
    ServerAddr.sin_addr.s_addr = inet_addr(ADDR);

    // 主动发起连接 [阻塞]
    if(connect( tcp_socket , (struct sockaddr *)&ServerAddr , 
                        sizeof(ServerAddr) ))
    {
        perror("connect error");
        close(tcp_socket);
    }
    printf("连接服务器成功...\n");  
}


Client::~Client()
{
    close(tcp_socket);
}