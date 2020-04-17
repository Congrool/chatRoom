#include "base/ThreadPool.hpp"

#include <type_traits>
#include <memory>
#include <assert.h>

namespace chatRoom
{
    threadPool::threadPool(int num)
    : numOfThreads(num),
     started_(false),
     mutex_(),
     cond_(mutex_)
    { }

    threadPool::~threadPool(){
        {
            mutexGuard lock(mutex_);
            started_ = false;
        }
        cond_.notifyAll();
        for(thread& worker : workers_)
            worker.join();
    }

    void threadPool::start(){
        {
            mutexGuard lock(mutex_);
            assert(started_ == false);
            started_ = true;
        }
        for(int i = 0; i < numOfThreads; ++i){
            thread t(
                [this](){
                    for(;;){
                        taskFunc task;
                        {
                            mutexGuard lock(mutex_);
                            while(this->started_ && this->tasks_.empty())
                                this->cond_.wait();
                            if(!this->started_ && this->tasks_.empty())
                                return;
                            task = std::move(tasks_.front());
                            tasks_.pop();
                        }
                        task();
                    }
                }
            );
            // Errors are raised if passed with std::move
            workers_.push_back(t);
            workers_[i].start();
        }
    }

} // namespace chatRoom
