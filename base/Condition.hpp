#ifndef CHATROOM_BASE_CONDITION_HPP
#define CHATROOM_BASE_CONDITION_HPP 0

#include "base/noncopyable.hpp"
#include "base/Mutex.hpp"

#include<pthread.h>
#include<assert.h>

namespace chatRoom
{

  class condition : noncopyable
	{
		public:
			condition(mutex& mutex) : mutex_(mutex){
				int ret = pthread_cond_init(&cond_, NULL);
				assert(ret == 0);
			}

			~condition(){
				int ret = pthread_cond_destroy(&cond_);
				assert(ret == 0);
			}

			void wait(){
				mutexGuard guard(mutex_);
				int ret = pthread_cond_wait(&cond_, mutex_.getMutexPtr());
				assert(ret == 0);
			}

			void notifyOne(){
				int ret = pthread_cond_signal(&cond_);
				assert(ret == 0);
			}

			void notifyAll(){
				int ret = pthread_cond_broadcast(&cond_);
				assert(ret == 0);
			}

		private:
			mutex&					mutex_;
			pthread_cond_t 	cond_;
	};
	
} // namespace chatRoom



#endif // CHATROOM_BASE_CONDITION_HPP