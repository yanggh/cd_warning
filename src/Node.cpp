#include <string.h>
#include "Node.h"

Node::Node(const uint8_t& ssys, const uint8_t& stop, const std::string& ip, const uint16_t& port, const bool& flag, const int& timeout)
{
    _ssys = ssys;
    _stop = stop;
    _ip = ip;
    _port = port;
    _flag = flag;
    _timeout = timeout;

    memset(&_sin, 0, sizeof(_sin));  
    _sin.sin_family=AF_INET;  
    _sin.sin_addr.s_addr=inet_addr(_ip.c_str());
    _sin.sin_port=htons(_port);
}

std::ostream &operator<<(std::ostream &out, Node node)
{
    out << "node._ssys = " << (int)node._ssys << "  ";
    out << "node._stop = " << (int)node._stop << "  "; 
    out << "node._ip = " << node._ip << "  ";
    out << "node._port = " << node._port << "  ";
    out << "node._flag = " << node._flag << "  ";
    out << "node._timeout = " << node._timeout << std::endl;

    return out;
}

bool operator == (Node node1, Node node2)
{
    return (node1._ip == node2._ip);
}

void  Node::ResetNode(int timeout)
{
    this->_timeout = timeout;
}

Node& Node::operator=(Node node)
{
    this->_ssys = node._ssys;
    this->_stop = node._stop;
    this->_ip = node._ip;
    this->_port = node._port;
    this->_flag = node._flag;
    this->_timeout = node._timeout;

    memset(&(this->_sin), 0, sizeof(struct sockaddr_in));  
    this->_sin.sin_family=AF_INET;  
    this->_sin.sin_addr.s_addr=inet_addr(node._ip.c_str());
    this->_sin.sin_port=htons(node._port);

    return *this;
}

