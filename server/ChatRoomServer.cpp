#include "server/ChatRoomServer.hpp"

namespace chatRoom
{
    ChatRoomServer::ChatRoomServer()
    :server_(ServerPort,0),
    conns_(server_.getConnList())
    { 
        server_.setOnConnClosedCallback(
            std::bind(&ChatRoomServer::onConnClosed,this,_1)
        );
        server_.setOnReceiveCallback(
            std::bind(&ChatRoomServer::onReceive,this,_1)
        );
    }

    void ChatRoomServer::start(){
        server_.start();
    }

    void ChatRoomServer::loop(){
        server_.loop();
    }

    void ChatRoomServer::onConnClosed(int closedConnId){
        std::string msg;
        msg += "Msg: "; msg += closedConnId; msg += " has left.";
        for(auto& it : conns_){
            it.second->send(msg);
        }
    }

    void ChatRoomServer::onReceive(TcpConnection& conn){
        std::string msg;
        if(conn.getMessage(msg) == 0){
            for(auto& it : conns_)
                if(it.second->getId() != conn.getId())
                    it.second->send(msg);
        }
    }
} // namespace chatRoom
