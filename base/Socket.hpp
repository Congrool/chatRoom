#ifndef CHATROOM_BASE_SOCKET
#define CHATROOM_BASE_SOCKET 0

#include <netinet/in.h>
#include <string>

namespace chatRoom
{
	// Managing struct sockaddr_in type in network order.
	class NetAddress{
		public:
			// wrapper for sockaddr_in
			// The first parameter is port number.
			// The second parameter is IP address in the
			// form of dotted decimal notation.
			// Both of them are in host byte order.
			explicit 
			NetAddress(uint16_t, std::string);

			// sin_addr is default INADDR_ANY
			// The first parameter is port number.
			// The second parameter is IP address in the
			// form of byte.
			// Both of them are in host byte order.
			explicit
			NetAddress(uint16_t, uint32_t = INADDR_ANY);

			// Using sockaddr_in as parameter
			explicit
			NetAddress(sockaddr_in* addr);


			~NetAddress() = default;

			sa_family_t family() const { return addr_.sin_family; }
			
			const struct sockaddr_in* getAddr() const { return &addr_; } 

		private:
			struct sockaddr_in addr_;
	};

	class Socket{
		public:
			explicit 
			Socket(int sockfd) : sockfd_(sockfd) { }
			
			~Socket() = default;
			
			int fd() { return sockfd_; }

			void bind(NetAddress&);

			void listen();

			int accept(sockaddr_in*);

			void connect(NetAddress);

			void shutdownWrite();

			// Enable/disable SO_KEEPALIVE
			void setKeepAlive(bool on);
			
		
		private:
			const int sockfd_;

	};
    
} // namespace chatRoom

#endif // CHATROOM_BASE_SOCKET

