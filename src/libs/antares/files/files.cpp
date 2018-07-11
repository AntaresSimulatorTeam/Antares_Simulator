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
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include "files.h"

#include <yuni/core/system/windows.hdr.h>
#ifndef YUNI_OS_MSVC
# include <unistd.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>


using namespace Yuni;



FILE* FileOpen(const char* filename, const char* mode)
{
	# ifdef YUNI_OS_WINDOWS
	wchar_t m[4];

	wchar_t wb[FILENAME_MAX];
	wb[0] = L'\\';
	wb[1] = L'\\';
	wb[2] = L'?';
	wb[3] = L'\\';
	int n = MultiByteToWideChar(CP_UTF8, 0, filename, -1, wb + 4, sizeof(wb) - 10);
	if (n <= 0)
		return NULL;
	wb[n + 4] = L'\0';
	MultiByteToWideChar(CP_UTF8, 0, mode, -1, m, sizeof(m));
	FILE* fd;
	return !_wfopen_s(&fd, wb, m) ? fd : NULL;
	# else
	return fopen(filename, mode);
	# endif
}

