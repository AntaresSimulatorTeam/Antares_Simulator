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
#include "../io.h"
#include "../directory.h"
#include "../../core/system/windows.hdr.h"
#ifndef YUNI_OS_WINDOWS
# ifdef YUNI_HAS_STDLIB_H
#	include <stdlib.h>
# endif
# include <unistd.h>
#else
# include <direct.h>
#endif
#include <sys/stat.h>
#include <fstream>
#include <errno.h>
#ifndef YUNI_OS_MSVC
# include <dirent.h>
#endif
#include <fcntl.h>


#ifndef S_ISDIR
# define S_ISDIR(mode) ( (mode & S_IFMT) == S_IFDIR)
#endif
