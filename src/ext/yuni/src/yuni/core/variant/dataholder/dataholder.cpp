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
#include "../variant.h"


namespace Yuni
{
namespace Private
{
namespace Variant
{


	IDataHolder* IDataHolder::invoke(const String& /*name*/)
	{
		return nullptr;
	}


	IDataHolder* IDataHolder::invoke(const String& /*name*/, IDataHolder* /*a1*/)
	{
		return nullptr;
	}


	IDataHolder* IDataHolder::invoke(const String& /*name*/, IDataHolder* /*a1*/, IDataHolder* /*a2*/)
	{
		return nullptr;
	}


	IDataHolder* IDataHolder::invoke(const String& /*name*/, IDataHolder* /*a1*/, IDataHolder* /*a2*/, IDataHolder* /*a3*/)
	{
		return nullptr;
	}


	IDataHolder* IDataHolder::invoke(const String& /*name*/, IDataHolder* /*a1*/, IDataHolder* /*a2*/, IDataHolder* /*a3*/, IDataHolder* /*a4*/)
	{
		return nullptr;
	}




} // namespace Variant
} // namespace Private
} // namespace Yuni

