#ifndef CHATROOM_SERVER_ACCEPTOR_HPP
#define CHATROOM_SERVER_ACCEPTRO_HPP 0

#include "base/Socket.hpp"
#include "server/Channel.hpp"
#include "server/Poller.hpp"

#include <functional>

namespace chatRoom
{  
    class Acceptor{
        public:
            typedef std::function<void(int,NetAddress)> NewConnFunc;
            Acceptor(int fd,NetAddress&,Poller&);
            ~Acceptor() = default;

            void handleRead();

            void setConnEstablishedCallBack(NewConnFunc& func)
            { newConnCallback_ = func; }

            void listen();

            const ChannelPtr& getChannelPtr() const
            { return accpetChannelPtr_; }

        private:
            Socket acceptSockfd_;
            Channel acceptChannel_;
            ChannelPtr accpetChannelPtr_;
            
            // Provided by Users.
            // Called when acceptSockfd_ ready.
            NewConnFunc newConnCallback_;
    };
} // namespace chatRoom

#endif // CHATROOM_SERVER_ACCEPTOR_HPP