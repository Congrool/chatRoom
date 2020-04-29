#ifndef CHATROOM_SERVER_CHANNEL_HPP
#define CHATROOM_SERVER_CHANNEL_HPP 0

#include <functional>
#include <memory>

namespace chatRoom
{

	// A class manages events the file discriptor interested in.
	// And set the callback.
	class Channel{
		public:
			typedef std::function<void()> EventCallback;

			explicit Channel(const int);
			~Channel() = default;

			void setReadEventCallback(EventCallback cb)
			{ readEventCallback = std::move(cb); }
			
			void setWriteEventCallback(EventCallback cb)
			{ writeEventCallback = std::move(cb); }
			
			void setErrorEventCallback(EventCallback cb)
			{ errorEventCallback = std::move(cb); }

			void enableReading();
			void disableReading();
			void enbaleWriting();
			void disableWriting();
			void disableAll();

			int index() { return index_; }
			int fd() { return sockfd_; }
			int events() { return events_; }
			int revents() { return revents_; }
			void set_index(int idx) { index_ = idx; }

			// handle read or write event according to revents_
			void handleEvent();

			// used by poller to set revents_ of channel 
			// accroding to the revents of file discriptor 
			// when the poll() return
			void set_revents(int revent)	{ revents_ = revent; }

			bool isNonevent() { return events_ == noneEvent; }

			bool isHandling() { return isHandling_; }

			bool setHandling(bool on) { isHandling_ = on; }

		private:
			const int sockfd_;
			int events_;
			int revents_;

			bool isHandling_;
			
			// It's used in poll class,
			// indicating the index of concerning pollfd
			// in pollfds_ array. It's initialized as -1 before
			// the pollfd having been created and put in pollfds_ array.
			int index_;

			const static int noneEvent;
			const static int readEvent;
			const static int writeEvent;

			EventCallback readEventCallback;
			EventCallback writeEventCallback;
			EventCallback errorEventCallback;
	};

	typedef std::shared_ptr<Channel> ChannelPtr;
	
} // namespace chatRoom

#endif // CHATROOM_SERVER_CHANNEL_HPP
