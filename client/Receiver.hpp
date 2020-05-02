#ifndef CHATROOM_CLIENT_RECEIVER_HPP
#define CHATROOM_CLIENT_RECEIVER_HPP 0

#include <functional>
#include <deque>
#include <vector>

namespace chatRoom
{

    /**
     * The basic function of Receiver object is receiving data
     * from file discriptor, typically sock,
     * and save them in the buffer for further using.
     */ 
    class Receiver
    {
        public:
            typedef 
            std::function<void(std::vector<std::string>&)>    
                receiveFunc;

            typedef 
            std::function<void(const char* first, const size_t len)> 
                closeFunc;
            /**
             * @param fd    An opened fd to receive data from
             */
            Receiver(int fd);

            /**
             * All bytes left in the buffer are handled
             * by someone who calls the destructor through
             * closeCallback_.
             */
            ~Receiver();

            /**
             * Receive data from fd.
             * @return      Num of bytes having been received.
             */
            int receive();

            /**
             * Default split charactor is '\0'.
             * This function get one Message from the buffer.
             * @return      A received message.
             */

            void waitForMsg();

            void setReceiveCallback(receiveFunc fun)
            { receiveCallback_ = fun; }

            void setCloseCallback(closeFunc fun)
            { closeCallback_ = fun; } 

        private:
            const int           fd_;
            std::deque<char>    buffer_;
            receiveFunc         receiveCallback_;
            closeFunc           closeCallback_;

            void readDataFromFd();

            std::vector<std::string> getMsgs();

    };
} // namespace chatRoom


#endif