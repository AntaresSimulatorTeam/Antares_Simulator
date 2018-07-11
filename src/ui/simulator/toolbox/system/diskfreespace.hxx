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

#include <yuni/yuni.h>
#include <yuni/core/system/memory.h>
#include <yuni/core/system/windows.hdr.h>
#include <yuni/core/string/wstring.h>
#include <antares/logs.h>

using namespace Yuni;



template<class StringT>
static inline StringT&  BytesToStringW(StringT& out, uint64 size)
{
	if (0 == size)
		return out << L"0 byte";

	// bytes
	if (size < 1024)
		return out << size << L" bytes";
	// KiB
	if (size < 1024 * 1024)
		return out << (size / 1024) << L" KiB";
	// MiB
	if (size < 1024 * 1024 * 1024)
		return out << (size / (1024 * 1024)) << L" MiB";

	// GiB
	double s = size / (1024. * 1024 * 1024);
	return out << Math::Round(s, 1) << L" GiB";
}


static inline uint64  DiskFreeSpace(const AnyString& folder)
{
	if (folder.empty())
		return (yuint64) -1; // error

	# ifdef YUNI_OS_WINDOWS
	unsigned __int64 i64FreeBytesToCaller;
	//unsigned __int64 i64TotalBytes;
	//unsigned __int64 i64FreeBytes;

	{
		Yuni::WString wstr(folder);
		if (wstr.empty())
			return (yuint64) -1; // error

		if (GetDiskFreeSpaceExW(wstr.c_str(),
			(PULARGE_INTEGER)&i64FreeBytesToCaller, nullptr, //(PULARGE_INTEGER)&i64TotalBytes,
			nullptr/*(PULARGE_INTEGER)&i64FreeBytes*/) != 0)
		{
			// +1 to not be strictly equal to 0
			// It won't change anything anyway
			return i64FreeBytesToCaller;
		}
	}
	{
		// the previous call may abort due to the UNC path
		// retrying with the standard path
		Yuni::WString wstr(folder);
		if (wstr.empty())
			return (yuint64) -1; // error

		if (GetDiskFreeSpaceExW(wstr.c_str(),
			(PULARGE_INTEGER)&i64FreeBytesToCaller, nullptr, //(PULARGE_INTEGER)&i64TotalBytes,
			nullptr/*(PULARGE_INTEGER)&i64FreeBytes*/) != 0)
		{
			// +1 to not be strictly equal to 0
			// It won't change anything anyway
			return i64FreeBytesToCaller;
		}
		else
		{
			//Antares::logs.error() << "last error : " << GetLastError() << " : " << folder;
		}
	}
	# endif

	(void) folder;
	return (yuint64) -1; // obviously an error
}





