#include "server/Acceptor.hpp"

#include <assert.h>

namespace chatRoom
{
    Acceptor::Acceptor(int fd, NetAddress& listenAddr, Poller& poller)
    : acceptSockfd_(fd),
    acceptChannel_(fd),
    accpetChannelPtr_(std::make_shared<Channel>(acceptChannel_)),
    owner_(poller)
    { 
        acceptSockfd_.bind(listenAddr);
        acceptChannel_.setReadEventCallback(
            std::bind(handleRead,this)
        );
    }

    Acceptor::~Acceptor(){
        removeAcceptChannel();
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
        addAcceptChannel();
    }
    
    void Acceptor::addAcceptChannel(){
        owner_.updateChannel(accpetChannelPtr_);
    }

    void Acceptor::removeAcceptChannel(){
        owner_.removeChannel(accpetChannelPtr_);
    }
} // namespace chatRoom
