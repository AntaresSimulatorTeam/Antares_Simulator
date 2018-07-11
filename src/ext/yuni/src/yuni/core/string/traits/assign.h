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
	class Assign final
	{
	public:
		static void Perform(CStringT& s, const C& rhs)
		{
			// By Default, we will clear the buffer and then append the new content
			// Which is the standard behavior but may not the most efficient way
			s.clear();
			Yuni::Extension::CString::Append<CStringT, C>::Perform(s, rhs);
		}
	};


	// T*
	template<class CStringT, class T>
	class Assign<CStringT, T*> final
	{
	public:
		static void Perform(CStringT& s, const T* rhs)
		{
			s = reinterpret_cast<void*>(rhs);
		}
	};


	// char*
	template<class CStringT>
	class Assign<CStringT, void*> final
	{
	public:
		static void Perform(CStringT& s, const void* rhs)
		{
			if (!rhs)
			{
				s.assignWithoutChecking("0x0", 3);
			}
			else
			{
				# ifdef YUNI_OS_MSVC
				// With Visual Studio, the option %p does not provide the prefix 0x
				typename CStringT::Type buffer[32];
				buffer[0] = '0';
				buffer[1] = 'x';
				// On Windows, it may return a negative value
				if (YUNI_PRIVATE_MEMBUF_SPTRINF(buffer + 2, sizeof(buffer) - 2, "%p", rhs) >= 0)
				{
					s.appendWithoutChecking(buffer,
						Yuni::Traits::Length<typename CStringT::Type*, typename CStringT::Size>::Value(buffer));
				}
				else
					s.assignWithoutChecking("0x0", 3);
				# else
				typename CStringT::Type buffer[32];
				// On Windows, it may return a negative value
				if (YUNI_PRIVATE_MEMBUF_SPTRINF(buffer, sizeof(buffer), "%p", rhs) >= 0)
				{
					s.assignWithoutChecking(buffer,
						Yuni::Traits::Length<typename CStringT::Type*, typename CStringT::Size>::Value(buffer));
				}
				else
					s.assignWithoutChecking("0x0", 3);
				# endif
			}
		}
	};



	// char*
	template<class CStringT>
	class Assign<CStringT, char*> final
	{
	public:
		static void Perform(CStringT& s, const char* rhs)
		{
			if (rhs)
			{
				s.assignWithoutChecking(rhs,
					Yuni::Traits::Length<char*, typename CStringT::Size>::Value(rhs));
			}
			else
				s.clear();
		}
	};


	// C[N]
	template<class CStringT, int N>
	class Assign<CStringT, char[N]> final
	{
	public:
		typedef char C;
		static void Perform(CStringT& s, const C rhs[N])
		{
			if (N > 0)
			{
				// The calculation with `N` is required to properly handle
				// both a zero-terminated buffer and a simple array
				s.assignWithoutChecking(rhs, N - ((rhs[N-1] == C()) ? 1 : 0));
			}
			else
				s.clear();
		}
	};


	// C
	template<class CStringT>
	class Assign<CStringT, char> final
	{
	public:
		static void Perform(CStringT& s, const char rhs)
		{
			s.assignWithoutChecking(rhs);
		}
	};


	// nullptr
	template<class CStringT>
	class Assign<CStringT, YuniNullPtr> final
	{
	public:
		static void Perform(CStringT& s, const YuniNullPtr&)
		{
			s.clear();
		}
	};



	// bool
	template<class CStringT>
	class Assign<CStringT, bool> final
	{
	public:
		static void Perform(CStringT& s, const bool rhs)
		{
			if (rhs)
				s.assignWithoutChecking("true", 4);
			else
				s.assignWithoutChecking("false", 5);
		}
	};






} // namespace CString
} // namespace Extension
} // namespace Yuni

#undef YUNI_PRIVATE_MEMBUF_SPTRINF
