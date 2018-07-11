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
#include "../../../yuni.h"
#include "traits.h"
#include <ctype.h>
#ifndef YUNI_OS_WINDOWS
#include <wchar.h>
#else
#include "../../../core/system/windows.hdr.h"
#endif
#include <limits.h>



namespace Yuni
{
namespace Private
{
namespace CStringImpl
{


	bool Equals(const char* const s1, const char* const s2, uint len)
	{
		for (uint i = 0; i != len; ++i)
		{
			if (s1[i] != s2[i])
				return false;
		}
		return true;
	}


	bool EqualsInsensitive(const char* const s1, const char* const s2, uint len)
	{
		for (uint i = 0; i != len; ++i)
		{
			if (tolower(s1[i]) != tolower(s2[i]))
				return false;
		}
		return true;
	}


	int Compare(const char* const s1, uint l1, const char* const s2, uint l2)
	{
		const uint l = (l1 < l2) ? l1 : l2;

		for (uint i = 0; i != l; ++i)
		{
			if (s1[i] != s2[i])
				return (((unsigned char)s1[i] < (unsigned char)s2[i]) ? -1 : +1);
		}
		return (l1 == l2) ? 0 : ((l1 < l2) ? -1 : +1);
	}


	int CompareInsensitive(const char* const s1, uint l1, const char* const s2, uint l2)
	{
		const uint l = (l1 < l2) ? l1 : l2;

		for (uint i = 0; i != l; ++i)
		{
			if (tolower(s1[i]) != tolower(s2[i]))
				return ((tolower((unsigned char)s1[i]) < tolower((unsigned char)s2[i])) ? -1 : +1);
		}
		return (l1 == l2) ? 0 : ((l1 < l2) ? -1 : +1);
	}


	bool Glob(const char* const s, uint l1, const char* const pattern, uint patternlen)
	{
		if (patternlen)
		{
			if (l1)
			{
				uint e = 0;
				uint prev = ((uint) -1);
				for (uint i = 0 ; i < l1; ++i)
				{
					if ('*' == pattern[e])
					{
						if (e + 1 == patternlen)
							return true;
						while (pattern[e+1] == '*')
							++e;
						if (e + 1 == patternlen)
							return true;

						prev = e;
						if (pattern[e + 1] == s[i])
							e += 2;
					}
					else
					{
						if (pattern[e] == s[i])
							++e;
						else
						{
							if (prev != ((uint) -1))
								e = prev;
							else
								return false;
						}
					}
				}
				return (e == patternlen);
			}
			return false;
		}
		return (l1 == 0);
	}



	size_t WCharToUTF8SizeNeeded(const wchar_t* wbuffer, size_t length)
	{
		if (length > INT_MAX) // consistency between platforms (windows)
			return 0;

		#ifndef YUNI_OS_WINDOWS
		{
			mbstate_t state;
			memset (&state, '\0', sizeof (state));
			size_t written = wcsnrtombs(nullptr, &wbuffer, length, 0, &state);
			return (written != (size_t) -1) ? written : 0;
		}
		#else
		{
			int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wbuffer, static_cast<int>(length), nullptr, 0,  nullptr, nullptr);
			return sizeRequired > 0 ? (size_t) sizeRequired : 0;
		}
		#endif
	}


	size_t WCharToUTF8(char*& out, size_t maxlength, const wchar_t* wbuffer, size_t length)
	{
		#ifndef YUNI_OS_WINDOWS
		{
			mbstate_t state;
			memset (&state, '\0', sizeof (state));
			size_t written = wcsnrtombs(out, &wbuffer, length, maxlength, &state);
			return (written != (size_t) -1) ? written : 0;
		}
		#else
		{
			int written = WideCharToMultiByte(CP_UTF8, 0, wbuffer, static_cast<int>(length), out, maxlength,  nullptr, nullptr);
			return (written > 0) ? (size_t) written : 0;
		}
		#endif
	}





} // namespace CStringImpl
} // namespace Private
} // namespace Yuni
