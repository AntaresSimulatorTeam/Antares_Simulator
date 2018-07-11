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
#include "rwmutex.h"


namespace Yuni
{


	void RWMutex::writeLock()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		pMutex.lock();
		pSemaphore.acquire(pSemaphore.maxReaders());
		pMutex.unlock();
		# endif
	}


	void RWMutex::writeUnlock()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		pMutex.lock();
		pSemaphore.release(pSemaphore.maxReaders());
		pMutex.unlock();
		# endif
	}




} // namespace Yuni

