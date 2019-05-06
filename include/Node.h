#ifndef _MyNode_H
#define _MyNode_H
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

class Node
{
public:
    Node(const uint8_t& ssys = 0, const uint8_t& stop = 0, const std::string& ip={}, const uint16_t& port = 0, const bool& flag = false, const int& timeout = 10);

    friend bool operator==(Node node1, Node node2);
    friend std::ostream& operator<<(std::ostream &out, Node node);
    Node& operator=(Node node);
    void  ResetNode(int timeout);

public:
    uint8_t _ssys;
    uint8_t _stop;
    std::string _ip;
    uint16_t _port;
    int      _flag{0};
    int      _timeout{10};
    struct sockaddr_in _sin;
};
#endif
