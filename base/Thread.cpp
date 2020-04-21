#include "base/Thread.hpp"
#include "base/ErrorLog.hpp"

#include <assert.h>

namespace chatRoom{

    void* runInStart(void* obj){
        if(obj){
            Thread* p = static_cast<Thread*>(obj);
            p->callFunc();
        }
        return NULL;
    }

    Thread::~Thread(){
        if(started_ && !joined_ ){
            pthread_detach(threadID_);
        }
    }

    void Thread::start(){
        if(started_) return;
        started_ = true;
        if(pthread_create(&threadID_,
                            nullptr,
                            runInStart,
                            (void*)this) != 0)    
        {
            started_ = false;
            coutErrorLog << "pthread_create error" << std::endl;
        }
    }

    int Thread::join(){
        assert(started_);
        assert(!joined_);
        joined_ = true;
        return pthread_join(threadID_, NULL);
    }
}