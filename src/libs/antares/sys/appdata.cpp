/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "appdata.h"
#ifdef YUNI_OS_WINDOWS
# include <yuni/core/system/windows.hdr.h>
# include <Shlobj.h>
#endif
#include <yuni/core/system/environment.h>

#include <ostream>

using namespace Yuni;


namespace OperatingSystem
{

	# ifdef YUNI_OS_WINDOWS
	static inline bool ConvertWideCharIntoString(String& out, const wchar_t* wpath)
	{
		int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wpath, -1, nullptr, 0,  nullptr, nullptr);
		if (sizeRequired <= 0)
			return false;

		out.reserve((uint) sizeRequired + 1);
		WideCharToMultiByte(CP_UTF8, 0, wpath, -1, (char*)out.data(), sizeRequired,  nullptr, nullptr);
		out.resize(((uint) sizeRequired) - 1);
		return true;
	}
	# endif





	bool FindLocalAppData(Yuni::String& out, bool allusers)
	{
		out.clear();

		# ifdef YUNI_OS_WINDOWS
		typedef HRESULT WINAPI SHGetFolderPathWFn(HWND hwndOwner, int nFolder, HANDLE hToken,
			DWORD dwFlags, __out  LPTSTR pszPath);

		HMODULE hShell32 = GetModuleHandleW(L"shell32.dll");
		if (hShell32)
		{
			// Windows XP
			SHGetFolderPathWFn* folderPathW = (SHGetFolderPathWFn*)(GetProcAddress(hShell32, "SHGetFolderPathW"));
			if (folderPathW)
			{
				wchar_t* wpath = new wchar_t[MAX_PATH + 1];
				if (!wpath)
					return false;
				folderPathW(0, (allusers ? CSIDL_COMMON_APPDATA : CSIDL_LOCAL_APPDATA), nullptr, 0, wpath);
				if (ConvertWideCharIntoString(out, wpath))
				{
					delete[] wpath;
					return true;
				}
				delete[] wpath;
			}
		}
		# else

		if (allusers)
		{
			out = "/etc";
			return true;
		}
		else
		{
			System::Environment::Read("HOME", out);
			if (not out.empty())
				return true;
		}
		# endif
		return false;
	}


	bool FindAntaresLocalAppData(Yuni::String& out, bool allusers)
	{
		if (not FindLocalAppData(out, allusers))
			return false;

		# ifdef YUNI_OS_WINDOWS
			out += "\\RTE\\antares";
		# else
		if (allusers)
			out += "/antares";
		else
		{
			// ~/.antares is already taken by wx - recent files
			out += "/.antares-data"; // hidden from home
		}
		# endif

		return true;
	}



} // namespace OperatingSystem

