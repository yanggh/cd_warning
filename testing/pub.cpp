#include <iostream>
#include <string>
#include <zmq.hpp>
#include <unistd.h>

using namespace std;
int main(int,char**)
{
    zmq::context_t context(1); //若在类中进行封装时，应与socket_t的生命周期一致。
    zmq::socket_t pubSocket(context,ZMQ_PUB); 
    pubSocket.bind("tcp://*:7777");
    
    while(true)
    {
        int  fatal = 0;
        int  fatal_Light = 0;

        int  serious = 0;
        int  serious_Light = 0;

        int  common = 0;
        int  common_Light = 0;

        cout << "input fatal, fatal_Light, serious, serious_Light, common, common_Light:" << endl;
        cin >> fatal >> fatal_Light >> serious >> serious_Light >> common >> common_Light;

        std::string strData = string("{fatal:") + to_string(fatal) + string(",fatal_Light:") + to_string(fatal_Light) + string(",serious:") + to_string(serious) + string(",serious_Light:") + to_string(serious_Light) + string(",common:") + to_string(common) + string(",common_Light:") + to_string(common_Light) + string("}");
        std::cout << "strData = " << strData << std::endl;
        zmq::message_t message(strData.size()+1);
        memcpy(message.data(), strData.data(), strData.size()+1);
        pubSocket.send(message);
        usleep(100);
//        std::cout << "strData = " << strData << std::endl;
//        strData = std::string("{fatal:0,fatal_Light:0,serious:10,serious_Light:227,common:0,common_Light:13}");
//        zmq::message_t message1(strData.size()+1);
//        memcpy(message1.data(), strData.data(), strData.size()+1);
//        pubSocket.send(message1);
//        std::cout << "strData = " << strData << std::endl;
    }
    return 0;
}

