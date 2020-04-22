#include "server/Acceptor.hpp"

#include <assert.h>

namespace chatRoom
{
    Acceptor::Acceptor(int fd, NetAddress& listenAddr)
    : acceptSockfd_(fd),
    acceptChannel_(fd)
    { 
        acceptSockfd_.bind(listenAddr);
        acceptChannel_.setReadEventCallback(
            std::bind(handleRead,this));
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
        acceptSockfd_.listen();
        acceptChannel_.enableReading();
    }
    
    void Acceptor::addAcceptChannel(Poller& poller){
        poller.updateChannel(
            std::make_shared<Channel>(acceptChannel_));
    }
} // namespace chatRoom
