#ifndef CHATROOM_CLIENT_THREADPOOL_HPP
#define CHATROOM_CLIENT_THREADPOOL_HPP 0

#include <functional>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <future>
#include <assert.h>

namespace chatRoom
{
    class ThreadPool
    {
        public:    
            typedef std::function<void()> taskFunc;

            ThreadPool(int numOfThreads);
            ~ThreadPool();

            void start();

            bool hasStarted();

            template<typename Fn, typename... Args>
            void enqueue(Fn&& f, Args&&... args){
                auto taskPtr = std::make_shared<std::packaged_task<void()> >( 
                    std::bind(
                        std::forward<Fn>(f),
                        std::forward<Args>(args)...
                    )
                );
                {
                    std::unique_lock<std::mutex> lock(mutex_);
                    assert(started_ == true);
                    tasks_.emplace(
                        [taskPtr](){
                            (*taskPtr)();
                        }
                    );
                }
                cond_.notify_one();
            }

        private:
            std::vector<std::thread>    workers_;
            std::queue<taskFunc>        tasks_;

            const int   numOfThreads_;
            bool        started_;

            std::mutex              mutex_;
            std::condition_variable cond_;
    };
} // namespace chatRoom


#endif