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
#include "method.h"


namespace Yuni
{
namespace Messaging
{
namespace API
{

	void Method::shrinkMemory()
	{
		pTmp.clear();
		pTmp.shrink();

		if (pBrief.empty() and pBrief.capacity() != 0)
			pBrief.shrink();
	}


	Method& Method::param(const AnyString& name, const AnyString& brief)
	{
		Parameter& param = pParams[(pTmp = name)];
		param.name = name;
		param.brief = brief;
		param.defvalue.clear();
		param.hasDefault = false;
		return *this;
	}


	Method& Method::param(const AnyString& name, const AnyString& brief, const AnyString& defvalue)
	{
		Parameter& param = pParams[(pTmp = name)];
		param.name = name;
		param.brief = brief;
		param.defvalue = defvalue;
		param.hasDefault = true;
		return *this;
	}




} // namespace API
} // namespace Messaging
} // namespace Yuni

