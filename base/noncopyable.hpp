#ifndef CHATROOM_BASE_NONCOPYABLE_HPP
#define CHATROOM_BASE_NONCOPYABLE_HPP 0

namespace chatRoom{

class noncopyable{
	public:
		noncopyable(const noncopyable&) = delete;
		void operator=(const noncopyable&) = delete;
		
	protected:
		noncopyable() = default;
		~noncopyable() = default;
};

}

#endif // CHATROOM_BASE_NONCOPYABLE_HPP