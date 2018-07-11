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
#include "../yuni.h"
#include "../core/string.h"


namespace Yuni
{
namespace DBI
{

	/*!
	** \brief Check whether a string is an identifier or not
	** \ingroup DBI
	*/
	bool IsValidIdentifier(AnyString text);




} // namespace DBI
} // namespace Yuni
