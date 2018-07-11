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
#include "../string.h"


namespace Yuni
{
namespace Process
{

	/*!
	** \brief Try to rename the process name
	**
	** This routine does nothing on Windows
	** This routine is an implementation of setproctitle
	*/
	YUNI_DECL char** Rename(int argc, char** argv, const AnyString& newprocessname);





} // namespace Process
} // namespace Yuni
