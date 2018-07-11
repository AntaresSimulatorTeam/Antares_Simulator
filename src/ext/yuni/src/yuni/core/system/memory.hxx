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
#include "memory.h"



namespace Yuni
{
namespace System
{
namespace Memory
{


	inline Usage::Usage()
	{
		update();
	}


	inline Usage::Usage(const Usage& copy)
		: available(copy.available), total(copy.total)
	{}



} // namespace Memory
} // namespace System
} // namespace Yuni
