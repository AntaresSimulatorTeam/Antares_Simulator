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
#include "../../../yuni.h"
#include <string>
#include <cstring>
#include "../../smartptr.h"
#include "../length.h"



namespace Yuni
{
namespace Extension
{


	// C{N}
	template<int N, class SizeT>
	class Length<char[N], SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	public:
		static SizeT Value(const char* const container)
		{
			// This value can not really be known at compile time
			// We may encounter literal strings :
			// "abc" -> N = 4 but the real length is 3
			// or a static buffer  char v[42] where the real length is 42
			return (N == 0) ? 0 : ('\0' == container[N-1] ? N-1 : N);
		}
	};


	// A mere CString (zero-terminated)
	template<class SizeT>
	class Length<char*, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	public:
		static SizeT Value(const char* const container)
		{
			return container ? static_cast<SizeT>(::strlen(container)) : 0u;
		}
	};


	// A mere wide string (zero-terminated)
	template<class SizeT>
	class Length<wchar_t*, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	public:
		static SizeT Value(const wchar_t* const container)
		{
			return container ? static_cast<SizeT>(::wcslen(container)) : 0u;
		}
	};



	// single char

	template<class SizeT>
	class Length<char, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 1, fixedLength = 1, };

	public:
		static SizeT Value(const char) {return 1u;}
	};


	// A single wide char
	template<class SizeT>
	class Length<wchar_t, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 1, fixedLength = 1, };

	public:
		static SizeT Value(const wchar_t) {return 2u;}
	};





	// CString
	template<uint ChunkSizeT, bool ExpandableT, class SizeT>
	class Length<Yuni::CString<ChunkSizeT, ExpandableT>, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	private:
		typedef Yuni::CString<ChunkSizeT, ExpandableT> CStringType;

	public:
		static SizeT Value(const CStringType& container)
		{
			return static_cast<SizeT>(container.size());
		}
	};


	template<uint ChunkSizeT, bool ExpandableT,
		template <class> class OwspP, template <class> class ChckP, class ConvP,
		template <class> class StorP, template <class> class ConsP, class SizeT>
	class Length<Yuni::SmartPtr<Yuni::CString<ChunkSizeT, ExpandableT>, OwspP, ChckP, ConvP, StorP, ConsP>, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	private:
		typedef Yuni::CString<ChunkSizeT, ExpandableT> CStringType;
		typedef Yuni::SmartPtr<Yuni::CString<ChunkSizeT, ExpandableT>, OwspP,ChckP,ConvP,StorP,ConsP> CStringTypePtr;

	public:
		static SizeT Value(const CStringTypePtr& container)
		{
			return (!container) ? 0 : static_cast<SizeT>(container->size());
		}
	};


	template<uint ChunkSizeT, bool ExpandableT, class SizeT>
	class Length<Yuni::CString<ChunkSizeT, ExpandableT>*, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	public:
		static SizeT Value(const Yuni::CString<ChunkSizeT, ExpandableT>* const container)
		{
			return (container) ? static_cast<SizeT>(container->size()) : 0u;
		}
	};



	// std::string
	template<class C, class T, class Alloc, class SizeT>
	class Length<std::basic_string<C, T, Alloc>, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	private:
		typedef std::basic_string<C,T,Alloc> StringType;

	public:
		static SizeT Value(const StringType& container)
		{
			return static_cast<SizeT>(container.size());
		}
	};


	template<class C, class T, class Alloc, class SizeT,
		template <class> class OwspP, template <class> class ChckP, class ConvP,
		template <class> class StorP, template <class> class ConsP>
	class Length<Yuni::SmartPtr<std::basic_string<C, T, Alloc>, OwspP, ChckP, ConvP, StorP, ConsP>, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	private:
		typedef std::basic_string<C,T,Alloc> StringType;
		typedef Yuni::SmartPtr<std::basic_string<C,T,Alloc>,OwspP,ChckP,ConvP,StorP,ConsP> StringTypePtr;

	public:
		static SizeT Value(const StringTypePtr& container)
		{
			return (!container) ? 0 : static_cast<SizeT>(container->size());
		}
	};


	template<class C, class T, class Alloc, class SizeT>
	class Length<std::basic_string<C, T, Alloc>*, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 0, fixedLength = 0, };

	private:
		typedef std::basic_string<C,T,Alloc> StringType;

	public:
		static SizeT Value(const StringType* const container)
		{
			return container ? static_cast<SizeT>(container->size()) : 0u;
		}
	};




	// nulptr

	template<class SizeT>
	class Length<YuniNullPtr, SizeT> final
	{
	public:
		typedef SizeT SizeType;
		enum { valid = 1, isFixed = 1, fixedLength = 1, };

	public:
		static SizeT Value(const YuniNullPtr&)
		{
			return 0u;
		}
	};




} // namespace Extension
} // namespace Yuni
