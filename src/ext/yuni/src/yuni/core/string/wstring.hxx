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
#include "wstring.h"


namespace Yuni
{

	inline WString::WString()
		: pWString()
		, pSize()
	{}


	#ifdef YUNI_HAS_CPP_MOVE
	inline WString::WString(WString&& rhs)
		: pWString(rhs.pWString)
		, pSize(rhs.pSize)
	{
		rhs.pWString = nullptr;
		rhs.pSize = 0;
	}
	#endif


	inline WString::WString(const AnyString& string, bool uncprefix)
		: pWString()
		, pSize()
	{
		prepareWString(string, uncprefix);
	}


	inline WString::~WString()
	{
		free(pWString);
	}


	inline void WString::assign(const AnyString& string, bool uncprefix)
	{
		prepareWString(string, uncprefix);
	}


	inline void WString::clear()
	{
		free(pWString);
		pWString = nullptr;
		pSize = 0;
	}


	inline uint WString::size() const
	{
		return static_cast<uint>(pSize);
	}


	inline void WString::replace(wchar_t from, wchar_t to)
	{
		for (size_t i = 0; i != pSize; ++i)
		{
			if (pWString[i] == from)
				pWString[i] = to;
		}
	}


	inline bool WString::empty() const
	{
		return (0 == pSize);
	}


	inline wchar_t* WString::data()
	{
		return pWString;
	}


	inline const wchar_t* WString::c_str() const
	{
		return pWString ? pWString : L"";
	}


	#ifdef YUNI_HAS_CPP_MOVE
	inline WString& WString::operator = (WString&& rhs)
	{
		free(pWString);
		pWString = rhs.pWString;
		pSize = rhs.pSize;
		rhs.pWString = nullptr;
		rhs.pSize = 0;
		return *this;
	}
	#endif


	inline WString& WString::operator = (const AnyString& string)
	{
		prepareWString(string, false);
		return *this;
	}




} // namespace Yuni
