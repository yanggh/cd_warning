#include <string.h>
#include <unistd.h>
#include <iostream>
#include <thread>
#include <syslog.h>
#include "Socket.h"

static const int nRecvBuf = 1024;       //设置成5M
static const int nSendBuf = 1024;
using  namespace std;
MySocket::MySocket(std::string ip, int port):_ip{ip}, _port{port}
{
    struct sockaddr_in sin;

    if(((_sockfd = socket(AF_INET, SOCK_DGRAM, 0))< 0))
    {
        syslog(LOG_ERR, "create  socket error");
        exit(-1);
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family=AF_INET;
    sin.sin_port=htons(port);
//    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_addr.s_addr = inet_addr(_ip.c_str());

    if(bind(_sockfd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in)) < 0)
    {
        syslog(LOG_ERR, "bind udp service error, ip = %s, port = %d, error = %s\n", _ip.c_str(), _port,  strerror(errno));
        exit(-1);
    }

    setsockopt(_sockfd,SOL_SOCKET, SO_RCVBUF, (const char *)&nRecvBuf,sizeof(int));
    setsockopt(_sockfd,SOL_SOCKET, SO_SNDBUF, (const char *)&nSendBuf,sizeof(int));
}

MySocket::~MySocket()
{
    if(_sockfd != -1)
    {
        close(_sockfd);
        _sockfd = -1;
    }
}

void MySocket::push_data(struct sockaddr_in& sin, std::string& data)
{
    qsdata.push(std::pair<struct sockaddr_in, std::string>(sin, data));
}

bool MySocket::get_data(struct sockaddr_in& sin, std::string& data)
{
    if(!qsdata.empty())
    {
        sin = qsdata.front().first;
        data = qsdata.front().second;
        qsdata.pop();
        return true;
    }

    return false;
}

int  MySocket::get_sockfd()
{
    return _sockfd;
}

int  MySocket::get_port()
{
    return _port;
}

std::string MySocket::get_ip()
{
    return _ip;
}

