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
#include "mutex.h"



namespace Yuni
{

	inline void Mutex::lock()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		EnterCriticalSection(&pSection);
		# else
		::pthread_mutex_lock(&pLock);
		# endif
		# endif
	}


	inline bool Mutex::trylock()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		return (0 != TryEnterCriticalSection(&pSection));
		# else
		return (0 == ::pthread_mutex_trylock(&pLock));
		# endif
		# else
		return false;
		# endif
	}


	inline void Mutex::unlock()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		LeaveCriticalSection(&pSection);
		# else
		::pthread_mutex_unlock(&pLock);
		# endif
		# endif
	}


	# ifndef YUNI_NO_THREAD_SAFE
	# ifndef YUNI_OS_WINDOWS
	inline pthread_mutex_t& Mutex::pthreadMutex()
	{
		return pLock;
	}

	inline const pthread_mutex_t& Mutex::pthreadMutex() const
	{
		return pLock;
	}
	# endif
	# endif



	inline MutexLocker::MutexLocker(Mutex& m) :
		pMutex(m)
	{
		m.lock();
	}


	inline MutexLocker::~MutexLocker()
	{
		pMutex.unlock();
	}




} // namespace Yuni
