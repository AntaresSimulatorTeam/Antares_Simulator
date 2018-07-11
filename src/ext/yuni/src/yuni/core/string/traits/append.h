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
#include "../../traits/length.h"
#include "integer.h"
#include <stdio.h>
#include <cassert>

#ifdef YUNI_OS_MSVC
#	define YUNI_PRIVATE_MEMBUF_SPTRINF(BUFFER,SIZE, F, V)  ::sprintf_s(BUFFER,SIZE,F,V)
#else
#	define YUNI_PRIVATE_MEMBUF_SPTRINF(BUFFER,SIZE, F, V)  ::snprintf(BUFFER,SIZE,F,V)
#endif


namespace Yuni
{
namespace Extension
{
namespace CString
{

	template<class CStringT, class C>
	class Append; // specialization required


	// C*
	template<class CStringT, class T>
	class Append<CStringT, T*> final
	{
	public:
		static void Perform(CStringT& s, const T* rhs)
		{
			s += reinterpret_cast<const void*>(rhs);
		}
	};


	// char*
	template<class CStringT>
	class Append<CStringT, char*> final
	{
	public:
		typedef typename CStringT::Type TypeC;
		typedef typename Static::Remove::Const<TypeC>::Type C;
		static void Perform(CStringT& s, const C* rhs)
		{
			if (rhs)
				s.appendWithoutChecking(rhs, Yuni::Traits::Length<C*,typename CStringT::Size>::Value(rhs));
		}
	};

	// C[N]
	template<class CStringT, int N>
	class Append<CStringT, char[N]> final
	{
	public:
		typedef typename CStringT::Type C;
		static void Perform(CStringT& s, const C rhs[N])
		{
			if (N > 0 and rhs)
			{
				// The calculation with `N` is required to properly handle
				// both a zero-terminated buffer and a simple array
				s.appendWithoutChecking(rhs, N - ((rhs[N-1] == C()) ? 1 : 0));
			}
		}
	};

	// C
	template<class CStringT>
	class Append<CStringT, char> final
	{
	public:
		typedef char C;
		static void Perform(CStringT& s, const C rhs)
		{
			s.appendWithoutChecking(rhs);
		}
	};


	// C
	template<class CStringT>
	class Append<CStringT, unsigned char> final
	{
	public:
		typedef unsigned char C;
		static void Perform(CStringT& s, const C rhs)
		{
			s.appendWithoutChecking(static_cast<char>(rhs));
		}
	};

	// nullptr
	template<class CStringT>
	class Append<CStringT, YuniNullPtr> final
	{
	public:
		static void Perform(CStringT&, const YuniNullPtr&)
		{ /* Do nothing */ }
	};


	// bool
	template<class CStringT>
	class Append<CStringT, bool> final
	{
	public:
		static void Perform(CStringT& s, const bool rhs)
		{
			if (rhs)
				s.appendWithoutChecking("true", 4);
			else
				s.appendWithoutChecking("false", 5);
		}
	};


	// wchar_t
	template<class CStringT>
	class Append<CStringT, wchar_t*> final
	{
	public:
		static void Perform(CStringT& string, const wchar_t* rhs)
		{
			string.append(rhs, Yuni::Traits::Length<wchar_t*, typename CStringT::Size>::Value(rhs));
		}
	};

	// wchar_t
	template<class CStringT, int N>
	class Append<CStringT, wchar_t[N]> final
	{
	public:
		static void Perform(CStringT& string, const wchar_t rhs[N])
		{
			if (N > 0 and rhs)
				string.append(rhs, N - ((rhs[N-1] == wchar_t()) ? 1 : 0));
		}
	};


	// void*
	template<class CStringT>
	class Append<CStringT, void*> final
	{
	public:
		static void Perform(CStringT& s, const void* rhs)
		{
			if (not rhs)
			{
				s.appendWithoutChecking("0x0", 3);
			}
			else
			{
				# ifdef YUNI_OS_MSVC
				{
					// With Visual Studio, the option %p does not provide the prefix 0x
					typename CStringT::Type buffer[32];
					buffer[0] = '0';
					buffer[1] = 'x';

					// On Windows, it may return a negative value
					if (YUNI_PRIVATE_MEMBUF_SPTRINF(buffer + 2, sizeof(buffer)-2, "%p", rhs) >= 0)
					{
						s.appendWithoutChecking(buffer,
							Yuni::Traits::Length<typename CStringT::Type*, typename CStringT::Size>::Value(buffer));
					}
					else
						s.appendWithoutChecking("0x0", 3);
				}
				# else
				{
					typename CStringT::Type buffer[32];
					// On Windows, it may return a negative value
					if (YUNI_PRIVATE_MEMBUF_SPTRINF(buffer, sizeof(buffer), "%p", rhs) >= 0)
					{
						s.appendWithoutChecking(buffer,
							Yuni::Traits::Length<typename CStringT::Type*, typename CStringT::Size>::Value(buffer));
					}
					else
						s.appendWithoutChecking("0x0", 3);
				}
				# endif
			}
		}
	};


	// void*
	template<class CStringT>
	class Append<CStringT, Yuni::UTF8::Char> final
	{
	public:
		static void Perform(CStringT& s, const Yuni::UTF8::Char& rhs)
		{
			assert(&rhs != NULL);
			rhs.write(s);
		}
	};




# define YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL(BUFSIZE, FORMAT, TYPE) \
	template<class CStringT> \
	class Append<CStringT, TYPE> final \
	{ \
	public: \
		static void Perform(CStringT& s, const TYPE rhs) \
		{ \
			typename CStringT::Type buffer[BUFSIZE]; \
			/* On Windows, it may return a negative value */ \
			if (YUNI_PRIVATE_MEMBUF_SPTRINF(buffer, BUFSIZE, FORMAT, rhs) >= 0) \
			{ \
				buffer[BUFSIZE - 1] = '\0'; /* making sure that it is zero-terminated */ \
				s.appendWithoutChecking(buffer, \
					Yuni::Traits::Length<typename CStringT::Type*, typename CStringT::Size>::Value(buffer)); \
			} \
		} \
	}

# define YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(TYPE) \
	template<class CStringT> \
	class Append<CStringT, TYPE> final \
	{ \
	public: \
		static void Perform(CStringT& s, const TYPE rhs) \
		{ \
			Yuni::Private::CStringImpl::From<Math::Base::Decimal, TYPE>::AppendTo(s, rhs); \
		} \
	}


	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(sint16);
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(sint32);
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(sint64);
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(uint16);
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(uint32);
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(uint64);
	# ifdef YUNI_HAS_LONG
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(long);
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL_INT(unsigned long);
	# endif

	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL(256, "%f",  float);
	YUNI_PRIVATE_MEMORY_BUFFER_APPEND_IMPL(256, "%f",  double);



	// std::vector<>
	template<class CStringT, class T>
	class Append<CStringT, std::vector<T> > final
	{
	public:
		typedef std::vector<T> ListType;
		static void Perform(CStringT& s, const ListType& rhs)
		{
			s += '[';
			if (not rhs.empty())
			{
				const typename ListType::const_iterator end = rhs.end();
				typename ListType::const_iterator i = rhs.begin();
				s += *i;
				++i;
				for (; i != end; ++i)
					s << ", " << *i;
			}
			s += ']';
		}
	};


	// std::vector<>
	template<class CStringT, class T>
	class Append<CStringT, std::list<T> > final
	{
	public:
		typedef std::list<T> ListType;
		static void Perform(CStringT& s, const ListType& rhs)
		{
			s += '[';
			if (not rhs.empty())
			{
				const typename ListType::const_iterator end = rhs.end();
				typename ListType::const_iterator i = rhs.begin();
				s += *i;
				++i;
				for (; i != end; ++i)
					s << ", " << *i;
			}
			s += ']';
		}
	};






} // namespace CString
} // namespace Extension
} // namespace Yuni

#undef YUNI_PRIVATE_MEMBUF_SPTRINF
