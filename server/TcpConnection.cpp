#include "server/TcpConnection.hpp"

#include <assert.h>

namespace chatRoom
{
    TcpConnection::TcpConnection(int connfd,
                                NetAddress& local, 
                                NetAddress& peer)
    : connfd_(connfd),
    connChannel_(connfd),
    connChannelPtr_(std::make_shared<Channel>(connChannel_)),
    inputBuffer(),
    outputBuffer(),
    closed_(false),
    localAddr_(local),
    peerAddr_(peer),
    selfPtr(std::move(std::make_shared<TcpConnection>(*this)))
    {
        connChannel_.setWriteEventCallback(
            std::bind(&TcpConnection::handleWrite,this)
        );
        connChannel_.setReadEventCallback(
            std::bind(&TcpConnection::handleRead,this)
        );
        connChannel_.enableReading();
        connChannel_.enbaleWriting();
    }

    TcpConnection::~TcpConnection(){
        assert(closed_);
        assert(connChannelPtr_->isNonevent());
        if(connClosedCallback_)
            connClosedCallback_(selfPtr);
    }

    void TcpConnection::send(char* buff, size_t len){
        outputBuffer.append(buff, len);
    }

    void TcpConnection::send(std::string& msg){
        send(msg.begin().base(), msg.length());
    }


    void TcpConnection::handleRead(){
        int hasRead = inputBuffer.read(connfd_.fd());
        if(hasRead > 0)
        {

            if(receiveCallback_)
                receiveCallback_(
                    inputBuffer.readStart(),
                    inputBuffer.readableSize()
                );
        }
        else if(hasRead == 0)
        {
            closed_ = true;
            closeConn();
        }
        else // hasRead < 0, handle Error
        {
            // Temporarily, do nothing
        }
    }

    void TcpConnection::closeConn()
    {
        if(closed_){
            if(inputBuffer.readableSize() == 0)
                connChannelPtr_->disableReading();         
            else{
                if(receiveCallback_)
                    receiveCallback_(
                        inputBuffer.begin(),
                        inputBuffer.readableSize()
                        );
            } 
         }
    }
    

    void TcpConnection::handleWrite(){
        if(closed_ && outputBuffer.readableSize() == 0){         
            connChannelPtr_->disableWriting();
            connfd_.shutdownWrite();
        }
        else{
            size_t hasWrite = outputBuffer.write(connfd_.fd());
            if(sendCallback_){
                std::string str(outputBuffer.readStart(),hasWrite);
                sendCallback_(str);
            }
        }
    }

} // namespace chatRoom
