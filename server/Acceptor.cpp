#include "server/Acceptor.hpp"

#include <assert.h>

namespace chatRoom
{
    Acceptor::Acceptor(int fd, NetAddress& listenAddr, Poller& poller)
    : acceptSockfd_(fd),
    acceptChannel_(fd),
    accpetChannelPtr_(std::make_shared<Channel>(acceptChannel_))
    { 
        acceptSockfd_.bind(listenAddr);
        acceptChannel_.setReadEventCallback(
            std::bind(&Acceptor::handleRead,this)
        );
    }

    void Acceptor::handleRead(){
        sockaddr_in peeraddr;
        int connfd = this->acceptSockfd_.accept(&peeraddr);
        assert(connfd >= 0);
        if(newConnCallback_){
            NetAddress netPeeraddr(&peeraddr);
            newConnCallback_(connfd,netPeeraddr);
        }
    }

    void Acceptor::listen(){
        acceptChannel_.enableReading();
        acceptSockfd_.listen();
    }
    
} // namespace chatRoom
