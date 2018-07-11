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
#include "thread.h"


namespace Yuni
{
namespace Thread
{


	inline bool IThread::started() const
	{
		return pStarted;
	}


	inline bool IThread::shouldAbort()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		Yuni::MutexLocker locker(pInnerFlagMutex);
		return (pShouldStop || not pStarted);
		# else
		return false;
		# endif
	}


	inline bool IThread::operator ! () const
	{
		return not started();
	}




} // namespace Thread
} // namespace Yuni

