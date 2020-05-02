#include "client/Sender.hpp"
#include <unistd.h>
#include <algorithm>

namespace chatRoom
{
    Sender::Sender(int fd)
    :fd_(fd)
    { }

    void Sender::send(const char* start, size_t len)
    {   
        int CurPos = 0, lastPos = -1;
        int hasWrite = 0;
        while((hasWrite = ::write(fd_,start+CurPos,len)) > 0){
            lastPos = CurPos;
            CurPos += hasWrite;
            std::vector<std::string> msgs = getMsgs(start+lastPos,start+CurPos);
            if(sendCallback_)
                sendCallback_(msgs);
        }
    }

    std::vector<std::string> getMsgs(const char* start,const char* end)
    {
        std::vector<std::string> msgs;
        const char* pos = nullptr;
        while((pos = std::search_n(start,end,1,'\0')) != end)
            msgs.emplace_back(std::move(std::string(start,pos)));
        return msgs;
    }

    
} // namespace chatRoom
