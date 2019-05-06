#include <thread>
#include "Zmq.h"
#include "Queue.h"

#ifdef __UDPDEAL_
#include "UdpDeal.h"
#endif

#ifdef __SNMPDEAL_
#include "RecvSnmp.h"
#endif

#ifdef __ISCS__
#include "Iscs.h"
#endif

using namespace std;

extern MyQueue<std::string> myqueue;

int main(int argc, char**argv)
{
#ifdef __SNMPDEAL_
    MySnmp    mysnmp;
#endif

    //给界面推送zmq处理
    thread  th1(ConsumerTask);

#ifdef __UDPDEAL_
    //Udp接收处理
    thread  th2(UdpDeal);
#endif

#ifdef __ISCS__
    //综合监控程勋
    thread  th3(ModBus_Deal);
#endif

    th1.join();
#ifdef __UDPDEAL_
    th2.join();
#endif

#ifdef __ISCS__
    th3.join();
#endif

    return 0;
}
