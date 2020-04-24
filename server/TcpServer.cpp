#include "server/TcpServer.hpp"

namespace chatRoom
{
    TcpServer::TcpServer(uint16_t portNum,int numOfThreads)
    : localAddr_(portNum),
    localSock_(::socket(localAddr_.family, SOCK_STREAM, 0)),
    threadPool_(numOfThreads),
    poller_(),
    acceptor_(localSock_.fd(),localAddr_,poller_),
    mutex_(),
    started_(false)
    { }

    void TcpServer::
    start()
    {
        // FIXME:
        // Maybe should protect started_ with mutex_
        assert(started_ == false);
        started_ = true;
        poller_.updateChannel(acceptor_.getChannelPtr());
        // acceptor_::handleRead will call ::accept()
        acceptor_.listen();     
        threadPool_.start();
        // set one Poller thread
        threadPool_.enqueue(
            std::bind(&TcpServer::defaultPollThreadLoop,this)
        );

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
    }
    
    
    void TcpServer::
    ConnectionEstablished(int connfd, NetAddress& peerAddr)
    {
        TcpConnection conn(connfd,
                    this->localAddr_,
                    peerAddr);
        conn.setConnClosedCallback(
            std::bind(&TcpServer::ConnectionClosed,this,_1)
        );
        conn.setReceiveCallback(
            std::bind(&TcpServer::MsgReceived,this,_1,_2)
        );
        // conn.setSendCallback(
        //     std::bind(&TcpServer::)
        // );
        poller_.updateChannel(conn.getChannelPtr());

        {
            mutexGuard lockGuard(mutex_);
            conns_.insert(conn.getSelfPtr());
        }
        
        if(onConnectionCallback_)
            threadPool_.enqueue(onConnectionCallback_);
    }

    void TcpServer::
    MsgReceived(const char* first, size_t len)
    {
        if(onReceivedCallback_)
            onReceivedCallback_(first, len); 
    }

    void TcpServer::
    defaultPollThreadLoop()
    {
        while(poller_.hasStarted()){
            // FIXME:
            // It's uneffecitive.
            // Because it's only one poll thread active 
            // at the same time.
            mutexGuard locGurad(mutex_);
            poller_.poll(0,activeChannelList);
            for(auto& it:activeChannelList)
            {
                threadPool_.enqueue(it->handleEvent);
            }
            
            activeChannelList.clear();
        }
    }

    void TcpServer::
    ConnectionClosed(TcpConnectionPtr& conn)
    {
        poller_.removeChannel(conn->getChannelPtr());
        {
            mutexGuard lockGuard(mutex_);
            conns_.erase(conn->getSelfPtr());
        }
        if(onClosedCallback_)
            onClosedCallback_(conn);
    }
} // namespace chatRoom
