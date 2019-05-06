#ifndef _MySocket_H
#define _MySocket_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>

#include <string>
#include <queue>

class MySocket{
public:
    MySocket(std::string ip, int port);
    ~MySocket();

    bool  get_data(struct sockaddr_in& sin, std::string& data);
    void  push_data(struct sockaddr_in& sin, std::string& data);
    int   get_sockfd();
    int   get_port();
    std::string get_ip();
protected:
    int              _sockfd{-1};
private:
    std::string      _ip;
    int              _port;
    std::queue<std::pair<struct sockaddr_in, std::string>> qsdata;
};
#endif
