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
    class TcpConnection{
        public:
            typedef TcpConnection*                          pointer;
            typedef std::function<void(std::string&)>       sendCallbackFunc;
            typedef std::function<
                    void(const char* first,size_t len)>     receiveCallbackFunc;
            typedef std::function<void(TcpConnection&)>     connClosedCallbackFunc;
            typedef Buffer::size_t                          size_t;
            // using fd returned by accept() as the argument
            explicit
            TcpConnection(int connfd,
                        NetAddress& local,
                        NetAddress& peer);

            ~TcpConnection();

            void setSendCallback(sendCallbackFunc func)
            { sendCallback_ = func; }

            void setReceiveCallback(receiveCallbackFunc func)
            { receiveCallback_ = func; }

            void setConnClosedCallback(connClosedCallbackFunc func)
            { connClosedCallback_ = func; }

            void handleRead();

            void handleWrite();

            void send(char* buff, size_t len);
            void send(std::string& );

            // Force the connection to close.
            // Someone who call the function should 
            // handle bytes stored in the buffer.
            void forceToClose();

            bool hasBytesToRead() const 
            { return outputBuffer.readableSize(); }

            bool hasBytesToWrite() const 
            { return inputBuffer.readableSize(); }
            
            const ChannelPtr& getChannelPtr()
            { return connChannelPtr_; }

            void setId(int id)
            { assert(id_ == -1); id_ = id; }

            const int getId() const
            { return id_; }

        private:
            Socket connfd_;
            ChannelPtr connChannelPtr_;
            
            Buffer inputBuffer;
            Buffer outputBuffer;

            enum connState{
                connected_ = 0,
                closing_ = 1,
                hasClosed_ = 2
            };

            connState state_;
            
            const NetAddress& localAddr_;
            const NetAddress& peerAddr_; 

            sendCallbackFunc sendCallback_;
            receiveCallbackFunc receiveCallback_;
            connClosedCallbackFunc connClosedCallback_;

            void tryCloseConn();

            void writeDataAndCallback();

            int id_;
    };

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
} // namespace chatRoom

#endif // CHATROOM_SERVER_TCPCONNECTION_HPP