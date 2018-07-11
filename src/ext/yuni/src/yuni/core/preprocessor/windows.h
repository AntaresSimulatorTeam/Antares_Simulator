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


/* Microsoft Windows */
#ifndef YUNI_OS_WINDOWS
#	define YUNI_OS_WINDOWS
#endif // YUNI_OS_WINDOWS
#define YUNI_OS_NAME "Microsoft Windows"

#undef  YUNI_OS_FLAG_WINDOWS
#define YUNI_OS_FLAG_WINDOWS 1


/* Windows 64 */
#ifdef _WIN64
#	define YUNI_OS_WIN64
#	define YUNI_OS_64
#else
#	define YUNI_OS_WIN32
#endif

/* Windows CE */
#ifdef _WIN32_WCE
#	define YUNI_OS_WINCE
#endif

/* Visual Studio */
#ifdef _MSC_VER
#	define YUNI_OS_MSVC
#	define YUNI_MSVC
#	define YUNI_COMPILER_NAME "Microsoft Visual C++"
#endif

/* Borland Compiler */
#ifdef __BORLANDC__
#	define YUNI_OS_BORLAND
#	define YUNI_COMPILER_NAME "Borland C++ Compiler"
#endif

/* MinGW */
#if defined(__MINGW32__) || defined(__MINGW64__)
#	define YUNI_OS_MINGW
#	undef YUNI_OS_32
#	undef YUNI_OS_64
#	ifdef __MINGW64__
#		define YUNI_OS_MINGW64
#		define YUNI_OS_64
#		define YUNI_COMPILER_NAME "MinGW64"
#	else
#		define YUNI_OS_MINGW32
#		define YUNI_OS_32
#		define YUNI_COMPILER_NAME "MinGW32"
#	endif
#endif
