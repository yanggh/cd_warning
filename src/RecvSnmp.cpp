#include <iostream>
#include <syslog.h>
#include "RecvSnmp.h"
#include "TrapCallBack.h"

using namespace std;

MySnmp::MySnmp()
{
    //----------[ create a SNMP++ session ]-------------------
    int status; 
    Snmp::socket_startup();        // Initialize socket subsystem
    snmp = new Snmp(status);       // check construction status
    if (status != SNMP_CLASS_SUCCESS)
    {
        syslog(LOG_INFO, "SNMP++ Session Create Fail, %s\n", snmp->error_msg(status));
    }

    OidCollection oidc;
    TargetCollection targetc;

    syslog(LOG_INFO, "Trying to register for traps on port %d.\n", trap_port);
    snmp->notify_set_listen_port(trap_port);
    status = snmp->notify_register(oidc, targetc, TrapCallBack, NULL);
    if (status != SNMP_CLASS_SUCCESS)
    {
        syslog(LOG_INFO, "Error register for notify (%d): %s\n", status, snmp->error_msg(status));
    }
    else
    {
        syslog(LOG_INFO, "Waiting for traps/informs...\n");
    }

    snmp->start_poll_thread(2);
}

MySnmp::~MySnmp()
{
    snmp->stop_poll_thread();
    delete snmp;
    Snmp::socket_cleanup();
}
