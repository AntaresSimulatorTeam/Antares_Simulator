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

#include "io.h"

using namespace Yuni;


/*extern*/ bool dry = true;

/*extern*/ Atomic::Int<> IOBytesDeleted   = 0;
/*extern*/ Atomic::Int<> IOFilesDeleted   = 0;
/*extern*/ Atomic::Int<> IOFoldersDeleted = 0;

/*extern*/ std::unordered_set<String> inputFolders;




bool RemoveFile(const String& filename, uint64 size)
{
	if (dry || IO::errNone == IO::File::Delete(filename))
	{
		++IOFilesDeleted;
		IOBytesDeleted += (Atomic::Int<>::Type) size;
		return true;
	}
	return false;
}


bool RemoveDirectoryIfEmpty(const String& folder)
{
	if (inputFolders.count(folder) == 0) // input folders must not be removed
	{
		if (dry)
		{
			++IOFoldersDeleted;
			return true;
		}
		else
		{
			# ifdef YUNI_OS_WINDOWS
			Yuni::WString wpath(folder);
			if (0 != RemoveDirectoryW(wpath.c_str()))
			# else
			if (0 == ::rmdir(folder.c_str()))
			# endif
			{
				++IOFoldersDeleted;
				return true;
			}
		}
	}
	return false;
}

