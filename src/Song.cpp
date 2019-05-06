#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdint>
#include <string.h>
#include <string>
#include <iostream>
#include <zmq.hpp>
#include <syslog.h>
#include <thread>
#include <atomic>
#include "Queue.h"
#include "Socket.h"
#include "Conf.h"

std::atomic<bool> on_off(false);
static  MyQueue<std::pair<int, int>> slqueue;

int  read_write_udp(const uint8_t* data, int len){

    MyConf*     myconf = MyConf::getInstance();
    string ip = myconf->get_sound_ip();
    int  port = myconf->get_sound_port();

    struct sockaddr_in ser_addr;
    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    inet_aton(ip.c_str(), (struct in_addr *)&ser_addr.sin_addr);
    ser_addr.sin_port = htons(port);

    int  sock_fd = socket(AF_INET, SOCK_DGRAM, 0);

    sendto(sock_fd, data, len, 0, (struct sockaddr*)&ser_addr, sizeof(ser_addr));

    for(int i = 0; i < len; i++)
        printf("%02x ", data[i]);

    printf("\n");
    return 0;
}

int  HardWare(const uint8_t *RGB_SO, const uint8_t *RGB_SC, const uint8_t *RGB_O, const uint8_t *RGB_C)
{
    usleep(500000);
    read_write_udp(RGB_SO, 10);
    if(memcmp(RGB_C, RGB_O, 10) == 0)
    {
        usleep(500000);
        read_write_udp(RGB_C, 10);
    }
    else
    {
        on_off = false;
        for(int i = 0; i < 10; i++)
        {
            usleep(500000);
            read_write_udp(RGB_C, 10);

            usleep(500000);
            read_write_udp(RGB_O, 10);

            if(on_off)
            {                
                break;
            }
        }
    }
    usleep(500000);
    read_write_udp(RGB_SC, 10);
    return 0;
}

void Sig_light(const int& rgb, const int& flag)
{
    uint8_t  RGB_SC[10] = {0XCC, 0XDD, 0XA1, 0X01, 0X00, 0X00, 0X00, 0X38, 0XDA, 0XB4};
    uint8_t  RGB_O[10] = {0XCC, 0XDD, 0XA1, 0X01, 0X00, 0X00, 0X00, 0X07, 0XA9, 0X52};
    uint8_t  RGB_C[10] = {0XCC, 0XDD, 0XA1, 0X01, 0X00, 0X00, 0X00, 0X07, 0XA9, 0X52};
    uint8_t  RGB_SO[10] = {0XCC, 0XDD, 0XA1, 0X01, 0X00, 0X00, 0X00, 0X07, 0XA9, 0X52};
    
    /****************************************/
    RGB_O[4] = 0;
    RGB_O[5] = rgb & 0x07;
    RGB_O[6] = 0;
    RGB_O[7] = flag & 0x07;
    RGB_O[8] = (0xa2 + RGB_O[5] + RGB_O[7]) & 0xff;
    RGB_O[9] = (0xa2 + RGB_O[5] + RGB_O[7] + RGB_O[8]) & 0xff;

    /****************************************/
    RGB_C[4] = 0;
    RGB_C[5] = 0;
    RGB_C[6] = 0;
    RGB_C[7] = RGB_O[7];
    RGB_C[8] = (0xa2 + RGB_C[7]) & 0xff;
    RGB_C[9] = (0xa2 + RGB_C[7] + RGB_C[8]) & 0xff;

    /****************************************/
    RGB_SO[4] = 0;
    if(rgb & 0x08)
    {
        RGB_SO[5] = 0x08;
    }
    else if(rgb & 0x10)
    {
        RGB_SO[5] = 0x10;
    }
    else if(rgb & 0x20)
    {
        RGB_SO[5] = 0x20;
    }
    else
    {
        RGB_SO[5] = 0;
    }
   
    RGB_SO[6] = 0;
    RGB_SO[7] = flag & 0x38;
    RGB_SO[8] = (0xa2 + RGB_SO[5] + RGB_SO[7]) & 0xff;
    RGB_SO[9] = (0xa2 + RGB_SO[5] + RGB_SO[7] + RGB_SO[8]) & 0xff;

    HardWare(RGB_SO, RGB_SC, RGB_O, RGB_C);
}

void Song(const int& fatal_l, const int& fatal_s, const int& serious_l, const int& serious_s, const int& common_l, const int& common_s)
{
    static int  fatal_o = 0;
    static int  serious_o = 0;
    static int  common_o = 0;

    int    rgb = 0;
    int    val = 0;
    int    flag = 0;

    if(fatal_l > fatal_o)
    {
        val = (fatal_s == 0 ? 1 : 0x09);
        rgb |= val;
        flag |= 0x09;
    }
    else if(fatal_l == 0 && fatal_o != 0)
    {
        rgb &= 0x36;
        flag |= 0x09;
    }

    if(serious_l > serious_o)
    {
        val = (serious_s == 0 ? 1 : 0x09);
        rgb |= (val << 1);
        flag |= (val << 1);
    }
    else if(serious_l == 0 && serious_o != 0)
    {
        rgb &= 0x2d;
        flag |= 0x12;
    }

    if(common_l > common_o)
    {
        val = (common_s == 0 ? 1 : 0x09);
        rgb |= (val << 2);
        flag |= (val << 2);
    }
    else if(common_l == 0 &&  common_o != 0)
    {
        rgb &= 0x1b;
        flag |= 0x24;
    }

    fatal_o = fatal_l;
    serious_o = serious_l;
    common_o = common_l;

    if(flag != 0)
    {
        syslog(LOG_INFO, "rgb = %02x, flag = %02x\n", rgb, flag);
        on_off = true;
        slqueue.push_data(std::pair<int, int>(rgb, flag));
    }
}

void Deal(const std::string& recvdata)
{
    int     fatal_s = 0, fatal_l = 0;
    int     serious_s = 0, serious_l = 0;
    int     common_s = 0, common_l = 0;
    uint8_t RGB_SC[10] = {0XCC, 0XDD, 0XA1, 0X01, 0X00, 0X00, 0X00, 0X38, 0XDA, 0XB4};

    if(recvdata == std::string("recoverRing") || recvdata == std::string("removeRing"))
    {
        read_write_udp(RGB_SC, 10);
        syslog(LOG_INFO, "removeRing\n");
    }
    else if(recvdata == std::string("update"))
    {
        syslog(LOG_INFO, "son_sys update");
    }
    else
    {
        if(-1 == sscanf(recvdata.c_str(), "%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d%*[^0-9]%d", &fatal_l, &fatal_s, &serious_l, &serious_s, &common_l, &common_s))
        {
            syslog(LOG_INFO, "request = %s", recvdata.c_str());
        }
        else
        {
            syslog(LOG_INFO, "request = %s, fatal = %d: %d, serious = %d:%d, common = %d:%d", recvdata.c_str(), fatal_l, fatal_s, serious_l, serious_s, common_l, common_s);

            Song(fatal_l, fatal_s, serious_l, serious_s, common_l, common_s);
        }
    }
}

void DealPubData()
{
    std::pair<int, int>  value;
    while(1)
    {      
        slqueue.wait_and_pop(value);
        syslog(LOG_INFO, "rgb-flag %02x:%02x\n", value.first, value.second);
        Sig_light(value.first, value.second);
    }
}

void  RecvPubData()
{
    MyConf*     myconf = MyConf::getInstance();
    char  constr[256] = {0};
    string ip = myconf->get_web_ip();
    int    port = myconf->get_pub_port();
    snprintf(constr, 256, "tcp://%s:%d", ip.c_str(), port);
    syslog(LOG_INFO, "%s", constr);

    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_SUB);
    socket.connect(constr);
    const char *filter = "";
    socket.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

    zmq::message_t request;
    std::string  recvdata;
    
    while(1)
    {        
        socket.recv (&request);
        recvdata.assign(static_cast<char*>(request.data()), request.size());
        Deal(recvdata);
    }
}

int Song()
{
    thread  th1(DealPubData);
    thread  th2(RecvPubData);

    th1.join();
    th2.join();
}
