#ifndef CHATROOM_SERVER_TCPCONNECTION_HPP
#define CHATROOM_SERVER_TCPCONNECTION_HPP 0

#include "base/Buffer.hpp"
#include "base/noncopyable.hpp"
#include "base/Socket.hpp"

#include "server/Channel.hpp"
#include "server/Poller.hpp"

#include <vector>

namespace chatRoom
{   
    /*
     * Handle duplex operations on established connection, 
     * such as sending and receiving bytes.
     * Both sides can suppose that the message it sends will
     * be received by another side, and never be dropped 
     * because of the TcpConnection class.
     */
    class TcpConnection : noncopyable{
        public:
            typedef std::function<void(std::string&)> sendCallbackFunc;
            typedef std::function<void()> receiveCallbackFunc;
            typedef std::function<void()> connClosedCallbackFunc;
            typedef Buffer::size_t size_t;
            // using fd returned by accept() as the argument
            explicit
            TcpConnection(int connfd,Poller& ,
                        NetAddress& local, NetAddress& peer);

            ~TcpConnection();

            void setSendCallback(sendCallbackFunc& func)
            { sendCallback_ = func; }

            void setReceiveCallback(receiveCallbackFunc& func)
            { receiveCallback_ = func; }

            void setConnClosedCallback(connClosedCallbackFunc& func)
            { connClosedCallback_ = func; }

            void handleRead();

            void handleWrite();

            void send(char* buff, size_t len);
            void send(std::string& );

            void addConnChannel();
            void removeConnChannel();

            bool hasBytesToRead() const 
            { return outputBuffer.readableSize(); }

            bool hasBytesToWrite() const 
            { return inputBuffer.readableSize(); }
            
            void close() { closed_ = true; }

        private:
            Socket connfd_;
            Channel connChannel_;
            ChannelPtr connChannelPtr_;

            Poller& owner_;
            
            Buffer inputBuffer;
            Buffer outputBuffer;

            bool closed_;

            const NetAddress& localAddr_;
            const NetAddress& peerAddr_; 

            sendCallbackFunc sendCallback_;
            receiveCallbackFunc receiveCallback_;
            connClosedCallbackFunc connClosedCallback_;
    };
} // namespace chatRoom

#endif // CHATROOM_SERVER_TCPCONNECTION_HPP