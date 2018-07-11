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
#ifndef YUNI_NO_THREAD_SAFE
#	define YUNI_NO_THREAD_SAFE // disabling thread-safety
#endif
#include <yuni/yuni.h>
#include "program.h"




int main(int argc, char* argv[])
{
	if (argc > 1)
	{
		Yuni::LibConfigProgram libconfig;
		return libconfig.execute(argc, argv);
	}
	return 0;
}
