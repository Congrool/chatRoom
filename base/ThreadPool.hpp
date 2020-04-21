#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP 0

#include "base/noncopyable.hpp"
#include "base/Condition.hpp"
#include "base/Thread.hpp"

#include<vector>
#include<queue>

namespace chatRoom
{
	class ThreadPool : noncopyable
	{
		public:
			typedef std::function<void()>	taskFunc;

			ThreadPool(int);
			~ThreadPool();
			
			template<typename Fn, typename... Args>
			void enqueue(Fn&& f, Args&&... args){

				// I don't know if there's some way to check the return type
				// if(std::is_same<typename std::result_of<Fn(Args...)>::type, void>::value == true){
					auto task = std::bind(std::forward<Fn>(f), std::forward<Args>(args)...);
					auto taskPtr = std::make_shared<decltype(task)>(task);
					{
						mutexGuard lock(mutex_);
						assert(started_ == true);
						tasks_.emplace([taskPtr](){ (*taskPtr)(); });
					}
					cond_.notifyOne();
				// }
			}

			void start();

			bool hasStarted() { return started_; }

		private:
			std::vector<Thread>		workers_;
			std::queue<taskFunc>	tasks_;

			int				numOfThreads;
			bool 			started_;
			// The order of the following two members
			// should not be changed, because of list initialization.
			Mutex 		mutex_;
			Condition 	cond_;

	};
  
} // namespace chatRoom



#endif // THREAD_POOL_HPP