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


namespace Yuni
{
namespace Extension
{
namespace CString
{


	template<class CStringT, class StringT>
	class Fill final
	{
	public:
		static void Perform(char* data, typename CStringT::Size size, const StringT& pattern)
		{
			const uint patternSize = Traits::Length<StringT,uint>::Value(pattern);
			if (0 == patternSize)
				return;

			const char* const cstr = Traits::CString<StringT>::Perform(pattern);
			// If equals to 1, it is merely a single char
			if (1 == patternSize)
			{
				for (typename CStringT::Size i = 0; i < size; ++i)
					data[i] = *cstr;
				return;
			}
			// We have to copy N times the pattern
			typename CStringT::Size p = 0;
			while (p + patternSize <= size)
			{
				YUNI_MEMCPY(data + p, patternSize * sizeof(char), cstr, patternSize * sizeof(char));
				p += patternSize;
			}
			for (; p < size; ++p)
				data[p] = ' ';
		}
	};


	template<class CStringT>
	class Fill<CStringT, char> final
	{
	public:
		static void Perform(char* data, typename CStringT::Size size, const char rhs)
		{
			for (typename CStringT::Size i = 0; i != size; ++i)
				data[i] = rhs;
		}
	};






} // namespace CString
} // namespace Extension
} // namespace Yuni
