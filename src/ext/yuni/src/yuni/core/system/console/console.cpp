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
#include "console.h"
#ifndef YUNI_OS_WINDOWS
# include <unistd.h>
#endif



namespace Yuni
{
namespace System
{
namespace Console
{

	bool IsStdoutTTY()
	{
		# ifdef YUNI_OS_WINDOWS
		return false;
		# else
		return isatty(1);
		# endif
	}


	bool IsStderrTTY()
	{
		# ifdef YUNI_OS_WINDOWS
		return false;
		# else
		return isatty(2);
		# endif
	}




} // namespace Console
} // namespace System
} // namespace Yuni

#
