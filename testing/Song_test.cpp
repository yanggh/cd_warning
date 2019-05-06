#include <thread>
#include "Song.h"

int main()
{
    std::thread th1(Song);
    
    th1.join();
    return 0;
}
