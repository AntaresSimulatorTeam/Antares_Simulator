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

#ifdef YUNI_OS_WINDOWS
# include <string.h>
# ifndef WIN32_LEAN_AND_MEAN
#	define WIN32_LEAN_AND_MEAN 1
# endif
# include <windows.h>
# include <shellapi.h>
# include <stdlib.h>
# include <stdio.h>
#endif


# ifdef YUNI_OS_WINDOWS
char** AntaresGetUTF8Arguments(int argc, char**)
{
	wchar_t** wargv = CommandLineToArgvW(GetCommandLineW(), &argc);
	char** argvUTF8 = (char**)malloc(argc * sizeof(char*));
	for (int i = 0; i != argc; ++i)
	{
		const uint len = (uint) wcslen(wargv[i]);
		const uint newLen = WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, NULL, 0, NULL, NULL);
		argvUTF8[i] = (char*)malloc((newLen + 1) * sizeof(char));
		memset(argvUTF8[i], 0, (newLen + 1) * sizeof(char));
		WideCharToMultiByte(CP_UTF8, 0, wargv[i], len, argvUTF8[i], newLen, NULL, NULL);
		argvUTF8[i][newLen] = '\0';
	}
	return argvUTF8;
}

void FreeUTF8Arguments(int argc, char** argv)
{
	for (int i = 0; i != argc; ++i)
		free(argv[i]);
	free(argv);
}

# else

# define AntaresGetUTF8Arguments(ARGC,ARGV) ARGV

# define FreeUTF8Arguments(ARGC,ARGV)

# endif

