#ifndef _MyNodes_H
#define _MyNodes_H
#include <vector>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DataSql.h"
#include "Conf.h"
#include "Node.h"
#include "Common.h"
using namespace std;

class MyNodes: public MyDataSql
{
public:
    static MyNodes* getInstance();
    ~MyNodes();

    void   DataHandle(MYSQL_ROW row);
    void   HeartBeat(const string& ip, const uint16_t& port);
    void   UpdateNodes(int sockfd);

    void   Show();
private:
    std::vector<Node>   nms;
    MyNodes();
    MyNodes(const MyNodes&);
    MyNodes(std::string sql);

    int                 _timeout;
};

//ostream &operator<<(ostream &out, MyNodes* mynodes);

#endif
