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
** \brief Standard types used by the Yuni Library
*/

#ifdef YUNI_HAS_STDINT_H
#	include <stdint.h>
#else
#	ifdef YUNI_OS_MSVC
#		include "windows/msinttypes/stdint.h"
#		include "windows/msinttypes/inttypes.h"
#	else
#		error "stdint.h is missing for this platform"
#	endif
#endif

#ifdef YUNI_OS_MSVC
# define YUNI_MSVC_SECURE_VSPRINTF
#endif

#ifdef YUNI_HAS_SYS_TYPES_H
# include <sys/types.h>
#endif




#ifdef __cplusplus /* Only with a C++ Compiler */

#ifndef YUNI_OS_CLANG
# include <cstddef>
#endif




namespace Yuni
{

	/* \todo Fix support for int128 on Visual Studio 10 */
	# if defined(YUNI_HAS_INT128_T) && !defined(__DISABLED_SUPPORT_FOR_INT128)
	/* 128-bit unsigned int */
	typedef unsigned __int128  uint128;
	typedef __int128           sint128;
	# endif

	/*! 64-bit unsigned int, guaranteed to be 8 bytes in size */
	typedef uint64_t       uint64;
	/*! 64-bit unsigned int, guaranteed to be 8 bytes in size */
	typedef int64_t        sint64;
	/*! 32-bit unsigned int, guaranteed to be 4 bytes in size */
	typedef uint32_t       uint32;
	/*! 32-bit unsigned int, guaranteed to be 4 bytes in size */
	typedef int32_t        sint32;
	/*! 16-bit unsigned int, guaranteed to be 2 bytes in size */
	typedef uint16_t       uint16;
	/*! 16-bit unsigned int, guaranteed to be 2 bytes in size */
	typedef int16_t        sint16;
	/*! 8-bit unsigned int, guaranteed to be 1 byte in size */
	typedef unsigned char  uint8;
	/*! 8-bit unsigned int, guaranteed to be 1 byte in size */
	typedef char           sint8;

	/*! unsigned char */
	typedef unsigned char  uchar;

	/*!
	** \brief Constant int64 value to indicate that the size must be autodetected by the location
	** of the terminating null character
	*/
	static const uint64 AutoDetectNullChar = static_cast<uint64>(-1);

	# ifndef YUNI_HAS_SSIZE_T
	/*! ssize_t */
	typedef Yuni::sint64 ssize_t;
	# endif


	/*! Decimal floating-point (32 bits) */
	typedef float   float32;
	/*! Decimal floating-point (32 bits) */
	typedef double  float64;


} /* namespace Yuni */





# else /* Actually we have a C Compiler */

# include "stddef.h"

# endif /* C++ Compiler */


/* C types */

/* \todo Fix support for int128 on Visual Studio 10 */
# if defined(YUNI_HAS_INT128_T) && !defined(__DISABLED_SUPPORT_FOR_INT128)
/*! 128-bit unsigned int */
typedef unsigned __int128  yuint128;
typedef __int128           yint128;
# endif

/*! 64-bit unsigned int, guaranteed to be 8 bytes in size */
typedef uint64_t       yuint64;
/*! 64-bit unsigned int, guaranteed to be 8 bytes in size */
typedef int64_t        yint64;
/*! 32-bit unsigned int, guaranteed to be 4 bytes in size */
typedef uint32_t       yuint32;
/*! 32-bit unsigned int, guaranteed to be 4 bytes in size */
typedef int32_t        yint32;
/*! 16-bit unsigned int, guaranteed to be 2 bytes in size */
typedef uint16_t       yuint16;
/*! 16-bit unsigned int, guaranteed to be 2 bytes in size */
typedef int16_t        yint16;
/*! 8-bit unsigned int, guaranteed to be 1 byte in size */
typedef unsigned char  yuint8;
/*! 8-bit unsigned int, guaranteed to be 1 byte in size */
typedef char           yint8;

/*! Decimal floating-point (32 bits) */
typedef float          yfloat32;
/*! Decimal floating-point (32 bits) */
typedef double         yfloat64;




/* Both C / C++ compiler */

# ifndef YUNI_HAS_UINT
/*! Convenient typedef around unsigned int */
typedef unsigned int  uint;
# endif


#ifdef YUNI_OS_CLANG
/* Try to fix issues with clang which does not yet support __float128, and produces
** compilation error in the STL */
typedef struct { double x, y; }  __float128;
#endif

