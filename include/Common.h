#ifndef __COMMON__
#define __COMMON__
#include <mutex>
class Lock
{
private:
    std::mutex mtx;
public:
    Lock()
    {
        mtx.lock();
    }

    ~Lock()
    {
        mtx.unlock();
    }
};
#endif
