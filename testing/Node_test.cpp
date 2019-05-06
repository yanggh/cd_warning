#include "Node.h"
#include <vector>
int main()
{
    std::vector<Node>   nms;

    for(int i = 0; i < 100000; i++)
    {
    Node p(1, 1, "ip", 30, true, 30);
        nms.push_back(p); 
    }
}
