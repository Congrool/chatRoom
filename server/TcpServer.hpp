#ifndef CHATROOM_SERVER_TCPSERVER_HPP
#define CHATROOM_SERVER_TCPSERVER_HPP 0

#include "base/ErrorLog.hpp"
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

    extern ErrorLog coutErrorLog;

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
            typedef std::function<void(int)>                connClosedCallbackFunc;
            typedef TcpConnection::receiveCallbackFunc      receiveCallbackFunc;
            typedef TcpConnection::sendCallbackFunc         sendCallbackFunc;
            typedef std::function<void()>                   connEstablishedFunc;
            typedef std::map<int,TcpConnectionPtr>          ConnContainer;

            TcpServer(uint16_t portNum, int numOfThreads);

            ~TcpServer();
            
            void start();

            void stop();

            void loop();

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
            
            /**
             * Tasks commpleted by TcpServer before doing 
             * onConnClosedCallback_ is removing channels from
             * poller and current connection from conns_.
             */
            connClosedCallbackFunc  onConnClosedCallback_;
            /**
             * Tasks commpleted before doing onReceivedCallback_
             * by TcpServer is telling the above
             * layer that messages has been received and stored
             * in the connection buffer but does nothing.
             */
            receiveCallbackFunc     onReceivedCallback_;

            /**
             * Tasks commpleted by TcpServer before doing
             * onSendCallback is putting the messages
             * to send in the connection's output buffer.
             */
            sendCallbackFunc        onSendCallback_;

            /**
             * Tasks commpleted by TcpServer before doing
             * onConnectionCallback_is inserting new channels
             * in poller and new connection in conns_.
             */
            connEstablishedFunc     onConnectionCallback_;

            void
            ConnectionEstablished(int connfd, NetAddress&);

            void
            MsgReceived(TcpConnectionPtr conn);

            void
            ConnectionClosed(TcpConnectionPtr conn);

            void
            MsgSent(std::string& msg);

            void 
            defaultPollThreadLoop();
    };


} // namespace chatRoom


#endif // CHATROOM_SERVER_TCPSERVER_HPP