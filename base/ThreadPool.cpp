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

    template<typename Fn, typename... Args>
    void threadPool::enqueue(Fn&& f, Args&&... args){

        // I don't know if there's another way to check the return type
        if(std::is_same<typename std::result_of<Fn(Args...)>::type, void>::value == true){
            auto task = std::make_shared<std::packaged_task<void()> >(
                std::bind(std::forward<Fn>(f), std::forward<Args>(args)...)
            );
            {
                mutexGuard lock(mutex_);
                assert(started_ == true);
                tasks_.emplace( [task](){
                                    (*task)(); 
                                    });
            }
            cond_.notifyOne();
        }
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
        }
    }

} // namespace chatRoom
