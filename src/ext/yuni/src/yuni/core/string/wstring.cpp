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
#include "wstring.h"
#ifdef YUNI_OS_WINDOWS
#include "../system/windows.hdr.h"
#else
#include <wchar.h>
#endif
#include "limits.h"




namespace Yuni
{

	WString::WString(const WString& rhs)
		: pSize(rhs.pSize)
	{
		pWString = (wchar_t*)::malloc(sizeof(wchar_t) * (pSize + 1));
		memcpy(pWString, rhs.pWString, pSize + 1);
	}


	WString& WString::operator = (const WString& rhs)
	{
		free(pWString);
		pSize = rhs.pSize;
		pWString = (wchar_t*)::malloc(sizeof(wchar_t) * (pSize + 1));
		memcpy(pWString, rhs.pWString, pSize + 1);
		return *this;
	}


	void WString::prepareWString(const AnyString& string, bool uncprefix)
	{
		if (string.empty())
		{
			if (uncprefix)
			{
				pSize = 4;
				pWString = (wchar_t*)::realloc(pWString, sizeof(wchar_t) * 5);
				pWString[0] = L'\\';
				pWString[1] = L'\\';
				pWString[2] = L'?';
				pWString[3] = L'\\';
				pWString[4] = L'\0';
			}
			else
				clear();
			return;
		}

		if (string.size() > INT_MAX)
		{
			clear();
			return;
		}

		// Offset according to the presence of the UNC prefix
		const uint offset = (not uncprefix) ? 0 : 4;

		#ifdef YUNI_OS_WINDOWS
		{
			// Allocate and convert the C-String. Several iterations may be required
			// for allocating enough room for the conversion.
			const int sizeRequired = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), string.size(), nullptr, 0);
			if (sizeRequired <= 0)
			{
				clear();
				return;
			}

			pSize = sizeRequired + offset;

			pWString = (wchar_t*) realloc(pWString, sizeof(wchar_t) * (pSize + 1));
			if (nullptr == pWString) // Impossible to allocate the buffer. Aborting.
			{
				clear();
				return;
			}

			// Converting into Wide String
			const int n = MultiByteToWideChar(CP_UTF8, 0, string.c_str(), static_cast<int>(string.size()), pWString + offset, static_cast<int>(pSize - offset));
			if (n != sizeRequired)
			{
				assert(false and "most likely an error");
				clear();
				return;
			}
		}
		#else
		{
			const char* wcstr = string.c_str();

			mbstate_t state;
			memset (&state, '\0', sizeof (state));

			size_t sizeRequired = mbsnrtowcs(nullptr, &wcstr, string.size(), 0, &state);
			if (0 == sizeRequired or (size_t) -1 == sizeRequired)
			{
				clear();
				return;
			}

			pSize = sizeRequired + offset;

			pWString = (wchar_t*) realloc(pWString, sizeof(wchar_t) * (pSize + 1));
			if (nullptr == pWString) // Impossible to allocate the buffer. Aborting.
			{
				clear();
				return;
			}

			memset (&state, '\0', sizeof (state));
			size_t written = mbsnrtowcs(pWString + offset, &wcstr, string.size(), pSize - offset, &state);
			if (0 == written or (size_t) -1 == written)
			{
				clear();
				return;
			}
		}
		#endif


		// prepend the Windows UNC prefix
		if (uncprefix)
		{
			pWString[0] = L'\\';
			pWString[1] = L'\\';
			pWString[2] = L'?';
			pWString[3] = L'\\';
		}

		// always ensure that the string is zero terminated
		pWString[pSize] = L'\0';
	}




} // namespace Yuni
