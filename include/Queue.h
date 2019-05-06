#ifndef  _MyQueue_H
#define  _MyQueue_H
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>
template<typename T>
class MyQueue{
public:
    MyQueue(){}
    MyQueue(MyQueue const& other)
    {
        std::lock_guard<std::mutex> lk(other.mtx);
        data_cond = other.qdata;
    }

    ~MyQueue(){
        std::lock_guard<std::mutex>  lk(mtx);
        while(!qdata.empty())
        {
            qdata.pop();
        }
    }
    
    void push_data(const T ss)
    {
        std::lock_guard<std::mutex>  lk(mtx);
        qdata.push(ss);
        data_cond.notify_one();
    }

    void wait_and_pop(T& value)
    {
        std::unique_lock<std::mutex> lk(mtx);
        data_cond.wait(lk, [this]{return !qdata.empty();});
        value = qdata.front();
        qdata.pop();
    }

    std::shared_ptr<T> wait_and_pop()
    {
        std::unique_lock<std::mutex> lk(mtx);
        qdata.wait(lk, [this]{ return !qdata.empty();});
        std::shared_ptr<T> res(std::make_shared<T>(qdata.front()));
        qdata.pop();
        return res;
    }

    bool try_pop(T& value)
    {
        std::lock_guard<std::mutex> lk(mtx);
        if(qdata.empty())
            return false;

        value = qdata.front();
        qdata.pop();
        return true;
    }

    std::shared_ptr<T> try_pop()
    {
        std::lock_guard<std::mutex> lk(mtx);
        if(qdata.empty())
            return std::shared_ptr<T>();
        std::shared_ptr<T> res(std::make_shared<T>(qdata.front()));
        qdata.pop();
        return res;
    }

    size_t  size()
    {
        std::lock_guard<std::mutex> lk(mtx);
        return qdata.size();
    }

    bool empty() const
    {
        std::lock_guard<std::mutex> lk(mtx);
        return qdata.empty();
    }
private:
    std::queue<T>  qdata;
    std::mutex   mtx;
    std::condition_variable data_cond;
};

#endif
