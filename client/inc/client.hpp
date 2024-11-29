#ifndef _CLIENT_H_
#define _CLIENT_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h> /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip.h> /* superset of previous */
#include <unistd.h>
#define ADDR "192.168.53.23"

class Client{
private:
    int tcp_socket;
public:
    Client(int dk);
    ~Client();
    int Get_Socket(){ return tcp_socket; }
};


#endif