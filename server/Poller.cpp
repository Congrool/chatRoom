#include "base/ErrorLog.hpp"
#include "server/Poller.hpp"

#include <assert.h>

namespace chatRoom
{
    Poller::Poller()
    : started_(false)
    { }

    Poller::~Poller()
    {
        mutexGuard lockGuard(mutex_);
        channelMaps_.clear();
    }

    void Poller::updateChannel(ChannelPtr channel){
        // add a new channel
        if(channel->index == -1){
            struct pollfd pfd;
            pfd.fd = channel->fd();
            pfd.events = channel->events();
            pfd.revents = 0;
            {
                channel->set_index(static_cast<int>(pollfds_.size())-1);
                mutexGuard lockGuard(mutex_);
                assert(channelMaps_.count(channel->fd()) == 0);
                pollfds_.emplace_back(std::move(pfd));
                channelMaps_[channel->index()] = channel;
            }
        }
        // update an exsiting channel
        else{
            mutexGuard lockGuard(mutex_);
            assert((channelMaps_.count(channel->fd()) == 1)
                && channelMaps_[channel->fd()] == channel);
            assert(0 <= channel->index()
                && channel->index() < static_cast<int>(pollfds_.size()));
            assert((pollfds_[channel->index()].fd == channel->fd()
                    || pollfds_[channel->index()].fd == -channel->fd()-1));
            pollfd& pfd = pollfds_[channel->index()];
            pfd.events = channel->events();
            pfd.revents = 0;
            // If new channel is nonevent, set the pfd ignored.
            if(channel->isNonevent()){
                pfd.fd = -channel->fd()-1;
            }
        }
    }

    void Poller::removeChannel(ChannelPtr channel){
        mutexGuard lockGuard(mutex_);
        if(channelMaps_.count(channel->fd()) == 0)
            return;
        assert(channelMaps_[channel->fd()] == channel);
        assert(0 <= channel->index() 
            && channel->index() < static_cast<int>(pollfds_.size())-1);
        assert(pollfds_[channel->index()].fd == channel->fd()
            || pollfds_[channel->index()].fd == -channel->fd()-1);
        // When should the erase fail?
        int ret = channelMaps_.erase(channel->fd());
        assert(ret == 1);
        if(channel->index() == pollfds_.size()-1)
            pollfds_.pop_back();
        else{
            int idx = channel->index();
            pollfd& pfd = pollfds_[idx];
            pfd.fd = pollfds_.back().fd;
            pfd.events = pollfds_.back().events;
            pfd.revents = pollfds_.back().revents;
            
            int backfd = pollfds_.back().fd < 0 ? 
                        -pollfds_.back().fd - 1 : pollfds_.back().fd; 
            channelMaps_[backfd]->set_index(idx);
            pollfds_.pop_back();
        }
    }

    void Poller::poll(int timeout, ChannelList& list){
        int ret,savedError;
        {
            mutexGuard lockGuard(mutex_);
            assert(started_ == true);
            ret = ::poll(pollfds_.begin().base(),pollfds_.size(),timeout);
            // why should we save errno?
            savedError = errno;
            // revents of pollfd has been updated
            if(ret > 0){ 
                for(auto& it:pollfds_){
                    // revents of pollfd with negative fd has been set as 0;
                    // So it will be skipped automatically.
                    if(it.revents > 0){
                        std::map<int,ChannelPtr>::const_iterator chp = 
                                        channelMaps_.find(it.fd);
                        assert(chp != channelMaps_.end());
                        assert(chp->second->fd == it.fd);
                        chp->second->set_revents(it.revents);
                        list.push_back(chp->second);
                    }
                }
            }
        }
        if(ret == 0){
            // do nothing
        }
        else{
            // Handle error
            // I don't know how to handle error. :-(
            // So just print "error". :-)
            if(savedError == EINVAL)
                coutErrorLog << "poll error: EINVAL";
            else if(savedError == ENOMEM)
                //In fact, I don't know what ENOMEM means.
                coutErrorLog << "poll error: ENOMEM";
            // Other errors are impossible, I think.
        }
    }

    void Poller::start() 
    { 
        mutexGuard lockGuard(mutex_); 
        assert(started_ == false);
        started_ = true; 
    }

    void Poller::stop()
    {
        mutexGuard lockGuard(mutex_);
        assert(started_ == true);
        started_ = false;
    }
} // namespace chatRoom
