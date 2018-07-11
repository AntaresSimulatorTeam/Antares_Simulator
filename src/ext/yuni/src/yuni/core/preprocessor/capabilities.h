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
/* !!! "C compatibility" header !!! */


/*!
** \brief Operating System / Capabilities Auto-Detection
**
** General defines :
** YUNI_OS_NAME: Name of the operating system (without its version which can only be
**   determined at runtime)
** YUNI_COMPILER_NAME: Name of the compiler currently used
**
** Usefull defines :
**
** - Microsoft Windows
** YUNI_OS_WINDOWS: The Microsoft Windows Operating system (32/64Bits)
** YUNI_OS_WIN32: Microsoft Windows 32Bits
** YUNI_OS_WIN64: Microsoft Windows 64Bits
** YUNI_OS_WINCE: Microsoft Windows CE
** YUNI_OS_MSVC: The compiler is Microsoft Visual Studio
**
** - Unixes
** YUNI_OS_UNIX
** - Linux
** YUNI_OS_LINUX
**
** - MacOS
** YUNI_OS_MACOS or YUNI_OS_MAC or YUNI_OS_DARWIN
**
** - FreeBSD
** YUNI_OS_FREEBSD (The value is the version)
**
** - AIX
** YUNI_OS_AIX
**
** - HPUX
** YUNI_OS_HPUX
**
** - BEOS
** YUNI_OS_BEOS
**
** - DragonFLY
** YUNI_OS_DRAGONFLY
**
** - LynxOS
** YUNI_OS_LYNX
**
** - NetBSD
** YUNI_OS_NETBSD
**
** - OpenBSD
** YUNI_OS_OPENBSD
**
** - Solaris/SunOS
** YUNI_OS_SOLARIS
** YUNI_OS_SUNOS
**
** - VMS
** YUNI_OS_VMS
**
** - Unknown
** YUNI_OS_UNKNOWN
** YUNI_OS_COMPILER
**
** Window systems
** - X Window System
** YUNI_WINDOWSYSTEM_X11
** - MFC (Microsoft Foundation Class) or any Microsoft Windows Window System
** YUNI_WINDOWSYSTEM_MSW
** - Mac OS X Cocoa
** YUNI_WINDOWSYSTEM_COCOA
**
** Misc:
**
** - Borland C++ : YUNI_OS_BORLAND
** - MinGW: YUNI_OS_MINGW
** - CLang: YUNI_OS_CLANG
** - LLVM: YUNI_OS_LLVM
** - Cygwin : YUNI_OS_CYGWIN
** - Gcc/G++ : YUNI_OS_GCC, YUNI_OS_GCC_VERSION : if YUNI_OS_GCC_VERSION > 30200  (> 3.2.0)
** - Intel Compiler: YUNI_OS_INTELCOMPILER
** - Sun Studio: YUNI_OS_SUNSTUDIO
** - Comeau C++: YUNI_OS_COMO
** - Cray C compiler: YUNI_OS_CRAYC
**
** 32/64 Bits
** YUNI_OS_32 or YUNI_OS_64
**
** C++0X
** YUNI_CPP_0X
**
** All those informations can be found at http://predef.sourceforge.net/
*/

#include <assert.h>

#define YUNI_OS_FLAG_WINDOWS  0
#define YUNI_OS_FLAG_UNIX     0
#define YUNI_OS_FLAG_LINUX    0
#define YUNI_OS_FLAG_MACOS    0


#if defined(__TOS_WIN__) || defined(__WIN32__) || defined(_WIN64) || defined(_WIN32) || defined(YUNI_OS_WINDOWS)
#	include "windows.h"
#else
#	include "unixes.h"
#endif



/* Window System */
#ifdef YUNI_OS_WINDOWS
#	define YUNI_WINDOWSYSTEM_MSW
#else
#	ifdef YUNI_OS_MAC
#		define YUNI_WINDOWSYSTEM_COCOA
#	else
#		define YUNI_WINDOWSYSTEM_X11
#	endif
#endif

/* LLVM */
#ifdef __llvm__
#	define YUNI_OS_LLVM
#endif

#ifdef __clang__
#	define YUNI_OS_CLANG
#	define YUNI_COMPILER_NAME "Compatible Clang Compiler"
#	define YUNI_OS_CLANG_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__) // compatible mode
#else
/* GNU C and C++ compiler */
#	ifdef __GNUC__
#		define YUNI_OS_GCC
#		define YUNI_OS_GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#		ifndef YUNI_COMPILER_NAME
#			define YUNI_COMPILER_NAME "GCC (GNU Compiler Collection)"
#		endif
#	else
#		define YUNI_OS_GCC_VERSION 0
#	endif
#endif


/* Intel Compiler */
#if defined(__INTEL_COMPILER) || defined(__ICC)
#	define YUNI_OS_INTELCOMPILER
#	ifdef YUNI_COMPILER_NAME /* in some cases, the compiler may already have been detected as Visual Studio */
#		undef YUNI_COMPILER_NAME
#	endif
#	define YUNI_COMPILER_NAME "ICC (Intel C++ Compiler)"
#endif

/* Como Compiler */
#ifdef __COMO__
#	define YUNI_OS_COMO
#	ifdef YUNI_COMPILER_NAME
#		undef YUNI_COMPILER_NAME
#	endif
#	define YUNI_COMPILER_NAME "Comeau C++"
#endif

/* CrayC Compiler */
#ifdef __CRAYC__
#	define YUNI_OS_CRAYC
#	ifdef YUNI_COMPILER_NAME
#		undef YUNI_COMPILER_NAME
#	endif
#	define YUNI_COMPILER_NAME "Cray C/C++"
#endif



/* 32/64 Bits modes */
#if !defined(YUNI_OS_32) && !defined(YUNI_OS_64)
#	if defined(__IA64__) || defined(_IA64) || defined(__amd64__) || defined(__x86_64__) || defined(_M_IA64) || defined(_WIN64) || defined(__MINGW64__)
#		 define YUNI_OS_64
#	else
#		 define YUNI_OS_32
#	endif
#endif






/*!
** \macro YUNI_ALWAYS_INLINE
** \brief Force inline
**
** \code
** YUNI_ALWAYS_INLINE static void max(int x, int y)
** {
**	return x > y ? x : y;
** }
** \endcode
*/
#if !defined(YUNI_ALWAYS_INLINE) && defined(YUNI_HAS_GCC_ALWAYS_INLINE)
#	define YUNI_ALWAYS_INLINE  __attribute__((always_inline))
#endif
#if !defined(YUNI_ALWAYS_INLINE) && defined(YUNI_HAS_MSVC_FORCE_INLINE)
#	define YUNI_ALWAYS_INLINE  __forceinline
#endif
#if !defined(YUNI_ALWAYS_INLINE)
#	define YUNI_ALWAYS_INLINE  inline
#endif


/*!
** \macro YUNI_ATTR_PURE
** \brief Declare a "pure" function
**
** A "pure" function is one that has no effects, and whose return value reflects
** only the function's parameters or nonvolatile global variables. Any parameter
** or global variable access must be read-only. Loop optimization and subexpression
** elimination can be applied to such functions.
**
** \code
** static void max(int x, int y) YUNI_ATTR_PURE
** {
**	return x > y ? x : y;
** }
** \endcode
*/
#if !defined(YUNI_ATTR_PURE) && defined(YUNI_HAS_GCC_ATTR_PURE)
#	define YUNI_ATTR_PURE  __attribute__((pure))
#endif
#if !defined(YUNI_ATTR_PURE)
#	define YUNI_ATTR_PURE
#endif


/*!
** \macro YUNI_ATTR_CONST
** \brief Declare a "const" function
**
** A "constant" function is a stricter variant of a pure function. Such functions
** cannot access global variables, and cannot take pointers as parameters.
** Thus, the constant function's return value reflects nothing but the
** passed-by-value parameters. Additional optimizations, on top of those possible
** with pure functions, are possible for such functions
**
** \code
** static void max(int x, int y) YUNI_ATTR_CONST
** {
**	return x > y ? x : y;
** }
** \endcode
*/
#if !defined(YUNI_ATTR_CONST) && defined(YUNI_HAS_GCC_ATTR_CONST)
#	define YUNI_ATTR_CONST  __attribute__((const))
#endif
#if !defined(YUNI_ATTR_CONST)
#	define YUNI_ATTR_CONST
#endif









#ifndef YUNI_HAS_CPP_KEYWORD_OVERRIDE
#	define override
#endif

#ifndef YUNI_HAS_CPP_KEYWORD_FINAL
#	define final
#endif




/*!
** \def YUNI_DEPRECATED
** \brief Deprecated
*/
#if defined(YUNI_OS_GCC)
#	if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#		define YUNI_DEPRECATED(text, func...)  func __attribute__((__deprecated__))
#	endif
#endif
#if defined(_MSC_VER) && (_MSC_VER >= 1300)
#	if (_MSC_FULL_VER >= 140050320)
#		define YUNI_DEPRECATED(text, func, ...) __declspec(deprecated(text)) func
#	else
#		define YUNI_DEPRECATED(text, func, ...) __declspec(deprecated) func
#	endif
#endif
#ifndef YUNI_DEPRECATED
#	define YUNI_DEPRECATED(text, func...) func
#endif

/*!
** \def Noreturn
**
** \code
** void foo() YUNI_NORETURN
** {
** }
** \endcode
*/
#if defined(YUNI_OS_GCC)
#	if (__GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ >= 1))
#		define YUNI_NORETURN  __attribute__((noreturn))
#	endif
#endif
#ifndef YUNI_NORETURN
#	define YUNI_NORETURN
#endif




/* C++0x */
#if defined(__GXX_EXPERIMENTAL_CPP0X__) || defined(_RWSTD_EXT_CXX_0X)
#	define YUNI_CPP_0X
#endif



/* OS Detection */
#ifndef YUNI_OS_NAME
#	define YUNI_OS_NAME "Unknown"
#	define YUNI_OS_UNKNOWN
#	warning "OS Detection: Unable to guess the operating system"
#endif

/* Compiler Detection */
#ifndef YUNI_COMPILER_NAME
#	define YUNI_COMPILER_UNKNOWN
#	define YUNI_COMPILER_NAME "Unknown"
#	warning "Compiler Detection: Unable to guess the compiler"
#endif



/*!
** \brief Thread-local variable
*/
/* Usage Example :
** \code
** YUNI_THREAD_LOCAL_STORAGE(int)  myThreadLocalVariable = 0;
** \endcode
**
** \warning The compiler directive is not supported on OS X
**
** \see Rules and Limitations for TLS on Windows http://msdn.microsoft.com/en-us/library/2s9wt68x.aspx
** \see http://gcc.gnu.org/onlinedocs/gcc-4.2.4/gcc/Thread_002dLocal.html
** \see http://msdn.microsoft.com/en-us/library/6e298fy4(VS.71).aspx
*/
#if defined(YUNI_OS_BORLAND)
#	define YUNI_THREAD_LOCAL_STORAGE(type)  type __thread
#else
#	if defined(YUNI_OS_WINDOWS) && (defined(YUNI_OS_MSVC) || defined(__INTEL_COMPILER))
#		define YUNI_THREAD_LOCAL_STORAGE(type)  __declspec(thread) type
#	else
#		define YUNI_THREAD_LOCAL_STORAGE(type)  __thread type
#	endif
#endif



/*!
** \def YUNI_VA_COPY
** \brief Copy one variable argument list into another
**
** You should use va_end to release the new list
*/
#ifdef YUNI_HAS_VA_COPY
#	define YUNI_VA_COPY(dst, src)  va_copy((dst), (src))
#else
#	if (defined(__GNUC__) && defined(__PPC__) && (defined(_CALL_SYSV) || defined(__WIN32__))) || defined(__WATCOMC__)
#		define YUNI_VA_COPY(dst, src)	  (*(dst) = *(src))
#	elif defined(YUNI_VA_COPY_AS_ARRAY)
#		define YUNI_VA_COPY(dst, src)	  memmove((dst), (src), sizeof (va_list))
#	else /* va_list is a pointer */
#		define YUNI_VA_COPY(dst, src)	  ((dst) = (src))
#	endif
#endif


/* Force the define YUNI_DYNAMIC_LIBRARY (Visual Studio) */
#if !defined(YUNI_DYNAMIC_LIBRARY) && defined(_WINDLL)
#	define YUNI_DYNAMIC_LIBRARY
#	define YUNI_DYNAMIC_LIBRARY_EXPORT
#endif

#ifdef YUNI_DYNAMIC_LIBRARY
#	ifdef YUNI_OS_MSVC
#		ifdef YUNI_DYNAMIC_LIBRARY_EXPORT
#			define YUNI_DECL             __declspec(dllexport)
#			define YUNI_EXPIMP_TEMPLATE
#		else
#			define YUNI_DECL             __declspec(dllimport)
#			define YUNI_EXPIMP_TEMPLATE  extern
#		endif
#	else
#		ifdef YUNI_OS_GCC
#			define YUNI_DECL             __attribute__((__visibility__("default")))
#			define YUNI_EXPIMP_TEMPLATE  extern
#		endif
#	endif
#endif
/* Fallback to empty */
#ifndef YUNI_DECL
#	define YUNI_DECL
#endif
#ifndef YUNI_EXPIMP_TEMPLATE
#	define YUNI_EXPIMP_TEMPLATE
#endif



/* In some cases, the macro 'unix' and 'linux' can be defined */
#ifdef unix
#	undef unix
#endif
#ifdef linux
#	undef linux
#endif


#ifndef YUNI_OS_GCC_VERSION
#	define YUNI_OS_GCC_VERSION 0
#endif



#ifdef YUNI_OS_MSVC
// Annoying warning from Visual Studio :
// decorated name length exceeded, name was truncated (when using a lot of templates)
#	pragma warning( disable : 4503)
#endif




/* Memcpy */
#ifndef NDEBUG
#	ifdef YUNI_OS_MSVC
#		define YUNI_MEMCPY(dst, dstsize, source, count)   do \
		{ \
			assert(dstsize >= count && "memcpy: destination buffer too small"); \
			memcpy_s(dst, static_cast<size_t>(dstsize), source, static_cast<size_t>(count)); \
		} while (0)
#	 else
#		define YUNI_MEMCPY(dst, dstsize, source, count)   do \
		{ \
			assert(dstsize >= count && "memcpy: destination buffer too small"); \
			memcpy(dst, source, static_cast<size_t>(count)); \
		} while(0)
#	endif
#else
#	ifdef YUNI_OS_MSVC
#		define YUNI_MEMCPY(dst, dstsize, source, count) \
		(void) memcpy_s(dst, static_cast<size_t>(dstsize), source, static_cast<size_t>(count))
#	 else
#		define YUNI_MEMCPY(dst, dstsize, source, count) \
		(void) memcpy(dst, source, static_cast<size_t>(count))
#	endif
#endif



/*!
** \brief likely / unlikely
**
** \code
** if (YUNI_LIKELY(x == 42))
**     does something
** else
**     does something else
** \endcode
*/
#if defined(YUNI_OS_GCC) || defined(YUNI_HAS_GCC_BUILTIN_EXPECT)
#	define YUNI_LIKELY(x)     __builtin_expect(!!(x), 1)
#	define YUNI_UNLIKELY(x)   __builtin_expect(!!(x), 0)
#else
#	define YUNI_LIKELY(x)     (x)
#	define YUNI_UNLIKELY(x)   (x)
#endif





#ifdef __cplusplus /* Only with a C++ Compiler */

namespace Yuni
{
namespace System
{

	/* Operating systems */
	enum
	{
		/*! Flag to indicate if the current operating system is Microsoft Windows */
		windows = YUNI_OS_FLAG_WINDOWS,
		/*! Flag to indicate if the current operating system is Unix based */
		unix    = YUNI_OS_FLAG_UNIX,
		/*! Flag to indicate if the current operating system is Linux based */
		linux   = YUNI_OS_FLAG_LINUX,
		/*! Flag to indicate if the current operating system is Mac OS */
		macos   = YUNI_OS_FLAG_MACOS,
	};



} /* namespace System */
} /* namespace Yuni */


namespace Yuni
{

	#ifdef YUNI_HAS_CONSTEXPR
		#ifndef NDEBUG
		constexpr static const bool debugmode = true;
		#else
		constexpr static const bool debugmode = false;
		#endif
	#else
	enum
	{
		#ifndef NDEBUG
		debugmode = 1,
		#else
		debugmode = 0,
		#endif
	};
	#endif

} /* namespace Yuni */


//# ifndef YUNI_HAS_CONSTEXPR
// There are some issues with Visual Studio 11
// The feature is disabled, waiting for a valid fix
//#	ifndef constexpr
//#		define constexpr  /* does nothing, but compiles */
//#	endif
//# endif

#endif /* __cplusplus */
