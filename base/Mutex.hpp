#ifndef CHATROOM_BASE_MUTEX_HPP
#define CHATROOM_BASE_MUTEX_HPP 0

#include "base/noncopyable.hpp"

#include <pthread.h>
#include <assert.h>

namespace chatRoom{

	class mutex : noncopyable
	{
		public:
			mutex(){
				int ret = pthread_mutex_init(&mutex_,NULL);
				assert(ret == 0);
			}

			~mutex(){
				int ret = pthread_mutex_destroy(&mutex_);
				assert(ret == 0);
			}

			void lock(){
				int ret = pthread_mutex_lock(&mutex_);
				assert(ret == 0);
			}

			void unlock(){
				int ret = pthread_mutex_unlock(&mutex_);
				assert(ret == 0);
			}

			pthread_mutex_t* getMutexPtr()	{ return &mutex_; }
		private:
			pthread_mutex_t mutex_;
	};

	class mutexGuard : noncopyable{
		public:

			explicit mutexGuard(mutex& mutex) : mutex_(mutex){
				mutex_.lock();
			}

			~mutexGuard(){
				mutex_.unlock();
			}

		private:
			mutex& mutex_;
	};
} // namespace chatRoom

#define mutexGuard(x) error "Name of mutexGurad object is missing"

#endif // CHART_BASE_MUTEX_HPP