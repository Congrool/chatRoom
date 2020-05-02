#include "client/ThreadPool.hpp"

namespace chatRoom
{
    ThreadPool::ThreadPool(int numOfThreads)
    : numOfThreads_(numOfThreads),
    started_(false)
    {}

    ThreadPool::~ThreadPool()
    {
        {
            std::unique_lock<std::mutex>(mutex_);
            started_ = false;
        }
        cond_.notify_all();
        for(auto& it:workers_)
            it.join();
    }

    void ThreadPool::start()
    {
        started_ = true;    
        workers_.reserve(numOfThreads_);
        for(int i = 0; i < numOfThreads_; ++i)
            workers_.emplace_back(
                [this](){
                    for(;;){
                        taskFunc task;
                        {
                            std::unique_lock<std::mutex> lock(mutex_);
                            this->cond_.wait(lock,
                                [this](){ 
                                    return !this->started_ || !this->tasks_.empty();
                                }
                            );
                            if(!this->started_ && tasks_.empty())
                                return;
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        task();
                    }
                }
            );
    }   
} // namespace chatRoom
