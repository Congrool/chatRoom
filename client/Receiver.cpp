#include "client/Receiver.hpp"
#include "base/ErrorLog.hpp"

#include <unistd.h>
#include <algorithm>
#include <climits>

namespace chatRoom
{
    Receiver::Receiver(int fd)
    : fd_(fd),
    buffer_(1024)
    { }

    // TODO:
    Receiver::~Receiver()
    {
        
    }

    int Receiver::receive(){
        readDataFromFd();
        auto msgs = getMsgs();
        if(receiveCallback_)
            receiveCallback_(msgs);
    }

    void Receiver::readDataFromFd(){
        int hasRead = 0;
        char tmpBuff[1024];
        while((hasRead = ::read(fd_,tmpBuff,1024)) > 0){
            buffer_.insert(buffer_.end(),tmpBuff,tmpBuff+hasRead);
        }

        if(hasRead < 0){
            coutErrorLog << "Receiver: readDataFromFd error";
            // handle error
        }
    }


    std::vector<std::string> Receiver::getMsgs(){
        std::vector<std::string> msgs;
        auto lastPos = buffer_.begin();
        decltype(lastPos) pos;
        while(true){
            pos = std::search_n(lastPos,buffer_.end(),1,'\0');
            if(pos == buffer_.end())
                break;
            msgs.emplace_back(std::move(std::string(lastPos,pos)));
        }
        return msgs;
    }

} // namespace chatRoom
