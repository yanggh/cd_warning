#ifndef  _MyRecvSnmp_H
#define  _MyRecvSnmp_H
#include "snmp_pp/snmp_pp.h"
#include "snmp_pp/collect.h"
#include "snmp_pp/notifyqueue.h"

using namespace Snmp_pp;

class MySnmp
{
public:
    MySnmp();
    ~MySnmp();
private:
    int  trap_port{162};
    Snmp *snmp{NULL};
};
#endif


