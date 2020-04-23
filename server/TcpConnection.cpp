#include "server/TcpConnection.hpp"

#include <assert.h>

namespace chatRoom
{
    TcpConnection::TcpConnection(int connfd, Poller& poller, 
                       NetAddress& local, NetAddress& peer)
    : connfd_(connfd),
    connChannel_(connfd),
    connChannelPtr_(std::make_shared<Channel>(connChannel_)),
    owner_(poller),
    inputBuffer(),
    outputBuffer(),
    closed_(false),
    localAddr_(local),
    peerAddr_(peer)
    {
        connChannel_.setWriteEventCallback(
            std::bind(handleWrite,this)
        );
        connChannel_.setReadEventCallback(
            std::bind(handleRead,this)
        );
        connChannel_.enableReading();
        connChannel_.enbaleWriting();
    }

    TcpConnection::~TcpConnection(){
        assert(closed_);
        assert(connChannelPtr_->isNonevent());
        removeConnChannel();
        if(connClosedCallback_)
            connClosedCallback_();
    }

    void TcpConnection::addConnChannel(){
        owner_.updateChannel(this->connChannelPtr_);
    }

    void TcpConnection::removeConnChannel(){
        owner_.removeChannel(this->connChannelPtr_);
    }

    void TcpConnection::send(char* buff, size_t len){
        outputBuffer.append(buff, len);
    }

    void TcpConnection::send(std::string& msg){
        send(msg.begin().base(), msg.length());
    }


    void TcpConnection::handleRead(){
         if(closed_){
            if(inputBuffer.readableSize() == 0)
                connChannelPtr_->disableReading();         
            else{
                if(receiveCallback_)
                    receiveCallback_();
            } 
         }
         else{
             inputBuffer.read(connfd_.fd());
             if(receiveCallback_)
                    receiveCallback_();
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
