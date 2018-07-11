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
#ifndef __YUNI_MESSAGING_PROTOCOL_HXX__
# define __YUNI_MESSAGING_PROTOCOL_HXX__


namespace Yuni
{
namespace Messaging
{

	inline Schema& Protocol::schema()
	{
		pTmp.clear();
		return pSchemas[pTmp];
	}


	inline Schema& Protocol::schema(const AnyString& name)
	{
		return pSchemas[(pTmp = name)];
	}


	inline const Schema::Hash& Protocol::allSchemas() const
	{
		return pSchemas;
	}




} // namespace Messaging
} // namespace Yuni

#endif // __YUNI_MESSAGING_PROTOCOL_HXX__
