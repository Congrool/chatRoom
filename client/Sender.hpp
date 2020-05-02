#ifndef CHATROOM_CLIENT_SENDER_HPP
#define CHATROOM_CLIENT_SENDER_HPP 0

#include <string>
#include <functional>
#include <vector>

namespace chatRoom
{
    class Sender
    {
        public:
            typedef std::function<void(std::vector<std::string>&)> sendFunc;
            
            Sender(int fd);
            
            ~Sender() = default;
            
            void send(std::string& msg);
            
            void send(const char* start, size_t len);

            void setSendCallback_(sendFunc fun)
            { sendCallback_ = fun; }

        private:
            int fd_;
            sendFunc sendCallback_;
            std::vector<std::string> getMsgs(const char* start,const char* end);
    
    };
} // namespace chatRoom


#endif