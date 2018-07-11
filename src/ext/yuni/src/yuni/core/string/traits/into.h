/*
** This file is part of libyuni, a cross-platform C++ framework (http://libyuni.org).
**
** This Source Code Form is subject to the terms of the Mozilla Public License
** v.2.0. If a copy of the MPL was not distributed with this file, You can
** obtain one at http://mozilla.org/MPL/2.0/.
**
** github: https://github.com/libyuni/libyuni/
** gitlab: https://gitlab.com/libyuni/libyuni/ (mirror)
*/
#pragma once
#include <stdlib.h>
#include <ctype.h>



namespace Yuni
{
namespace Extension
{
namespace CString
{


	/*!
	** \brief Generic implementation
	*/
	template<class T>
	class Into final
	{
	public:
		enum { valid = 0, };

		template<class StringT> static bool Perform(const StringT&, T& out)
		{
			out = T();
			return true;
		}

		template<class StringT> static T Perform(const StringT&)
		{
			return T();
		}
	};


	/*!
	** \brief const char*
	*/
	template<>
	class Into<char*> final
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, const char*& out)
		{
			out = s.data();
			return true;
		}

		template<class StringT> static const char* Perform(const StringT& s)
		{
			return s.data();
		}
	};




	/*!
	** \brief String, with the same POD type
	*/
	template<uint ChunkSizeT, bool ExpandableT>
	class Into<Yuni::CString<ChunkSizeT, ExpandableT> > final
	{
	public:
		typedef Yuni::CString<ChunkSizeT, ExpandableT> TargetType;
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, TargetType& out)
		{
			out = s;
			return true;
		}

		template<class StringT> static TargetType Perform(const StringT& s)
		{
			return s;
		}
	};


	/*!
	** \brief std::string
	*/
	template<class CharT, class TraitsT, class AllocT>
	class Into<std::basic_string<CharT,TraitsT, AllocT> > final
	{
	public:
		typedef std::basic_string<CharT,TraitsT, AllocT> TargetType;
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, TargetType& out)
		{
			out.assign(s.c_str(), s.size());
			return true;
		}

		template<class StringT> static TargetType Perform(const StringT& s)
		{
			return TargetType(s.c_str(), s.size());
		}
	};


	/*!
	** \brief char
	*/
	template<>
	class Into<char> final
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, char& out)
		{
			out = (!s) ? '\0' : s[0];
			return true;
		}

		template<class StringT> static char Perform(const StringT& s)
		{
			return (!s) ? '\0' : s[0];
		}
	};


	/*!
	** \brief unsigned char
	*/
	template<>
	class Into<unsigned char> final
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, unsigned char& out)
		{
			out = static_cast<unsigned char>((!s) ? '\0' : s[0]);
			return true;
		}

		template<class StringT> static unsigned char Perform(const StringT& s)
		{
			return static_cast<unsigned char>((!s) ? '\0' : s[0]);
		}
	};


	/*!
	** \brief char[]
	*/
	template<int N>
	class Into<char[N]> final
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, char*& out)
		{
			if (StringT::zeroTerminated)
			{
				// We have to copy the final zero in the same time
				size_t length = (N-1) < s.sizeInBytes() ? N : s.sizeInBytes() + 1;
				YUNI_MEMCPY(out, length, s.data(), length);
			}
			else
			{
				// The N char can be used
				size_t length = (N) < s.sizeInBytes() ? N : s.sizeInBytes();
				YUNI_MEMCPY(out, length, s.data(), length);
			}
			return true;
		}

		template<class StringT> static char* Perform(const StringT& s)
		{
			return s.data();
		}
	};



	/*!
	** \brief bool
	*/
	template<>
	class Into<bool> final
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, bool& out)
		{
			out = Perform(s);
			return true;
		}

		template<class StringT> static bool Perform(const StringT& s)
		{
			if (not s.empty())
			{
				const uint count = static_cast<uint>(s.sizeInBytes());
				if (count < 5)
				{
					if (count == 1)
					{
						return ('1' == s[0] or 'Y' == s[0] or 'y' == s[0] or 'O' == s[0]
							||  'o' == s[0] or 't' == s[0] or 'T' == s[0]);
					}

					char buffer[5] = {0,0,0,0,0};
					for (uint i = 0; i != count; ++i)
						buffer[i] = static_cast<char>(::tolower(s[i]));
					return (!::strcmp("true", buffer) or !::strcmp("on", buffer) or !::strcmp("yes", buffer));
				}
			}
			return false;
		}
	};




	namespace // anonymous
	{

		class AutoDetectBaseNumber final
		{
		public:
			static inline const char* Value(const char* const s, uint length, int& base)
			{
				switch (s[0])
				{
					case '#' :
						{
							base = 16;
							return s + 1;
						}
					case '0' :
						{
							if (length > 2 and (s[1] == 'x' or s[1] == 'X'))
							{
								base = 16;
								return s + 2;
							}
						}
				}
				base = 10;
				return s;
			}

		}; // class AutoDetectBaseNumber

	} // anonymous namespace




# define YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(TYPE,CONVERT)  \
	template<> \
	class Into<TYPE> final \
	{ \
	public: \
		typedef TYPE IntoType; \
		enum { valid = 1 }; \
		enum { bufferSize = 256u }; \
		\
		template<class StringT> static bool Perform(const StringT& s, IntoType& out) \
		{ \
			if (s.empty()) \
			{ \
				out = IntoType(); \
				return true; \
			} \
			char* pend; \
			int base; \
			if (not StringT::zeroTerminated) \
			{ \
				char buffer[bufferSize]; \
				if (s.size() < bufferSize) \
				{ \
					YUNI_MEMCPY(buffer, bufferSize, s.data(), s.size()); \
					buffer[s.size()] = '\0'; \
				} \
				else \
				{ \
					YUNI_MEMCPY(buffer, bufferSize, s.data(), bufferSize - 1); \
					buffer[bufferSize - 1] = '\0'; \
				} \
				const char* p = AutoDetectBaseNumber::Value(buffer, s.size(), base); \
				out = static_cast<IntoType>(::CONVERT(p, &pend, base)); \
				return (NULL != pend and '\0' == *pend); \
			} \
			else \
			{ \
				const char* p = AutoDetectBaseNumber::Value(s.c_str(), s.size(), base); \
				out = static_cast<IntoType>(::CONVERT(p, &pend, base)); \
				return NULL != pend and (pend - p == s.size()); \
			} \
		} \
		\
		template<class StringT> static IntoType Perform(const StringT& s) \
		{ \
			if (s.empty()) \
				return IntoType(); \
			char* pend; \
			int base; \
			if (!StringT::zeroTerminated) \
			{ \
				char buffer[bufferSize]; \
				if (s.size() < bufferSize) \
				{ \
					YUNI_MEMCPY(buffer, bufferSize, s.data(), s.size()); \
					buffer[s.size()] = '\0'; \
				} \
				else \
				{ \
					YUNI_MEMCPY(buffer, bufferSize, s.data(), bufferSize - 1); \
					buffer[bufferSize - 1] = '\0'; \
				} \
				const char* p = AutoDetectBaseNumber::Value(buffer, s.size(), base); \
				return static_cast<IntoType>(::CONVERT(p, &pend, base)); \
			} \
			else \
			{ \
				const char* p = AutoDetectBaseNumber::Value(s.c_str(), s.size(), base); \
				return static_cast<IntoType>(::CONVERT(p, &pend, base)); \
			} \
		} \
	}



	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(sint16, strtol);
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(sint32, strtol);
	# ifdef YUNI_OS_MSVC
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(sint64, _strtoi64);
	# else
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(sint64, strtoll);
	# endif

	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(uint16, strtoul);
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(uint32, strtoul);
	# ifdef YUNI_OS_MSVC
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(uint64, _strtoui64);
	# else
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(uint64, strtoull);
	# endif


	# ifdef YUNI_HAS_LONG
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(long, strtol);
	YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC(unsigned long, strtoul);
	# endif


# undef YUNI_CORE_EXTENSION_ISTRING_TO_NUMERIC




	/*!
	** \brief float
	*/
	template<>
	class Into<float> final
	{
	public:
		enum { valid = 1 };
		enum { bufferSize = 256u };

		template<class StringT> static bool Perform(const StringT& s, float& out)
		{
			if (not s.empty())
			{
				char* pend;
				const char* cstr;
				char buffer[bufferSize];
				if (!StringT::zeroTerminated)
				{
					if (s.size() < bufferSize)
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), s.size());
						buffer[s.size()] = '\0';
					}
					else
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), bufferSize - 1);
						buffer[bufferSize - 1] = '\0';
					}
					cstr = buffer;
				}
				else
					cstr = s.c_str();

				# ifdef YUNI_OS_MSVC
				// Visual Studio does not support strtof
				out = static_cast<float>(strtod(cstr, &pend));
				# else
				out = static_cast<float>(strtof(cstr, &pend));
				# endif
				return (pend and pend - cstr == s.size());
			}
			out = 0.f;
			return true;
		}

		template<class StringT> static float Perform(const StringT& s)
		{
			if (not s.empty())
			{
				char* pend;
				const char* cstr;
				char buffer[bufferSize];
				if (!StringT::zeroTerminated)
				{
					if (s.size() < bufferSize)
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), s.size());
						buffer[s.size()] = '\0';
					}
					else
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), bufferSize - 1);
						buffer[bufferSize - 1] = '\0';
					}
					cstr = buffer;
				}
				else
					cstr = s.c_str();

				# ifdef YUNI_OS_MSVC
				// Visual Studio does not support strtof
				return static_cast<float>(::strtod(cstr, &pend));
				# else
				return static_cast<float>(::strtof(cstr, &pend));
				# endif
			}
			return 0.f;
		}
	};


	/*!
	** \brief double
	*/
	template<>
	class Into<double> final
	{
	public:
		enum { valid = 1 };
		enum { bufferSize = 256u };

		template<class StringT> static bool Perform(const StringT& s, double& out)
		{
			if (not s.empty())
			{
				char* pend;
				const char* cstr;
				char buffer[bufferSize];
				if (!StringT::zeroTerminated)
				{
					if (s.size() < bufferSize)
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), s.size());
						buffer[s.size()] = '\0';
					}
					else
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), bufferSize - 1);
						buffer[bufferSize - 1] = '\0';
					}
					cstr = buffer;
				}
				else
					cstr = s.c_str();

				out = static_cast<double>(::strtod(cstr, &pend));
				return (NULL != pend and '\0' == *pend);
			}
			out = 0.;
			return true;
		}

		template<class StringT> static double Perform(const StringT& s)
		{
			if (not s.empty())
			{
				char* pend;
				const char* cstr;
				char buffer[bufferSize];
				if (!StringT::zeroTerminated)
				{
					if (s.size() < bufferSize)
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), s.size());
						buffer[s.size()] = '\0';
					}
					else
					{
						YUNI_MEMCPY(buffer, bufferSize, s.data(), bufferSize - 1);
						buffer[bufferSize - 1] = '\0';
					}
					cstr = buffer;
				}
				else
					cstr = s.c_str();

				return static_cast<double>(::strtod(cstr, &pend));
			}
			return 0.;
		}
	};


	/*!
	** \brief const void*
	*/
	template<>
	class Into<void*> final
	{
	public:
		enum { valid = 1 };

		template<class StringT> static bool Perform(const StringT& s, const void*& out)
		{
			Static::If<sizeof(void*) == 4, uint32, uint64>::Type  p;
			if (Into<uint32>::Perform(s, p))
			{
				out = reinterpret_cast<void*>(p);
				return true;
			}
			out = 0x0;
			return false;
		}

		template<class StringT> static void* Perform(const StringT& s)
		{
			return reinterpret_cast<void*>((sizeof(void*) == 4)
				? Into<uint32>::Perform(s) : Into<uint64>::Perform(s));
		}
	};





} // namespace CString
} // namespace Extension
} // namespace Yuni
