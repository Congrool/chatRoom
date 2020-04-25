#ifndef CHATROOM_SERVER_TCPSERVER_HPP
#define CHATROOM_SERVER_TCPSERVER_HPP 0

#include "server/Acceptor.hpp"
#include "server/Poller.hpp"
#include "server/TcpConnection.hpp"
#include "server/ThreadPool.hpp"

#include <set>

namespace chatRoom
{   
    #define _1 std::placeholders::_1
    #define _2 std::placeholders::_2
    #define _3 std::placeholders::_3

    // what TcpServer should manage are as follows:
    // 1. onConnection: what to do when a new 
    //    connection has been established
    // 2. onReceive: what to do when receiving messages 
    //    from one of the established connections
    // 3. onClosed: what to do when a exsiting 
    //    connection has been closed
    class TcpServer : noncopyable
    {
        public:
            typedef Poller::ChannelList ChannelList;
            typedef TcpConnection::connClosedCallbackFunc   connClosedCallbackFunc;
            typedef TcpConnection::receiveCallbackFunc      receiveCallbackFunc;
            typedef TcpConnection::sendCallbackFunc         sendCallbackFunc;
            typedef std::function<void()>                   connEstablishedFunc;
            
            typedef std::set<TcpConnectionPtr>              ConnContainer;

            TcpServer(uint16_t portNum, int numOfThreads);
            ~TcpServer();
            void start();

            void stop();

            ConnContainer& getConnList()
            { return conns_;}

            void setOnConnClosedCallback(connClosedCallbackFunc func)
            { onConnClosedCallback_ = func;}

            void setOnReceiveCallback(receiveCallbackFunc func)
            { onReceivedCallback_ = func; }

            void setOnSendCallback(sendCallbackFunc func)
            { onSendCallback_ = func; }

            void setOnConnectionCallback(connEstablishedFunc func)
            { onConnectionCallback_ = func; }

        private:
            NetAddress localAddr_;
            Socket localSock_;
            ThreadPool threadPool_;
            Acceptor acceptor_;
            Poller poller_;        
            ConnContainer conns_;
            Mutex mutex_;

            bool started_;

            ChannelList activeChannelList;

            connClosedCallbackFunc  onConnClosedCallback_;
            receiveCallbackFunc     onReceivedCallback_;
            sendCallbackFunc        onSendCallback_;
            connEstablishedFunc     onConnectionCallback_;

            void
            ConnectionEstablished(int connfd, NetAddress&);

            void
            MsgReceived(const char* first, size_t len);

            void
            ConnectionClosed(TcpConnectionPtr& conn);

            // void
            // MsgSent();

            void 
            defaultPollThreadLoop();
    };


} // namespace chatRoom


#endif // CHATROOM_SERVER_TCPSERVER_HPP