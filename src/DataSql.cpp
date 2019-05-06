#include "DataSql.h"
#include "Conf.h"
MyDataSql::MyDataSql()
{
}

MyDataSql::~MyDataSql()
{
}

void MyDataSql::GetSqlRes(const std::string sql)
{
    MyConf* myconf = MyConf::getInstance();

    std::string mysqlip =  myconf->get_mysql_ip();
    std::string username = myconf->get_username();
    std::string password = myconf->get_password();
    std::string database = myconf->get_database();

    mysql = mysql_init(NULL);
    if(!mysql_real_connect(mysql, mysqlip.c_str(), username.c_str(), password.c_str(), database.c_str(), 0, NULL, 0))
    {
        syslog(LOG_INFO, "mysql_real_connect error, ip is %s, username is %s,  password is %s, error: %s", mysqlip.c_str(), username.c_str(), password.c_str(), mysql_error(mysql));
        exit(-1);
    }

    int ret =mysql_real_query(mysql, sql.c_str(), sql.length());
    if(ret == 0)
    {
       my_res = mysql_store_result(mysql);//返回查询的全部结果集
       while(NULL != (row = mysql_fetch_row(my_res)))
       {
           DataHandle(row);
       }
    }
    else
    {
        syslog(LOG_INFO, "ip is %s, username is %s,  password is %s, error: %s", mysqlip.c_str(), username.c_str(), password.c_str(), mysql_error(mysql));
        syslog(LOG_ERR, "mysql_real_query error, sql is %s", sql.c_str());
        exit(-1);
    }

    mysql_free_result(my_res);
    mysql_close(mysql);
    mysql = NULL;
    mysql_library_end();
}
