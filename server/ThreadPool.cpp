#include "server/ThreadPool.hpp"

#include <type_traits>
#include <memory>
#include <assert.h>
#include <errno.h>

namespace chatRoom
{
	ThreadPool::ThreadPool(int num)
	:tasks_(), 
	numOfThreads(num),
	started_(false),
	mutex_(),
	cond_(mutex_)
	{ }

	ThreadPool::~ThreadPool(){
		{
			mutexGuard lock(mutex_);
			started_ = false;
		}
		cond_.notifyAll();		
		for(int i = 0; i < numOfThreads; ++i)
			workers_[i].join();
	}

	void ThreadPool::start(){
		started_ = true;
		// Caution!
		// If not preallocate enough space for workers, 
		// error will be arised when moving wokers existing in old space
		// to a new allocated space at which time destructor is invoked.
		// However, destructor for class Thread is specially defined 
		// which may call pthread_detach(). That means the thread is wild 
		// and out of control, but still waiting on cond_. Then, when
		// ThreadPool notifies threads which are waiting on cond_, the
		// wild thread will be waked up as well, which may visit the memory
		// the ThreadPool owns, even after the ThreadPool is destructed.
		// It's dangerous and inconspicuous.
		// It took me several hours to find the problem. :-(
		workers_.reserve(numOfThreads);
		for(int i = 0; i < numOfThreads; ++i){
			workers_.emplace_back(
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
			workers_[i].start();
		}
	}

} // namespace chatRoom
