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
#include "../../smartptr.h"
#include "../cstring.h"



namespace Yuni
{
namespace Extension
{


	template<>
	class IntoCString<char*> final
	{
	public:
		enum { valid = 1, converted = 0, zeroTerminated = 1, };

	public:
		static const char* Perform(const char* const container)
		{
			return container; /* identity */
		}
	};


	template<int N>
	class IntoCString<char[N]> final
	{
	public:
		enum { valid = 1, converted = 0, zeroTerminated = 1, };

	public:
		static const char* Perform(const char* container)
		{
			return container; /* identity */
		}
	};


	template<uint ChunkSizeT, bool ExpandableT>
	class IntoCString<Yuni::CString<ChunkSizeT, ExpandableT> > final
	{
	public:
		typedef Yuni::CString<ChunkSizeT, ExpandableT> CStringType;
		enum { valid = 1, converted = 0, zeroTerminated = CStringType::zeroTerminated, };

	public:
		static const char* Perform(const CStringType& container)
		{
			return container.c_str();
		}
	};

	template<uint ChunkSizeT, bool ExpandableT,
		template <class> class OwspP, template <class> class ChckP, class ConvP,
		template <class> class StorP, template <class> class ConsP>
	class IntoCString<Yuni::SmartPtr<Yuni::CString<ChunkSizeT, ExpandableT>, OwspP, ChckP, ConvP, StorP, ConsP> > final
	{
	public:
		typedef Yuni::CString<ChunkSizeT, ExpandableT> CStringType;
		typedef Yuni::SmartPtr<Yuni::CString<ChunkSizeT, ExpandableT>, OwspP,ChckP,ConvP,StorP,ConsP> CStringTypePtr;
		enum { valid = 1, converted = 0, zeroTerminated = CStringType::zeroTerminated, };

	public:
		static const char* Perform(const CStringTypePtr& container)
		{
			return (!container) ? nullptr : container->c_str();
		}
	};



	template<uint ChunkSizeT, bool ExpandableT>
	class IntoCString<Yuni::CString<ChunkSizeT, ExpandableT>* > final
	{
	public:
		typedef typename Yuni::CString<ChunkSizeT, ExpandableT> CStringType;
		enum { valid = 1, converted = 0, zeroTerminated = CStringType::zeroTerminated, };

	public:
		static const char* Perform(const CStringType* const container)
		{
			return container ? container->data() : nullptr;
		}
	};



	template<class T, class Alloc>
	class IntoCString<std::basic_string<char, T, Alloc> > final
	{
	public:
		enum { valid = 1, converted = 0, zeroTerminated = 1, };

	public:
		static const char* Perform(const std::basic_string<char,T,Alloc>& container)
		{
			return container.c_str();
		}
	};


	template<class T, class Alloc,
		template <class> class OwspP, template <class> class ChckP, class ConvP,
		template <class> class StorP, template <class> class ConsP>
	class IntoCString<Yuni::SmartPtr<std::basic_string<char, T, Alloc>, OwspP, ChckP, ConvP, StorP, ConsP> > final
	{
	public:
		typedef std::basic_string<char, T,Alloc> StringType;
		typedef Yuni::SmartPtr<std::basic_string<char,T,Alloc>, OwspP,ChckP,ConvP,StorP,ConsP> StringTypePtr;
		enum { valid = 1, converted = 0, zeroTerminated = 1, };

	public:
		static const char* Perform(const StringTypePtr& container)
		{
			return (!container) ? nullptr : container->c_str();
		}
	};



	template<class T, class Alloc>
	class IntoCString<std::basic_string<char, T, Alloc>* > final
	{
	public:
		enum { valid = 1, converted = 0, zeroTerminated = 1, };

	public:
		static const char* Perform(const std::basic_string<char,T,Alloc>* const container)
		{
			return container ? container->c_str() : nullptr;
		}
	};


	template<>
	class IntoCString<YuniNullPtr> final
	{
	public:
		enum { valid = 1, converted = 0, zeroTerminated = 1, };

	public:
		static const char* Perform(const YuniNullPtr&)
		{
			return nullptr;
		}
	};






} // namespace Extension
} // namespace Yuni
