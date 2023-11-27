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

#include <stdint.h>

#ifdef YUNI_OS_MSVC
#define YUNI_MSVC_SECURE_VSPRINTF
#endif

#ifdef YUNI_HAS_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef __cplusplus /* Only with a C++ Compiler */

#ifndef YUNI_OS_CLANG
#include <cstddef>
#endif

namespace Yuni
{
/*! unsigned char */
typedef unsigned char uchar;

/*!
** \brief Constant int64 value to indicate that the size must be autodetected by the location
** of the terminating null character
*/
static const uint64_t AutoDetectNullChar = static_cast<uint64_t>(-1);

#ifndef YUNI_HAS_SSIZE_T
/*! ssize_t */
typedef int64_t ssize_t;
#endif
} /* namespace Yuni */

#else /* Actually we have a C Compiler */

#include "stddef.h"

#endif /* C++ Compiler */

/* Both C / C++ compiler */

#ifndef YUNI_HAS_UINT
/*! Convenient typedef around unsigned int */
typedef unsigned int uint;
#endif
