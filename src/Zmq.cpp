#include <zmq.hpp>
#include <syslog.h>
#include <list>
#include <thread>
#include "Zmq.h"
#include "Conf.h"
#include "Queue.h"
using namespace std;

MyQueue<std::string> myqueue;

int ConsumerTask()
{
    MyConf* myconf = MyConf::getInstance();

    string ip = myconf->get_web_ip();
    int  port = myconf->get_web_port();

    zmq::context_t context (1);
    zmq::socket_t  socket (context, ZMQ_REQ);

    char   constr[256] = {0};
    snprintf(constr, 256, "tcp://%s:%d", ip.data(), port);
    syslog(LOG_INFO, "constr = %s", constr);
    socket.connect (constr);
    
    std::string    data;
    zmq::message_t reply;
    zmq::message_t req;

    while(1)
    {
        myqueue.wait_and_pop(data);
        req.rebuild(data.data(), data.size());
        syslog(LOG_INFO, "req data: %s", data.c_str());

        if(socket.send(req) < 0)
        {
            syslog(LOG_ERR, "zmq send error!\n");
        }

        if(socket.recv (&reply) < 0)
        {
            syslog(LOG_ERR, "zmq recv error!\n");
        }
    }
    
    socket.close();
    return 0;
}
