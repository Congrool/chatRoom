#ifndef CHATROOM_BASE_THREAD_HPP
#define CHATROOM_BASE_THREAD_HPP 0

#include "base/noncopyable.hpp"

#include<pthread.h>
#include<string>
#include<functional>
#include<unistd.h>
#include<syscall.h>
#include<type_traits>
#include<memory>
#include<future>

namespace chatRoom{

class Thread{
	public:
		typedef std::function<void()> Func;

		template<typename Fn,typename... Args>
		Thread(Fn&& f, Args&&... args)
		: threadID_(0),
		name_(),
		started_(false),
		joined_(false),
		func_(std::bind(std::forward<Fn>(f),std::forward<Args>(args)...))
		{ }
		
		~Thread();

		void start();	// call pthread_create
		int join();
		pthread_t self() { return threadID_; }
		bool equal(Thread& t) { return pthread_equal(this->threadID_,t.self()) != 0;}
		const std::string& name() { return name_; }
		pid_t	tid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }
		void setName(const std::string& name) { name_ = std::move(name); }

		void callFunc(){
			func_();
		}

	private:
		pthread_t		threadID_;
		std::string 	name_;
		Func		 	func_;
		bool 			started_;
		bool			joined_;
};

}

#endif // CHATROOM_BASE_THREAD_HPP