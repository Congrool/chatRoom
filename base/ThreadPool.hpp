#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP 0

#include "base/noncopyable.hpp"
#include "base/Condition.hpp"
#include "base/Thread.hpp"

#include<vector>
#include<queue>

namespace chatRoom
{
	class threadPool : noncopyable
	{
		public:
			typedef std::function<void()>	taskFunc;

			threadPool(int);
			~threadPool();
			
			template<typename Fn, typename... Args>
			void enqueue(Fn&&, Args&&...);

			void start();

			bool hasStarted() { return started_; }

		private:
			std::vector<thread>		workers_;
			std::queue<taskFunc>	tasks_;

			int				numOfThreads;
			bool 			started_;
			// The order of the following two members
			// should not be changed, because of list initialization.
			mutex 		mutex_;
			condition 	cond_;

	};
  
} // namespace chatRoom



#endif // THREAD_POOL_HPP