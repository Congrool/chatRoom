#ifndef ERRORLOG_HPP
#define ERRORLOG_HPP 0


#include "base/noncopyable.hpp"

#include<iostream>
#include<sstream>
namespace chatRoom
{

	class ErrorLog : noncopyable{
		public:
			ErrorLog(std::ostream& stream = std::cout) : defaultStream(stream){};

			std::ostream& operator<<(std::string& msg){
				defaultStream << msg;
				return defaultStream;
			}

			template<int charArrayLen>
			std::ostream& operator<<(const char(&msg)[charArrayLen]){
				defaultStream << msg;
				return defaultStream;
			}

		private:
			std::ostream& defaultStream;
	};

	ErrorLog coutErrorLog;

} // namespace chatRoom

#endif // ERRORLOG_HPP