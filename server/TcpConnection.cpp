#include "server/TcpConnection.hpp"

#include <assert.h>

namespace chatRoom
{
    TcpConnection::TcpConnection(int connfd,
                                NetAddress& local, 
                                NetAddress& peer)
    : connfd_(connfd),
    connChannelPtr_(std::make_shared<Channel>(connfd)),
    inputBuffer(),
    outputBuffer(),
    closed_(false),
    localAddr_(local),
    peerAddr_(peer),
    selfPtr(std::move(std::make_shared<TcpConnection>(*this)))
    {
        connChannelPtr_->setWriteEventCallback(
            std::bind(&TcpConnection::handleWrite,this)
        );
        connChannelPtr_->setReadEventCallback(
            std::bind(&TcpConnection::handleRead,this)
        );
        connChannelPtr_->enableReading();
        connChannelPtr_->enbaleWriting();
    }

    TcpConnection::~TcpConnection(){
        assert(closed_);
        assert(connChannelPtr_->isNonevent());
    }

    void TcpConnection::send(char* buff, size_t len){
        outputBuffer.append(buff, len);
    }

    void TcpConnection::send(std::string& msg){
        send(msg.begin().base(), msg.length());
    }

    // If peer client has shutdown on write, the channel
    // is always ready for read, and hasRead is 0.
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
            // do nothing
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
                        inputBuffer.readStart(),
                        inputBuffer.readableSize()
                    );
            } 
         }
    }
    

    void TcpConnection::handleWrite(){
        if(closed_ && outputBuffer.readableSize() == 0){         
            connChannelPtr_->disableWriting();
            connfd_.shutdownWrite();
            if(connClosedCallback_)
                connClosedCallback_(selfPtr);
        }
        else{
            size_t hasWrite = outputBuffer.write(connfd_.fd());
            if(sendCallback_){
                std::string str(outputBuffer.readStart(),hasWrite);
                sendCallback_(str);
            }
        }
    }

    void TcpConnection::forceToClose(){
        closed_ = true;
        connfd_.shutdownWrite();
        connChannelPtr_->disableAll();
        if(connClosedCallback_)
            connClosedCallback_(selfPtr);
    }
} // namespace chatRoom
