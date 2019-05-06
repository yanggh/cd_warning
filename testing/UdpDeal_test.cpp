#include <thread>
#include "Queue.h"
#include "UdpDeal.h"

MyQueue<std::string> myqueue;

int main(int argc, char** argv)
{
    std::thread  th1(UdpDeal);
    th1.join();    
    return 0;
}
