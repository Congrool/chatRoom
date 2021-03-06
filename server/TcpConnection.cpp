#include "server/TcpConnection.hpp"

#include <assert.h>
#include <algorithm>

namespace chatRoom
{
    TcpConnection::TcpConnection(int connfd,
                                NetAddress& local, 
                                NetAddress& peer)
    : connfd_(connfd),
    connChannelPtr_(std::make_shared<Channel>(connfd)),
    inputBuffer(),
    outputBuffer(),
    state_(connected_),
    localAddr_(local),
    peerAddr_(peer),
    id_(-1)
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
        assert(state_ == hasClosed_);
        assert(connChannelPtr_->isNonevent());
    }

    void TcpConnection::send(char* buff, size_t len){ 
        mutexGuard lock(mutex_);
        assert(state_ == connected_);
        outputBuffer.append(buff, len);     
    }

    void TcpConnection::send(std::string& msg){
        msg += '\0';
        send(msg.begin().base(), msg.length());
    }

    int TcpConnection::getMessage(std::string& msg){
        mutexGuard lock(mutex_);
        assert(connected_ != hasClosed_);
        auto start_ = inputBuffer.readStart();
        auto len = inputBuffer.readableSize();
        auto end_ = start_ + len;
        auto pos_ = std::search_n(start_, end_ ,1,'\0');
        if(pos_ >= end_) return -1;
        inputBuffer.retrieveLen(pos_-start_+1);
        std::string tmp(start_,pos_);
        msg = std::move(tmp);
        return 0;
    }

    // If peer client has shutdown on write, the channel
    // is always ready for read, and hasRead is 0.
    void TcpConnection::handleRead(){
        if(state_ == connected_){
            int hasRead = -1;
            {
                mutexGuard lock(mutex_);
                hasRead = inputBuffer.read(connfd_.fd());
            }
            if(hasRead > 0)
            {

                if(receiveCallback_)
                    receiveCallback_(shared_from_this());
            }
            else if(hasRead == 0)
            {
                state_ = closing_;
                connChannelPtr_->disableReading();
                tryCloseConn();
            }
            else // hasRead < 0, handle Error
            {
                // do nothing
            }
        }
        else
            tryCloseConn();
    }
    
    void TcpConnection::handleWrite(){
        if(state_ == connected_){
            writeDataAndCallback();         
        }
        else
            tryCloseConn();
    }

    void TcpConnection::forceToClose(){
        state_ = hasClosed_;
        connfd_.shutdownWrite();
        connChannelPtr_->disableAll();
        if(connClosedCallback_)
            connClosedCallback_(shared_from_this());
    }

    void TcpConnection::tryCloseConn()
    {
        assert(state_ == closing_);
        if(connChannelPtr_->isNonevent() && connClosedCallback_){
            state_ = hasClosed_;
            connClosedCallback_(shared_from_this());
            return;
        }
        if(inputBuffer.readableSize() && receiveCallback_)
            receiveCallback_(shared_from_this());

        if(outputBuffer.readableSize() == 0){
            connChannelPtr_->disableWriting();
            connfd_.shutdownWrite();
        }
        else{
            writeDataAndCallback();
        }
    }
    
    void TcpConnection::writeDataAndCallback(){
        if(outputBuffer.readableSize() == 0) return;
        size_t hasWrite = outputBuffer.write(connfd_.fd());
        if(sendCallback_){
            std::string str(outputBuffer.readStart(),hasWrite);
            sendCallback_(str);
        }
    }
} // namespace chatRoom
