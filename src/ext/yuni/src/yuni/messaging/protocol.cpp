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
#include "protocol.h"


namespace Yuni
{
namespace Messaging
{

	void Protocol::shrinkMemory()
	{
		// ask to reduce the memory consumption to all schemas
		Schema::Hash::iterator end = pSchemas.end();
		for (Schema::Hash::iterator i = pSchemas.begin(); i != end; ++i)
			i->second.shrinkMemory();

		// release
		pTmp.clear();
		pTmp.shrink();
	}




} // namespace Messaging
} // namespace Yuni


