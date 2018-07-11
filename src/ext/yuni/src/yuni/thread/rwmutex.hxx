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
#include "rwmutex.h"



namespace Yuni
{

	inline RWMutex::RWMutex(uint maxReaders)
		# ifndef YUNI_NO_THREAD_SAFE
		: pSemaphore(maxReaders), pMutex()
		# endif
	{
	}


	inline RWMutex::RWMutex(const RWMutex& copy)
		: NonMovable()
		# ifndef YUNI_NO_THREAD_SAFE
		, pSemaphore(copy.pSemaphore), pMutex()
		# endif
	{
	}


	inline RWMutex& RWMutex::operator = (const RWMutex&)
	{
		// Does nothing on purpose
		return *this;
	}


	inline void RWMutex::readLock()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		pSemaphore.acquire();
		# endif
	}


	inline void RWMutex::readUnlock()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		pSemaphore.release();
		# endif
	}





	inline ReadMutexLocker::ReadMutexLocker(RWMutex& m) :
		pMutex(m)
	{
		m.readLock();
	}


	inline ReadMutexLocker::~ReadMutexLocker()
	{
		pMutex.readUnlock();
	}


	inline WriteMutexLocker::WriteMutexLocker(RWMutex& m) :
		pMutex(m)
	{
		m.writeLock();
	}


	inline WriteMutexLocker::~WriteMutexLocker()
	{
		pMutex.writeUnlock();
	}




} // namespace Yuni
