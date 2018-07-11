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
#include "loop.h"
#include "queueservice.h"

namespace Yuni
{
namespace Media
{

	Loop::Loop(QueueService* queueservice)
		: pMediaService(queueservice)
		, pClosing(false)
	{
		timeout(50);
	}


	void Loop::beginClose()
	{
		pClosing = true;
	}


	void Loop::endClose()
	{
		stop();
	}


	bool Loop::onLoop()
	{
		if (not pMediaService or pClosing)
			return false;

		pMediaService->updateDispatched();
		return true;
	}






} // namespace Media
} // namespace Yuni
