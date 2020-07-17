#include<string>
#include<yuni/core/string/string.h>


using namespace std;

namespace Antares
{
	namespace UnitTests
	{
	
		class fakeLogger;

		class Buffer
		{
		public:
			Buffer() {}

			template<typename U> 
			Buffer& operator << (const U& u)
			{
				// Appending the piece of message to the buffer
				buffer_.append(u);
				return *this;
			}

			void clear() { buffer_.clear(); }
			string content() { return buffer_.to<string>(); }
			bool contains(string sub_string) { return buffer_.contains(sub_string); }
			bool empty() { return buffer_.empty(); }
		private:
			Yuni::CString<1024> buffer_;
		};


		class fakeLogger
		{
		public:
			fakeLogger() {}

			Buffer& error() { return error_buffer_; }
			Buffer& info() { return info_buffer_; }
			Buffer& debug() { return debug_buffer_; }
			Buffer& warning() { return warning_buffer_; }

		private:
			Buffer error_buffer_;
			Buffer info_buffer_;
			Buffer debug_buffer_;
			Buffer warning_buffer_;
		};


	}	// UnitTests

	extern UnitTests::fakeLogger  logs;

}	// Antares


