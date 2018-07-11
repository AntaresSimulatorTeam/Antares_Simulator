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
#include "schema.h"


namespace Yuni
{
namespace Messaging
{

	Schema::Defaults& Schema::Defaults::param(const AnyString& name, const AnyString& brief)
	{
		API::Method::Parameter& param = pParams[(pTmp = name)];
		param.name = name;
		param.brief = brief;
		param.defvalue.clear();
		param.hasDefault = false;
		return *this;
	}


	Schema::Defaults& Schema::Defaults::param(const AnyString& name, const AnyString& brief, const AnyString& defvalue)
	{
		API::Method::Parameter& param = pParams[(pTmp = name)];
		param.name = name;
		param.brief = brief;
		param.defvalue = defvalue;
		param.hasDefault = true;
		return *this;
	}






} // namespace Messaging
} // namespace Yuni

