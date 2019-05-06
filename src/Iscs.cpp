#include "Iscs.h"
#include "Conf.h"

const int      NB_CONNECTION = 5;
const uint16_t UT_INPUT_REGISTERS_ADDRESS = 0x01;
const uint16_t UT_INPUT_REGISTERS_NB = 100;

static  const std::string  iscs_1 = "SELECT subsys_.SUBSYSTEM_CODE, IFNULL(sub_.QTY,0) qty FROM TBL_RESMANAGE_SUBSYSTEMINFO subsys_ LEFT JOIN (SELECT b.SUBSYSTEM_CODE,COUNT(*) QTY FROM TBL_ALARM_ALARMINFO a LEFT JOIN TBL_RESMANAGE_SUBSYSTEMINFO b ON a.SUBSYSTEMID=b.SUBSYSTEMID WHERE a.ALARM_STATE=0 AND a.FAULTID!=\"\" AND a.FAULTUNITID!=\"\" AND a.SUBSYSTEMID!=\"\" AND a.STATIONID!=\"\"  GROUP BY a.SUBSYSTEMID  LIMIT  8) sub_ ON subsys_.SUBSYSTEM_CODE=sub_.SUBSYSTEM_CODE LIMIT 9;";

static const  std::string  iscs_2  =  "SELECT        station.STATION_NAME,   IFNULL(q.QTY,0) qty  FROM TBL_RESMANAGE_STATIONINFO station LEFT JOIN(SELECT IFNULL(count(*),0) QTY,alarm.STATIONID FROM TBL_ALARM_ALARMINFO  alarm, TBL_RESMANAGE_FAULTINFO  fault ,TBL_RESMANAGE_SUBSYSTEMINFO system  WHERE alarm.FAULTID=fault.FAULTID AND fault.FAULT_CODE=4226 AND alarm.SUBSYSTEMID=system.SUBSYSTEMID AND system.SUBSYSTEM_CODE=8 AND alarm.ALARM_STATE!=2 GROUP BY  STATIONID) q ON station.STATIONID=q.STATIONID WHERE STATION_ORDER_NO <> 97 AND STATION_ORDER_NO <> 98 AND STATION_ORDER_NO <> 99 ORDER BY station.STATION_ORDER_NO asc  LIMIT 34";
    
static  MyIscs myiscs(iscs_1, iscs_2);

MyModBus::MyModBus(const std::string ip, const int port, const int slave)
{
    ctx = modbus_new_tcp((const char*)(ip.c_str()), port);
    if(ctx == NULL)
    {
        syslog(LOG_INFO, "%s-%d:modbus_new_tcp(%s:%d) fail", __FILE__, __LINE__, ip.c_str(), port);
        modbus_free(ctx);
        ctx = NULL;
        exit(-1);
    }

    mb_mapping = modbus_mapping_new_start_address( 
            0, 0,
            0, 0,
            0, 0,
            UT_INPUT_REGISTERS_ADDRESS, UT_INPUT_REGISTERS_NB);
    if (mb_mapping == NULL) 
    {
        syslog(LOG_INFO, "%s-%d: modbus_mapping_new_start_address fail", __FILE__, __LINE__);
        exit(-1);
    }

    for(int i = 0; i < UT_INPUT_REGISTERS_NB; i++)
    {
        mb_mapping->tab_input_registers[i] = 1;
    }

    modbus_set_slave(ctx, slave);
    modbus_set_debug(ctx, FALSE);
    server_socket = modbus_tcp_listen(ctx, NB_CONNECTION);
    if (server_socket == -1) 
    {
        syslog(LOG_ERR, "%s-%d: modbus_tcp_listen fail", __FILE__, __LINE__);
        modbus_free(ctx);
        ctx = NULL;
        exit(-1);
    }
}

MyModBus::~MyModBus()
{
    if (server_socket != -1) 
    {
        close(server_socket);
        server_socket = -1;
    }

    if(ctx != NULL)
    {
        modbus_free(ctx);
        ctx = NULL;
    }

    if(mb_mapping != NULL)
    {
        modbus_mapping_free(mb_mapping);
    }
}

void  MyModBus::ModSelect()
{
    fd_set refset;
    fd_set rdset;
    int    rc = -1;
    int    fdmax = -1;
    int    master_socket = -1;

    /* Clear the reference set of socket */
    FD_ZERO(&refset);
    /* Add the server socket */
    FD_SET(server_socket, &refset);

    /* Keep track of the max file descriptor */
    fdmax = server_socket;
    while(1)
    {
        rdset = refset;
        if (select(fdmax+1, &rdset, NULL, NULL, NULL) == -1) 
        {
            syslog(LOG_ERR, "Server select() failure.");
            return;
        }

        /* Run through the existing connections looking for data to be read */
        for (master_socket = 0; master_socket <= fdmax; master_socket++) 
        {
            if (!FD_ISSET(master_socket, &rdset)) 
            {
                continue;
            }

            if (master_socket == server_socket) 
            {
                /* A client is asking a new connection */
                socklen_t addrlen;
                struct sockaddr_in clientaddr;
                int newfd;

                /* Handle new connections */
                addrlen = sizeof(clientaddr);
                memset(&clientaddr, 0, sizeof(clientaddr));
                newfd = accept(server_socket, (struct sockaddr *)&clientaddr, &addrlen);
                if (newfd == -1) 
                {
                    syslog(LOG_ERR, "Server accept() error");
                }
                else 
                {
                    FD_SET(newfd, &refset);

                    if (newfd > fdmax) 
                    {
                        /* Keep track of the maximum */
                        fdmax = newfd;
                    }
                }
            } 
            else 
            {
                modbus_set_socket(ctx, master_socket);
                rc = modbus_receive(ctx, query);

                if (rc > 0) 
                {
                    while(mb_mapping == NULL)
                    {
                        syslog(LOG_INFO, "mb_mapping is NULL!\n");
                        sleep(1);
                    }

                    modbus_reply(ctx, query, rc, mb_mapping);
                }
                else if (rc == -1) 
                {
                    /* This example server in ended on connection closing or
                     * any errors. */
                    syslog(LOG_INFO, "(Connection closed on socket %d", master_socket);
                    close(master_socket);

                    /* Remove from reference set */
                    FD_CLR(master_socket, &refset);

                    if (master_socket == fdmax) 
                    {
                        fdmax--;
                    }
                }
            }
        }
    }
}

void  MyModBus::SetMapVal(int pos, int val)
{
    if(val > 0)
        mb_mapping->tab_input_registers[pos] = 1;
    else 
        mb_mapping->tab_input_registers[pos] = 0;
}

MyIscs::MyIscs(std::string sql1, std::string sql2)
{
    _sub = 0;
    _sql1 = sql1;
    _sql2 = sql2;
}

void MyIscs::DataHandle(MYSQL_ROW row)
{
    int val = atoi(row[1]) > 0 ? 1 : 0;
    modbus1->SetMapVal(_sub, val);
    modbus2->SetMapVal(_sub, val);
    _sub++;
}

void  MyIscs::Iscs_Work(const std::string ip1, const int port1, const std::string ip2, const int port2)
{
    modbus1 = new MyModBus(ip1, port1, 1);
    modbus2 = new MyModBus(ip2, port2, 2);

    while(1)
    {
        std::this_thread::sleep_for(std::chrono::seconds (5));
        GetSqlRes(_sql1);
        GetSqlRes(_sql2);
        _sub = 0;
    }

    delete modbus1;
    delete modbus2;
}

int  Iscs_Deal()
{
    MyConf* myconf = MyConf::getInstance();

    std::string  major_ip = myconf->get_modbus_major_ip();
    std::string  min_ip = myconf->get_modbus_min_ip();
    
    int  major_port = myconf->get_modbus_major_port();
    int  min_port =   myconf->get_modbus_min_port();


    cout << "major_ip:" << major_ip << ", major_port:" << major_port << endl;
    cout << "min_ip:" <<   min_ip   << ", min_port:"   << min_port   << endl;
    myiscs.Iscs_Work(major_ip, major_port, min_ip, min_port);
    cout << "Iscs_Work end" << endl;
    return 0;
}

int  Mod1_Service()
{
    myiscs.ModSelect1(); 
    cout << "Mod1 end" << endl;
}

int  Mod2_Service()
{
    myiscs.ModSelect2(); 
    cout << "Mod2 end" << endl;
}

void  ModBus_Deal()
{
    std::thread  th1(Iscs_Deal);
    sleep(1);
    std::thread  th2(Mod1_Service);
    std::thread  th3(Mod2_Service);

    th1.join();
    th2.join();
    th3.join();

    cout << "ModBus_Deal" << endl;
}
