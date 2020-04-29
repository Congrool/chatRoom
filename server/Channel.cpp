#include "server/Channel.hpp"

#include <poll.h>
#include <assert.h>

namespace chatRoom
{
    const int Channel::noneEvent = 0;
    const int Channel::readEvent = POLLIN;
    const int Channel::writeEvent = POLLOUT;

    /**
     * The member of the Channel class is visited
     * by a signal thread at a time,through 
     * handleEvent function may be multithread.
     */
    Channel::Channel(const int fd)
    : sockfd_(fd),
    events_(0),
    revents_(0),
    isHandling_(false),
    index_(-1),
    readEventCallback(nullptr),
    writeEventCallback(nullptr),
    errorEventCallback(nullptr)
    { }
    
    void Channel::enableReading() { events_ |= readEvent; }
    void Channel::enbaleWriting() { events_ |= writeEvent; }
    void Channel::disableReading() { events_ &= ~readEvent; }
    void Channel::disableWriting() { events_ &= ~writeEvent; }
    void Channel::disableAll() { events_ = noneEvent; }

    void Channel::handleEvent(){
        assert(isHandling_ == true);
        if((revents_ & (POLLERR | POLLNVAL)) && errorEventCallback)
            errorEventCallback();
        if((revents_ & (POLLIN | POLLRDNORM)) && readEventCallback)
            readEventCallback();
        if((revents_ & (POLLOUT)) && writeEventCallback)
            writeEventCallback();
        isHandling_ = false;
    }

    
} // namespace chatRoom
