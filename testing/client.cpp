#include <zmq.hpp>
#include <string>
#include <iostream>
 
int main ()
{
    //  Prepare our context and socket
    zmq::context_t context (1);
    zmq::socket_t socket (context, ZMQ_REQ);
 
    std::cout << "Connecting to hello world server…" << std::endl;
    socket.connect ("tcp://localhost:5555");
 
    //  Do 10 requests, waiting each time for a response
    zmq::message_t reply;
    zmq::message_t request;
    int num = 0;

    std::string  data1 = "123456";
    std::string  data2 = "4567890-"; 
    std::string  data3 = "asd";
    std::string  data4 = "asdfasdfasdfasdfadsfasd";
    for (int request_nbr = 0; request_nbr != 10; request_nbr++) {
        request.rebuild(data1.data(), data1.size() + 1);
        std::cout << "Sending: " << (char*)request.data() << "…" << std::endl;
        socket.send (request);
 
        //  Get the reply.
        socket.recv (&reply);
        std::cout << "Received World " << request_nbr << std::endl;

        request.rebuild(data2.data(), data2.size() + 1);
        std::cout << "Sending: " << (char*)request.data() << "…" << std::endl;
        socket.send (request);

        //  Get the reply.
        socket.recv (&reply);
        std::cout << "Received World " << request_nbr << std::endl;


        request.rebuild(data3.data(), data3.size() + 1);
        std::cout << "Sending: " << (char*)request.data() << "…" << std::endl;
        socket.send (request);

        //  Get the reply.
        socket.recv (&reply);
        std::cout << "Received World " << request_nbr << std::endl;

        request.rebuild(data4.data(), data4.size() + 1);
        std::cout << "Sending: " << (char*)request.data() << "…" << std::endl;
        socket.send (request);

        //  Get the reply.
        socket.recv (&reply);
        std::cout << "Received World " << request_nbr << std::endl;

    }
    return 0;
}
