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
#include "../../yuni.h"


# ifdef YUNI_OS_WINDOWS
#   include "windows.hdr.h"
#	include <shellapi.h>


#	define YUNI_MAIN() \
	int main(int argc, char** argv)

#   define YUNI_MAIN_CONSOLE(argc, argv)  \
	/* Forward declaration */ \
	int WINAPI main(int argc, char** argv); \
	\
	int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) \
	{ \
		int argc; \
		char** argv; \
		\
		/* convert the command line arguments */ \
		LPWSTR* arglist = CommandLineToArgvW(GetCommandLineW(), &argc); \
		if (nullptr == arglist) \
			return EXIT_FAILURE; \
		\
		argv = (char**) malloc(sizeof(char*) * (argc + 1)); \
		argv[argc] = nullptr; \
		\
		for (int i = 0; i < argc; ++i) \
		{ \
			if (arglist[i] && *(arglist[i]) != L'\0') \
			{ \
				int length = WideCharToMultiByte(CP_UTF8, 0, arglist[i], -1, NULL, 0, NULL, NULL); \
				if (length <= 0) \
				{ \
					LocalFree(arglist); \
					return EXIT_FAILURE; \
				} \
				\
				argv[i] = (char*) malloc(sizeof(char) * (length + 1)); \
				WideCharToMultiByte(CP_UTF8, 0, arglist[i], -1, argv[i], length, NULL, NULL); \
				argv[i][length] = '\0'; \
			} \
			else \
			{ \
				argv[i] = (char*) malloc(sizeof(char) * 1); \
				argv[i][0] = '\0'; \
			} \
		} \
		\
		LocalFree(arglist); \
		\
		\
		int exitcode = main(argc, argv); \
		\
		/* release memory*/ \
		for (int i = 0; i < argc; ++i) \
			free(argv[i]); \
		free(argv); \
		\
		return exitcode; \
	} \
	\
	\
	int WINAPI main(int argc, char* argv[])


#else

#   define YUNI_MAIN_CONSOLE(argc, argv) \
		int main(int argc, char* argv[])

#   define YUNI_MAIN() \
		int main(int argc, char* argv[])

#endif

