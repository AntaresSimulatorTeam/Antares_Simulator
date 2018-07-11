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
#include "../io.h"
#include "../directory.h"
#include "commons.h"
#include "../file.h"
#ifdef YUNI_OS_WINDOWS
# include "../../core/string/wstring.h"
#endif


namespace Yuni
{
namespace IO
{
namespace Directory
{


	# ifdef YUNI_OS_WINDOWS

	static bool WindowsMake(const AnyString& path)
	{
		String norm;
		Yuni::IO::Normalize(norm, path);

		WString wstr(norm, true);
		if (wstr.size() < 4)
			return false;
		wchar_t* t = wstr.data() + 4;

		while (*t != L'\0')
		{
			if ((*t == L'\\' or *t == L'/') and (*(t-1) != ':'))
			{
				*t = L'\0';
				if (!CreateDirectoryW(wstr.c_str(), nullptr))
				{
					if (GetLastError() != ERROR_ALREADY_EXISTS)
						return false;
				}
				*t = L'\\';
			}
			++t;
		}

		if (not CreateDirectoryW(wstr.c_str(), nullptr))
		{
			if (GetLastError() != ERROR_ALREADY_EXISTS)
				return false;
		}
		return true;
	}


	# else



	static bool UnixMake(const AnyString& path, uint mode)
	{
		const uint len = path.size();
		char* buffer = new char[len + 1];
		YUNI_MEMCPY(buffer, len, path.c_str(), len);
		buffer[len] = '\0';
		char* pt = buffer;
		char tmp;

		do
		{
			if ('\\' == *pt or '/' == *pt or '\0' == *pt)
			{
				tmp = *pt;
				*pt = '\0';
				if ('\0' != buffer[0] and '\0' != buffer[1] and '\0' != buffer[2])
				{
					if (mkdir(buffer, static_cast<mode_t>(mode)) < 0)
					{
						if (errno != EEXIST and errno != EISDIR and errno != ENOSYS)
						{
							delete[] buffer;
							return false;
						}
					}
				}
				if ('\0' == tmp)
					break;
				*pt = tmp;
			}
			++pt;
		}
		while (true);

		delete[] buffer;
		return true;
	}

	# endif






	bool Create(const AnyString& path, uint mode)
	{
		if (not path.empty() and not Yuni::IO::Exists(path))
		{
			# ifdef YUNI_OS_WINDOWS
			// `mode` is not used on Windows
			(void) mode;
			return WindowsMake(path);
			# else
			return UnixMake(path, mode);
			# endif
		}
		return true;
	}





} // namespace Directory
} // namespace IO
} // namespace Yuni

