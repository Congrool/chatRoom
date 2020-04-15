#ifndef CHATROOM_BASE_THREAD_HPP
#define CHATROOM_BASE_THREAD_HPP 0

#include "base/noncopyable.hpp"

#include<pthread.h>
#include<string>
#include<functional>
#include<unistd.h>
#include<syscall.h>
#include<future>
#include<memory>

namespace chatRoom{

class thread : noncopyable{
	public:
		typedef std::packaged_task<void()> threadTask;
		typedef std::shared_ptr<threadTask> threadTaskPtr;
		typedef void*(*threadFunc)();

		template<typename F,typename... Args>
		thread(F&& f, Args&&... args)
		: threadID_(0),
		name_(),
		started_(false),
		joined_(false)
		{
			func_ = std::make_shared<threadTask>(
					std::bind(std::forward<F>(f),std::forward<Args>(args)...)
					);
		}
		
		~thread();

		void start();	// call pthread_create
		int join();
		pthread_t self() { return threadID_; }
		bool equal(thread& t) { return pthread_equal(this->threadID_,t.self()) != 0;}
		const std::string& name() { return name_; }
		pid_t	tid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }
		void setName(const std::string& name) { name_ = std::move(name); }
		void callFunc(){
			(*func_)();
		}

	private:
		pthread_t		threadID_;
		std::string 	name_;
		threadTaskPtr 	func_;
		bool 			started_;
		bool			joined_;

};

}

#endif // CHATROOM_BASE_THREAD_HPP