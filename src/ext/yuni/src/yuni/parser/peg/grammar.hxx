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
#include "grammar.h"


namespace Yuni
{
namespace Parser
{
namespace PEG
{


	inline bool Grammar::loadFromString(const AnyString& content)
	{
		return loadFromData(content, "<memory>");
	}




} // namespace PEG
} // namespace Parser
} // namespace Yuni
