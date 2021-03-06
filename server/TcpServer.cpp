#include "server/TcpServer.hpp"

namespace chatRoom
{
    TcpServer::TcpServer(uint16_t portNum,int numOfThreads)
    : localAddr_(portNum),
    localSock_(::socket(localAddr_.family(), SOCK_STREAM, 0)),
    threadPool_(numOfThreads),
    poller_(),
    acceptor_(localSock_.fd(),localAddr_),
    mutex_(),
    started_(false)
    { 
        acceptor_.setConnEstablishedCallBack(
            std::bind(&TcpServer::ConnectionEstablished,this, _1, _2)
        );
    }

    TcpServer::~TcpServer(){
        assert(started_ == false);
    }

    void TcpServer::
    start()
    {
        // FIXME:
        // Maybe should protect started_ with mutex_
        assert(started_ == false);
        started_ = true;
        acceptor_.listen();
        poller_.start();
        poller_.updateChannel(acceptor_.getChannelPtr());
        threadPool_.start();
    }

    void TcpServer::
    loop()
    {
        defaultPollThreadLoop();
    }
    

    void TcpServer::
    stop()
    {   
        {
            mutexGuard lockGuard(mutex_);
            assert(started_ == true);
            started_ = false;
        }
        poller_.stop();
        for(auto& it:conns_)
            it.second->forceToClose();
    }
    
    
    void TcpServer::
    ConnectionEstablished(int connfd, NetAddress& peerAddr)
    {
        auto connPtr = std::make_shared<TcpConnection>(
                connfd,this->localAddr_,peerAddr);
        connPtr->setConnClosedCallback(
            std::bind(&TcpServer::ConnectionClosed,this,_1)
        );
        connPtr->setReceiveCallback(
            std::bind(&TcpServer::MsgReceived,this,_1)
        );
        connPtr->setSendCallback(
            std::bind(&TcpServer::MsgSent,this,_1)
        );

        // If client has shutdowned on write, the connfd is always
        // ready for read. 
        poller_.updateChannel(connPtr->getChannelPtr());

        {
            mutexGuard lockGuard(mutex_);
            connPtr->setId(conns_.size());
            conns_[connPtr->getId()] = connPtr;
        }
        
        if(onConnectionCallback_)
            onConnectionCallback_();
    }

    void TcpServer::
    MsgReceived(TcpConnectionPtr conn)
    {
        
        if(onReceivedCallback_)
            onReceivedCallback_(conn); 
        else{
            coutErrorLog << "onReceivedCallback_ is NULL";
        }
    }

    void TcpServer::
    MsgSent(std::string& msg)
    {
        if(onSendCallback_)
            onSendCallback_(msg);
    }

    void TcpServer::
    defaultPollThreadLoop()
    {
        while(poller_.hasStarted()){
            ChannelList activeList = std::move(poller_.poll(500));
            {
                mutexGuard lock(mutex_);
                for(auto& it: activeList)
                {
                    if(it->isHandling() == false){
                        it->setHandling(true);
                        threadPool_.enqueue(&Channel::handleEvent,it.get());
                    }
                }
            }
        }
    }

    void TcpServer::
    ConnectionClosed(TcpConnectionPtr conn)
    {
        poller_.removeChannel(conn->getChannelPtr());
        {
            mutexGuard lockGuard(mutex_);
            conns_.erase(conn->getId());
        }
        if(onConnClosedCallback_)
            onConnClosedCallback_(conn->getId());
    }
} // namespace chatRoom
