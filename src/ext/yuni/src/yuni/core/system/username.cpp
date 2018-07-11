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
#include "../../yuni.h"

#ifdef YUNI_OS_WINDOWS
# include "username.h"
# include "windows.hdr.h"


namespace Yuni
{
namespace Private
{
namespace System
{

	uint WindowsUsername(char* cstring, uint size)
	{
		enum
		{
			// The maximum length, (see UCLEN)
			defaultSize = 256,
		};
		DWORD unwsize = defaultSize;

		wchar_t unw[defaultSize];
		if (GetUserNameW(unw, &unwsize))
		{
			if (unwsize > 0)
			{
				// The variable `unwsize` contains the final zero
				--unwsize;
				// Getting the size of the buffer into UTF8
				int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, unw, unwsize, NULL, 0,  NULL, NULL);
				if (sizeRequired > 0)
				{
					if (static_cast<uint>(sizeRequired) > size)
						sizeRequired = size;
					WideCharToMultiByte(CP_UTF8, 0, unw, unwsize, cstring, sizeRequired,  NULL, NULL);
					return static_cast<uint>(sizeRequired);
				}
			}
		}
		return 0;
	}




} // namespace System
} // namespace Private
} // namespace Yuni

#endif
