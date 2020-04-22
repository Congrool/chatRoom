#ifndef CHATROOM_BASE_POLLER_HPP
#define CHATROOM_BASE_POLLER_HPP 0

#include "base/noncopyable.hpp"
#include "server/Channel.hpp"

#include <vector>
#include <map>
#include <poll.h>

namespace chatRoom
{
	// Using ::poll as default.
	// Maybe ::epoll should be suported later as an option.
 	class Poller : noncopyable
	{
		public:
			typedef std::vector<ChannelPtr> ChannelList;
			Poller();
			~Poller() = default;
			void updateChannel(ChannelPtr channel);
			void removeChannel(ChannelPtr channel);
			
			// timeout indicating the maximum time poll() can block.
			// ChannelList used to return ready Channels.
			void poll(int timeout, ChannelList&);

			// FIXME: started_ may be in the critical region
			// and should be protected by a lockguard when modified.
			bool hasStarted() { return started_; }
			void start() { started_ = true; }
			void stop() { started_ = false; }

		private:
			// It's the key to jump out of the endless poll loop.
			bool started_;

			std::map<int,ChannelPtr> channelMaps_;
			std::vector<pollfd> pollfds_;
	};
} // namespace chatRoom


#endif // CHATROOM_BASE_POLLER_HPP