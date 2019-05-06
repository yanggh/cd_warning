#ifndef _MyDataSql_H
#define _MyDataSql_H

#include <mysql.h>
#include <string>
#include <syslog.h>

class MyDataSql{
public:
    MyDataSql();
    virtual ~MyDataSql();
    virtual void  DataHandle(MYSQL_ROW row) = 0;

    void GetSqlRes(const std::string sql);
private:
    MYSQL     *mysql;
    MYSQL_RES *my_res;
    MYSQL_ROW row;
};
#endif
