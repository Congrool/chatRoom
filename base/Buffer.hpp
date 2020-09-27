#ifndef CHATROOM_BASE_BUFFER_HPP
#define CHATROOM_BASE_BUFFER_HPP 0

#include "base/noncopyable.hpp"
#include <vector>
#include <sys/uio.h>
#include <memory.h>

namespace chatRoom
{
	// Used as user buffer

	// FIXME:
	// We should reorganize the buffer, 
	// when readableStart is vary large.
	// It exhausts the memory.

  	class Buffer 
	{
		public:
			typedef std::vector<char>::size_type size_t;
			
			Buffer(size_t = initialSize);
			
			~Buffer() = default;

			// Read data from the sys buffer of the 
			// file descriptor to Buffer class.
			// Return num of bytes actually read.
			size_t read(int fd);

			size_t write(int fd);

			void append(const char* buff, size_t len);

			void append(char ch)
			{ buffer_.emplace_back(ch); }

			void retrieveAll();

			void retrieveLen(size_t len);

			size_t readableSize() const;

			size_t writableSize() const;

			const char* readStart() 
			{ return begin() + readableStart_; } 

			char* writeStart()
			{ return begin() + writeableStart_; }

			// FIXME:
			// Maybe it should not be exposed to outside.
			// It's not safe.
			char* begin() { return buffer_.begin().base(); }

			size_t size() { return buffer_.size(); }

		private:
			const static size_t prefixSize_ = 8;
			const static size_t buffSizeOnStack = 65536;
			const static size_t initialSize = 1024;

			int readableStart_;

			int writeableStart_;

			std::vector<char> buffer_;

	};

} // namespace chatRoom


#endif // CHATROOM_BASE_BUFFER_HPP