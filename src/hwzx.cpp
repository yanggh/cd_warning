#include <string>
#include <map>
#include <syslog.h>
#include "Snmp.h"
#include "Conf.h"
#include "snmp_pp/snmp_pp.h"
#include "snmp_pp/collect.h"
#include "snmp_pp/notifyqueue.h"

using namespace std;
using namespace Snmp_pp;

static  const string  zte_gw_ip   =  "10.71.20.129";
static  const string  zte_g_cs_ip =  "10.71.14.11";
static  const string  zte_7_cs_ip =  "10.71.14.12";

bool  zxGetData(Pdu& pdu, int& son_sys, int& bus, string& jk, string& cw, int& code, string& atime)
{
    MyConf* myconf = MyConf::getInstance();
    map<string, int> codemap = myconf->get_zx_code();
    map<string, int> ipmap = myconf->get_ip_bus();
    Vb  nextVb;
    Oid oid;
    string acode; //原始code

    char  value[1024];
    int   codeflag = 0;
    for(int i = 0; i < pdu.get_vb_count(); i++)
    {
        pdu.get_vb(nextVb, i);
        memset(value, 0, 1024);
        nextVb.get_value(value);
        nextVb.get_oid(oid);
           
        syslog(LOG_INFO, "i = %d, oid = %s, value = %s\n", i, oid.get_printable(), value);
        /*************中兴设备********************/
        if(oid  == Oid(zxEventNotify["zxEvType"].c_str()))
        {
            if(zxEvType["zxWarning"] == value)
            {
                codeflag = 0;
            }
            else if((zxEvType["zxRecover"] == value) && (zxEvType["zxConfirm"] == value))
            {
                codeflag = 4096;
            }
        } 
        else if(oid  == Oid(zxEventNotify["zxEvTime"].c_str()))
        {
            char tmp[1024];
            bzero(tmp, 1024);
            int year, mon, day, hh, mm, ss;
            sscanf(value, "%04d-%02d-%02d, %02d:%02d:%02d*", &year, &mon, &day, &hh, &mm, &ss); 
            snprintf(tmp, 1024, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hh, mm, ss);
            atime = tmp;
        }
        else if(oid  == Oid(zxEventNotify["zxAlarmCode"].c_str()))
        {
            acode = value;
        }
        else if(oid  == Oid(zxEventNotify["zxNetype"].c_str()))
        {
            if(strstr(value, "ngn_ss") != NULL)
            {
                son_sys = 401; 
            }
            else  if(strstr(value, "ngn_msg9000") != NULL)
            {
                son_sys = 414; 
            }
            else  if(strstr(value, "ZXMP S385") != NULL)
            {
//                if(memcmp(heartip, zte_g_cs_ip.c_str(), strlen(zte_g_cs_ip.c_str())) == 0)
//                {
//                    son_sys = 12; 
//                }
//                else  if(memcmp(heartip, zte_7_cs_ip.c_str(), strlen(zte_7_cs_ip.c_str())) == 0)
//                {
//                    son_sys = 1; 
//                }
            }
            else  if(strstr(value, "MSAG5200V3") != NULL)
            {
                son_sys = 425;
            }
        }
        else if(oid  == Oid(zxEventNotify["zxInstanceName"].c_str()))
        {
            if(son_sys == 401 || son_sys == 425 || son_sys == 414)
            {
                if(son_sys == 401)
                {
                    jk = "1";
                }
                else if(son_sys == 414)
                {
                    jk = "2";
                }
                else if(son_sys == 425)
                {
                    jk = "3";
                }
                
                int  jk1 = 0;
                int  cw1 = 0;
                sscanf(value, "%*[^=]=%*[^=]=%d,%*[^=]=%d,%*s", &jk1, &cw1);
                cw = jk1 + "-" + cw1;
            }
            else if(son_sys == 12 || son_sys == 1)
            {
                char cw1[256];
                bzero(cw1, 256);
                if(strstr(value, "(") != NULL)
                {
                    sscanf(value, "%*[^,],%[^(]s", cw1);
                }
                else
                {
                    sscanf(value, "%*[^,],%[^#]s", cw1);
                }
                cw = cw1;
            }
        }
        else if(oid  == Oid(zxEventNotify["zxNeIP"].c_str()))
        {
            bus = ipmap[value];
            if(son_sys == 1 || son_sys == 12)
            {
                jk = value;
            }
        }
    }

    if(son_sys == 401 || son_sys == 425 || son_sys == 414 || son_sys == 1 || son_sys == 12)
    { 
        if(son_sys == 401)
        {
            acode = "401" + acode;
            son_sys = 2;
        }
        else if(son_sys == 414)
        {
            acode = "414" + acode;
            son_sys = 2;
        }
        else if(son_sys == 425)
        {
            acode = "425" + acode;
            son_sys = 2;
        }
        code = codemap[acode] - codeflag;

        return true;
    }
    else
    {
        return false;            
    }
}
//华为设备
bool  hwGetData(Pdu& pdu, int& son_sys, int& bus, string& jk, string& cw, int& code, string& atime)
{
    Vb      nextVb;
    Oid     oid;
    string  acode; //原始code

    char    value[1024];
    for(int i = 0; i < pdu.get_vb_count(); i++)
    {
        pdu.get_vb(nextVb, i);
        memset(value, 0, 1024);
        nextVb.get_value(value);
        nextVb.get_oid(oid);
        syslog(LOG_INFO, "i = %d, oid = %s, value = %s\n", i, oid.get_printable(), value);
        /*************华为设备********************/
        //槽位信息
        if(oid  == Oid(hwEventNotify["hwNePos"].c_str()))
        {
            //jk cw
        }
        //告警时间
        else  if(oid  == Oid(hwEventNotify["hwEvTime"].c_str()))
        {
            //atime
            char tmp[1024];
            bzero(tmp, 1024);
            int year, mon, day, hh, mm, ss;
            sscanf(value, "%04d/%02d/%02d %02d:%02d:%02d*", &year, &mon, &day, &hh, &mm, &ss);
            snprintf(tmp, 1024, "%04d-%02d-%02d %02d:%02d:%02d", year, mon, day, hh, mm, ss);
            atime = tmp;
        }
        //告警,告警恢复标志
        else  if(oid  == Oid(hwEventNotify["hwFaultFlag"].c_str()))
        {
            //acode flat
        }
        //设备IP
        else  if(oid  == Oid(hwEventNotify["hwDevIP"].c_str()))
        {
            //bus       
        }
        //告警ID
        else  if(oid  == Oid(hwEventNotify["hwReaId"].c_str()))
        {
            //acode + acode
        }
    }
}
