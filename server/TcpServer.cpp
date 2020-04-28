#include "server/TcpServer.hpp"

namespace chatRoom
{
    TcpServer::TcpServer(uint16_t portNum,int numOfThreads)
    : localAddr_(portNum),
    localSock_(::socket(localAddr_.family(), SOCK_STREAM, 0)),
    threadPool_(1),
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
        // acceptor_::handleRead will call ::accept()

        threadPool_.start();
        // set one Poller thread
        // threadPool_.enqueue(
            // std::bind(&TcpServer::defaultPollThreadLoop,this)
        // );

    }

    void TcpServer::
    loop()
    {
        defaultPollThreadLoop();
    }
    
    // FIXME:
    // Before stop, the server should send all bytes
    // in the output buffer of each connection. 
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
            threadPool_.enqueue(onConnectionCallback_);
    }

    void TcpServer::
    MsgReceived(TcpConnectionPtr conn)
    {
        
        if(onReceivedCallback_)
            threadPool_.enqueue(onReceivedCallback_,conn); 
        else{
            coutErrorLog << "onReceivedCallback_ is NULL";
        }
    }

    void TcpServer::
    MsgSent(std::string& msg)
    {
        if(onSendCallback_)
            threadPool_.enqueue(
                std::bind(onSendCallback_,msg)
            );
    }

    void TcpServer::
    defaultPollThreadLoop()
    {
        while(poller_.hasStarted()){
            // FIXME:
            // Race condition!
            // When a channel is handling its event, the event is
            // still in the poller list, which means that the channel
            // will appear in the activeList again with the same 
            // event to handle and ask another thread to do it.
            // Then, race condition occurs.  
            ChannelList activeList = poller_.poll(500);
            {
                mutexGuard lock(mutex_);
                for(auto& it: activeList)
                {
                    threadPool_.enqueue(&Channel::handleEvent,it.get());
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
            threadPool_.enqueue(
                onConnClosedCallback_,conn->getId()
            );
    }
} // namespace chatRoom
