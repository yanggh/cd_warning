#include <iostream>
#include <string>
#include <zmq.hpp>
int main(int,char**)
{
    zmq::context_t context(1);
    zmq::socket_t subSocket(context,ZMQ_SUB);
    subSocket.connect("tcp://localhost:5555");
    const char *filter = "";
    subSocket.setsockopt(ZMQ_SUBSCRIBE, filter, strlen (filter));

    zmq::message_t request(1024);
    for(int i = 0 ; i < 100000; i++)
    {
        subSocket.recv (&request);
        std::cout << std::string(static_cast<char *>(request.data()), request.size()) << std::endl;
    }
    return 0;
}
