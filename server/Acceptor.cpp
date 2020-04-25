#include "server/Acceptor.hpp"

#include <assert.h>

namespace chatRoom
{
    Acceptor::Acceptor(int fd, NetAddress& listenAddr)
    : acceptSockfd_(fd),
    acceptChannelPtr_(std::make_shared<Channel>(fd))
    { 
        acceptSockfd_.bind(listenAddr);
        acceptChannelPtr_->setReadEventCallback(
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
        acceptChannelPtr_->enableReading();
        acceptSockfd_.listen();
    }
    
} // namespace chatRoom
