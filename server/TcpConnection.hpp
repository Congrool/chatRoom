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
     * 
     * Using '\0' as the separation character of TcpConnection
     * layer.
     * 
     * TcpConnection is almost visited by a signal thread
     * at a time, except inputBuffer and outputBuffer.
     */

    // Take cause that it's public inheritance.
    // Otherwise, it will throw bad_weak_ptr error.
    class TcpConnection : public std::enable_shared_from_this<TcpConnection>{
        public:
            typedef std::shared_ptr<TcpConnection>          pointer;
            typedef std::function<void(std::string&)>       sendCallbackFunc;
            typedef std::function<void(pointer)>            receiveCallbackFunc;
            typedef std::function<void(pointer)>            connClosedCallbackFunc;
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

            bool isOnline() const
            { return state_ == connected_; }

            // If there's not a whole message in the buffer
            // return -1;
            // else
            // return 0;
            int getMessage(std::string&);

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

            // The following three callbacks is multithread.
            sendCallbackFunc sendCallback_;
            receiveCallbackFunc receiveCallback_;
            connClosedCallbackFunc connClosedCallback_;

            Mutex   mutex_;

            void tryCloseConn();

            void writeDataAndCallback();

            int id_;
    };

    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
} // namespace chatRoom

#endif // CHATROOM_SERVER_TCPCONNECTION_HPP