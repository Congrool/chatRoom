#include "base/Buffer.hpp"
#include "base/ErrorLog.hpp"

#include <assert.h>
#include <errno.h>
#include <algorithm>

namespace chatRoom
{
    Buffer::Buffer(size_t size)
    : buffer_(size+prefixSize_),
    readableStart_(prefixSize_),
    writeableStart_(prefixSize_)
    { }

    // When read from the socket, we don't know how much
    // bytes will be read.
    size_t Buffer::read(int fd){
        char extraBuf[buffSizeOnStack];
        struct iovec buff[2];
        buff[0].iov_base = begin()+writeableStart_;
        buff[0].iov_len = writeableSize();
        buff[1].iov_base = extraBuf;
        buff[1].iov_len = buffSizeOnStack;
        int ionum = writeableSize() < buffSizeOnStack ? 2 : 1;
        ssize_t hasRead = ::readv(fd, buff, 2);
        if(hasRead < 0){
            // Handle Error
            int savedErrno = errno;
            if(savedErrno == EINVAL)
                coutErrorLog << "readv error: EINVAL";
        }
        else if(static_cast<size_t>(hasRead) <= writeableSize()){
            writeableStart_ += hasRead;
        }
        else{
            size_t tocpy = static_cast<size_t>(hasRead) - writeableSize();
            buffer_.resize(tocpy);
            std::copy(extraBuf,extraBuf+tocpy,buffer_.end());
            writeableStart_ += hasRead;
        }
        return hasRead;
    }


    void Buffer::retrieveAll(){
        readableStart_ = prefixSize_;
        writeableStart_ = prefixSize_;
    }

    void Buffer::retrieveLen(size_t len){
        readableStart_ += len;
    }

    size_t Buffer::readableSize(){
        return static_cast<size_t>(writeableStart_ - readableStart_);
    }

    size_t Buffer::writeableSize(){
        return static_cast<size_t>(buffer_.size() - writeableStart_);
    }

} // namespace chatRoom
