#include <list>
#include <cstring>
#include <algorithm>
#include "Queue.h"
#include "Nodes.h"

static const bool  DISCON = false;
static const bool  CONNECT = true;

static const int   SYSSUB = 0;
static const int   IPSUB = 1;
static const int   PORTSUB = 2;

extern MyQueue<std::string> myqueue;

//const string  MYSQL_SYS_STR = "SELECT SUBSYSTEM_CODE,SERVER_IP,SERVER_PORT FROM TBL_RESMANAGE_SUBSYSTEMINFO WHERE SUBSYSTEM_CODE != 1 AND SUBSYSTEM_CODE != 2 AND SUBSYSTEM_CODE < 10;";
const string  MYSQL_SYS_STR = "SELECT SUBSYSTEM_CODE,SERVER_IP,SERVER_PORT FROM TBL_RESMANAGE_SUBSYSTEMINFO WHERE SUBSYSTEM_CODE != 13 AND SUBSYSTEM_CODE != 11 ";

const char* KEEP_ALIVE_STR = "{ \"type\": \"0xFFFF\", \"fnum\": \"0\", \"flen\": \"0\", \"son_sys\": \"%d\", \"stop\": \"%d\", \"eng\": \"0\", \"node\":\"0\", \"bug\": \"0\", \"time\": \"0\", \"res1\": \"%s\", \"res2\": \"%d\", \"res3\": \"%d\", \"check\": \"0\"}";

MyNodes* MyNodes::getInstance()
{
    Lock  lck;
    static  MyNodes mynodes(MYSQL_SYS_STR);
    return &mynodes;
}

MyNodes::MyNodes(std::string sql)
{
    MyConf* myconf = MyConf::getInstance();
    _timeout = myconf->get_keepalive();
    GetSqlRes(sql);
}

MyNodes::MyNodes(const MyNodes&)
{

}

MyNodes::MyNodes()
{

}

MyNodes::~MyNodes()
{
}

void MyNodes::DataHandle(MYSQL_ROW row)
{

    uint8_t stop = 1;
    uint8_t ssys = atoi(row[SYSSUB]);
    std::string ip = std::string(row[IPSUB]);
    uint16_t   port = atoi(row[PORTSUB]);

    int     timeout = _timeout;
    bool    flag = DISCON;

    Node p(ssys, stop, ip, port, flag, timeout);

    nms.push_back(p);
}

void  MyNodes::HeartBeat(const string& ip, const uint16_t& port)
{
    for(auto& val : nms)
    {
        if(val._ip == ip)
        {
            syslog(LOG_INFO, "UPDATE Heartbeat : %s, %d, %d, %d, %s, %d, %d", ip.c_str(), port, val._ssys, val._stop, val._ip.c_str(), val._port, val._flag);
            val._timeout = _timeout;

            if(val._flag == DISCON)
            {
                val._flag = CONNECT;

                char  output[1024];
                memset(output, 0, 1024);
                if(snprintf(output, 256, KEEP_ALIVE_STR,  val._ssys, val._stop, val._ip.c_str(), val._port, val._flag) > 0)
                {
                    //TODO ENQUEUE
                    syslog(LOG_INFO, "CONNECT FFFF is %s", output);
                    myqueue.push_data(output);
                }
            }
            break;
        }
    }
}

void MyNodes::UpdateNodes(int sockfd)
{
    //心跳数据
    uint8_t  alive[10];

    time_t tt = time(NULL);
    struct tm *t = localtime(&tt);

    alive[0] = 0xaa;
    alive[1] = 0x0a;

    alive[3] = (t->tm_year + 1900) / 100;
    alive[4] = (t->tm_year + 1900) % 100;
    alive[5] = t->tm_mon + 1;
    alive[6] = t->tm_mday + 1;
    alive[7] = t->tm_hour;
    alive[8] = t->tm_min;
    alive[9] = t->tm_sec; 

    //遍历子系统时间
    for(auto& val : nms)
    {
        syslog(LOG_INFO, "UPDATE: %d, %d, %s, %d, %d", val._ssys, val._stop, val._ip.c_str(), val._port, val._flag);

        if(val._timeout > 0)
        {
            val._timeout --;
        }
        else
        {
            if(val._flag == CONNECT)
            {
                val._flag = DISCON;
    
                char  output[1024];
                memset(output, 0, 1024);
    
                if(snprintf(output, 256, KEEP_ALIVE_STR,  val._ssys, val._stop, val._ip.c_str(), val._port, val._flag) > 0)
                {
                    //TODO ENQUEUE         
                    syslog(LOG_INFO, "DISCON FFFF is %s", output);
                    myqueue.push_data(output);
                }
            }
        }
         
        alive[2] = val._ssys;
        if(val._port != 162)
        {
            sendto(sockfd, alive, 10, 0, (struct sockaddr*)&(val._sin), sizeof(val._sin));
        }
    }
}

void MyNodes::Show()
{
    for(auto& val : nms)
        std::cout << val;
}
