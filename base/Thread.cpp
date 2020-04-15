#include "base/Thread.hpp"
#include "base/ErrorLog.hpp"

#include <assert.h>

namespace chatRoom{

    void* runInStart(void* obj){
        if(obj){
            thread* p = static_cast<thread*>(obj);
            p->callFunc();
        }
        return nullptr;
    }

    thread::~thread(){
        if(started_ && !joined_ ){
            pthread_detach(threadID_);
        }
    }

    void thread::start(){
        if(started_) return;
        started_ = true;
        if(pthread_create(&threadID_,
                            nullptr,
                            runInStart,
                            (void*)this))    
        {
            started_ = false;
            coutErrorLog << "pthread_create error" << std::endl;
        }
    }

    int thread::join(){
        assert(started_);
        assert(!joined_);
        joined_ = true;
        return pthread_join(threadID_,nullptr);
    }
}