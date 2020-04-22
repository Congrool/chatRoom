#include <arpa/inet.h>
#include <memory.h>

#include "base/Socket.hpp"
#include "base/ErrorLog.hpp"

namespace chatRoom
{   
    inline uint16_t toNet16(uint16_t data) { return htobe16(data); }
    inline uint32_t toNet32(uint32_t data) { return htobe32(data); }
    inline uint64_t toNet64(uint64_t data) { return htobe64(data); }

    inline void ipToNet(std::string ip, sockaddr_in& addr){
        if(inet_aton(ip.c_str(),&addr.sin_addr) <= 0){
            coutErrorLog << "inet_aton error" << std::endl;
            exit(-1);
        }
    }

    inline void memZero(void* arr, size_t len) {
        memset(arr, 0, len);
    }

    NetAddress::NetAddress(uint16_t port, std::string ipstr){
        memZero(static_cast<void*>(&(this->addr_)),sizeof(this->addr_));
        this->addr_.sin_family = AF_INET;
        this->addr_.sin_port = toNet16(port);
        ipToNet(ipstr,addr_);
    }

    NetAddress::NetAddress(uint16_t port, uint32_t ipaddr){
        memZero(static_cast<void*>(&(this->addr_)),sizeof(this->addr_));
        this->addr_.sin_family = AF_INET;
        this->addr_.sin_addr.s_addr = toNet32(ipaddr);
        this->addr_.sin_port = toNet16(port);
    }

    NetAddress::NetAddress(sockaddr_in* addr){
        memZero(static_cast<void*>(&(this->addr_)),sizeof(this->addr_));
        this->addr_.sin_family = AF_INET;
        this->addr_.sin_addr.s_addr = toNet32(addr->sin_addr.s_addr);
        this->addr_.sin_port = toNet16(addr->sin_port);
    }

    void Socket::bind(NetAddress& addr){
        if(::bind(sockfd_,
            (const struct sockaddr*)addr.getAddr(),
            static_cast<socklen_t>(sizeof(sockaddr_in)))
            < 0)
        {
            coutErrorLog << "bind error" << std::endl;
        }
    }

    void Socket::listen(){
        if(::listen(sockfd_,SOMAXCONN) < 0)
            coutErrorLog << "listen error" << std::endl;
    }

    int Socket::accept(sockaddr_in* peerAddrRet){
        int connfd = -1;
        sockaddr peeraddr;
        socklen_t addrlen = static_cast<socklen_t>(sizeof(peeraddr));
        if((connfd =::accept(sockfd_,
                        &peeraddr,
                        &addrlen)) < 0)
        {
            coutErrorLog << "accpet Error" << std::endl;
        }
        sockaddr_in* peeraddrPtr = (sockaddr_in*)(&peeraddr);
        memcpy(peerAddrRet,peeraddrPtr,static_cast<size_t>(addrlen));
        return connfd;
    }

    void Socket::connect(NetAddress addr){
        int ret = -1;
        if((ret = ::connect(this->sockfd_,
                            (sockaddr*)addr.getAddr(),
                            static_cast<socklen_t>(sizeof(sockaddr_in))))
                            < 0)
        {
            coutErrorLog << "connect Error" << std::endl;
        }
    }

    void Socket::shutdownWrite(){
        if(::shutdown(sockfd_,SHUT_WR) < 0)
        {
            coutErrorLog << "socket::shutdownWrite error";
        }
    }

    void Socket::setKeepAlive(bool on){
        int optval = on ? 1 : 0;
        ::setsockopt(sockfd_, SOL_SOCKET, SO_KEEPALIVE,
                    &optval,static_cast<socklen_t>(sizeof(optval)));
    }

} // namespace chatRoom
