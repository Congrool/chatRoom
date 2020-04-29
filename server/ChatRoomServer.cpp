#include "server/ChatRoomServer.hpp"

namespace chatRoom
{
    ChatRoomServer::ChatRoomServer()
    :server_(ServerPort,6),
    conns_(server_.getConnList())
    { 
        server_.setOnConnClosedCallback(
            std::bind(&ChatRoomServer::onConnClosed,this,_1)
        );
        server_.setOnReceiveCallback(
            std::bind(&ChatRoomServer::onReceive,this,_1)
        );
    }

    ChatRoomServer::~ChatRoomServer(){
        server_.stop();
    }

    void ChatRoomServer::start(){
        server_.start();
    }

    void ChatRoomServer::loop(){
        server_.loop();
    }

    void ChatRoomServer::onConnClosed(int closedConnId){
        char buf[64];
        snprintf(buf,64,"Msg: %d has left.",closedConnId);
        auto len = strlen(buf);
        for(auto& it : conns_){
            it.second->send(buf,len);
        }
    }

    void ChatRoomServer::onReceive(TcpConnectionPtr conn){
        std::string msg;
        if(conn->getMessage(msg) == 0){
            for(auto& it : conns_)
                if(it.second->getId() != conn->getId() &&
                    it.second->isOnline())
                    it.second->send(msg);
        }
    }
} // namespace chatRoom
