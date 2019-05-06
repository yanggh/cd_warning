#include <string>
#include "Queue.h"
#include "Nodes.h"

MyQueue<std::string> myqueue;
int main(int argc, char** argv)
{
    MyNodes* mynodes = MyNodes::getInstance();

    std::cout << "====================show======================" << std::endl;
    mynodes->Show();
    mynodes->HeartBeat("10.71.15.222", 8682);
    std::cout << "====================show======================" << std::endl;
    mynodes->Show();
    return 0;
}
