#ifndef _MyIscs_H
#define _MyIscs_H

#include <unistd.h>
#include <thread>
#include <syslog.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <modbus.h>
#include <iostream>
#include <string>
#include "DataSql.h"

class MyModBus{
public:
    MyModBus(const std::string ip, const int port, const int slave);
    ~MyModBus();
    void  ModSelect();

    void  SetMapVal(int pos, int val);
private:
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
    int     server_socket;
    modbus_mapping_t *mb_mapping{NULL};
    modbus_t *ctx{NULL};
};

class MyIscs: public MyDataSql
{
public:
    MyIscs(std::string sql1, std::string sql2);
    void  Iscs_Work(const std::string ip1, const int port1, const std::string ip2, const int port2);

    void  ModSelect1()
    {
        modbus1->ModSelect();
    }

    void  ModSelect2()
    {
        modbus2->ModSelect();
    }

private:
    MyModBus*  modbus1{NULL};
    MyModBus*  modbus2{NULL};
    void  DataHandle(MYSQL_ROW row);

    int  _sub;
    std::string _sql1;
    std::string _sql2;
};

void  ModBus_Deal();
#endif
