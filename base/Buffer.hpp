#ifndef CHATROOM_BASE_BUFFER_HPP
#define CHATROOM_BASE_BUFFER_HPP 0

#include "base/noncopyable.hpp"
#include <vector>
#include <sys/uio.h>

namespace chatRoom
{
	// Used as user buffer

  class Buffer : noncopyable
	{
		public:
			typedef std::vector<char>::size_type size_t;
			
			Buffer(size_t = initialSize);
			
			~Buffer() = default;

			// Read data from the sys buffer of the 
			// file descriptor to Buffer class.
			// Return num of bytes actually read.
			size_t read(int fd);


			void retrieveAll();

			void retrieveLen(size_t len);

			size_t readableSize();

			size_t writeableSize();

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