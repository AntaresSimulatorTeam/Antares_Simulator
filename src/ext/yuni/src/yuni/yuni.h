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



#ifdef YUNI_CONFIG_H_LOCATION
#	include YUNI_CONFIG_H_LOCATION
#else
#	include "config.h"
#endif


/* Platform checks */
#include "platform.h"
/* Standard Preprocessor tools */
#include "core/preprocessor/std.h"
/* Operating System / Capabilities auto-detection */
#include "core/system/capabilities.h"
/* Standard Types */
#include "core/system/stdint.h"
/* ISO 646 - part of C90 standard but not by default on all compilers */
#include <iso646.h>


#ifdef __cplusplus /* Only with a C++ Compiler */

/*!
** \brief The Yuni Framework
*/
namespace Yuni
{}

/* nullptr */
#include "core/system/nullptr.h"
/* All standard forward declarations */
#include "core/fwd.h"

/* banned.h - SDL requirement */
#ifdef YUNI_OS_MSVC
//#	include "core/system/vs-banned.h"
#endif

#endif
