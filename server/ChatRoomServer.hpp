#ifndef CHATROOM_SERVER_CHATROOMSERVER_HPP
#define CHATROOM_SERVER_CHATROOMSERVER_HPP 0

#include "server/TcpServer.hpp"


// Now, the order of messages received may be not
// as same as the order they sent because of mutithreads.
// Maybe we can use sliding window on connections to
// solve the problem.
namespace chatRoom
{
    const uint16_t ServerPort = 8848;
    class ChatRoomServer : noncopyable
    {
        public:
            typedef TcpServer::ConnContainer ConnContainer;
            ChatRoomServer();

            ~ChatRoomServer();

            void start();

            void loop();
        private:
            TcpServer server_;
            ConnContainer& conns_; 

            void onConnClosed(int closedConnId);

            void onReceive(TcpConnectionPtr conn);


    };
} // namespace chatRoom



#endif