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
#include "bit.h"




namespace Yuni
{
namespace Bit
{

	inline yuint32 Count(yuint32 i)
	{
		#ifdef YUNI_HAS_GCC_BUILTIN_POPCOUNT
		return static_cast<yuint32>(__builtin_popcount(i));
		#else
		// variable-precision SWAR algorithm
		i = i - ((i >> 1) & 0x55555555);
		i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
		return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
		#endif
	}


	template<class T>
	inline uint Count(T data)
	{
		uint c = 0;
		while (data)
		{
			c += c & 0x1u;
			data >>= 1;
		}
		return c;
	}


	inline bool Get(const char* data, uint index)
	{
		# ifdef YUNI_OS_MSVC
		return (YUNI_BIT_GET(data, index)) ? true : false;
		# else
		return YUNI_BIT_GET(data, index);
		# endif
	}


	inline void Set(char* data, uint index)
	{
		//data[index >> 3] |= 1 << ((ENDIANESS) ? (7 - (i) & 7) : (i) & 7);
		YUNI_BIT_SET(data, index);
	}


	inline void Unset(char* data, uint index)
	{
		//data[index >> 3] ~= (1 << ((ENDIANESS) ? (7 - (index) & 7) : (index) & 7));
		YUNI_BIT_UNSET(data, index);
	}




} // namespace Bit
} // namespace Yuni

