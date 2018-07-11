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
#include "mutex.h"
#include <cassert>
#include <iostream>
#include <cerrno>

#include "../core/atomic/int.h"
#if YUNI_ATOMIC_MUST_USE_MUTEX != 0
#warning Atomic types must ue mutex. the implementation should be checked YUNI_OS_GCC_VERSION
#endif



namespace Yuni
{

	enum // anonymous
	{
		/*!
		** \brief The spin count for the critical section object
		**
		** On single-processor systems, the spin count is ignored and the critical section
		** spin count is set to 0 (zero). On multiprocessor systems, if the critical section
		** is unavailable, the calling thread spinsdwSpinCount times before performing a
		** wait operation on a semaphore associated with the critical section. If the critical
		** section becomes free during the spin operation, the calling thread avoids the
		** wait operation.
		** \see http://msdn.microsoft.com/en-us/library/ms683476%28v=vs.85%29.aspx
		*/
		spinCount = 3000,
	};


	inline void Mutex::destroy()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		DeleteCriticalSection(&pSection);
		# else
		switch (::pthread_mutex_destroy(&pLock))
		{
			case 0: // Ok good
			{
				break;
			}
			// If an error happens, we will let the program continue but
			// it can becaome ugly around here...
			case EBUSY:
			{
				std::cerr << "\nattempt to destroy a mutex while it is locked or referenced\n";
				assert(false and "attempt to destroy a mutex while it is locked or referenced");
				break;
			}
			default:
			{
				std::cerr << "\nfailed to destroy a mutex\n";
				assert(false and "\nfailed to destroy a mutex\n");
				break;
			}
		}
		::pthread_mutexattr_destroy(&pAttr);
		# endif
		# endif // no thread safe
	}


	inline void Mutex::copy(const Mutex& rhs)
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		InitializeCriticalSectionAndSpinCount(&pSection, static_cast<DWORD>(spinCount));
		(void) rhs; // unused
		# else
		::pthread_mutexattr_init(&pAttr);
		int type; // = PTHREAD_MUTEX_NORMAL;
		if (0 == ::pthread_mutexattr_gettype(&rhs.pAttr, &type))
		{
			if (PTHREAD_MUTEX_RECURSIVE == type)
			{
				# if defined(YUNI_OS_DARWIN) or defined(YUNI_OS_FREEBSD) or defined(YUNI_OS_SOLARIS) or defined(YUNI_OS_SUNOS) or defined(YUNI_OS_HAIKU) or defined(YUNI_OS_CYGWIN)
				::pthread_mutexattr_settype(&pAttr, PTHREAD_MUTEX_RECURSIVE);
				# else
				::pthread_mutexattr_settype(&pAttr, PTHREAD_MUTEX_RECURSIVE_NP);
				# endif
			}
		}
		::pthread_mutex_init(& pLock, &pAttr);
		# endif
		# else
		(void) rhs; // unused
		# endif // no thread safe
	}



	Mutex::Mutex(const Mutex& rhs)
		: NonMovable(rhs)
	{
		copy(rhs);
	}


	Mutex::~Mutex()
	{
		destroy();
	}


	Mutex::Mutex(bool recursive)
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		(void) recursive; // already recursive on Windows
		InitializeCriticalSectionAndSpinCount(&pSection, static_cast<DWORD>(spinCount));
		# else
		::pthread_mutexattr_init(&pAttr);
		if (recursive)
		{
			# if defined(YUNI_OS_DARWIN) or defined(YUNI_OS_FREEBSD) or defined(YUNI_OS_SOLARIS) or defined(YUNI_OS_SUNOS) or defined(YUNI_OS_HAIKU) or defined(YUNI_OS_CYGWIN)
			::pthread_mutexattr_settype(&pAttr, PTHREAD_MUTEX_RECURSIVE);
			# else
			::pthread_mutexattr_settype(&pAttr, PTHREAD_MUTEX_RECURSIVE_NP);
			# endif
		}
		::pthread_mutex_init(&pLock, &pAttr);
		# endif
		# else
		(void) recursive;
		# endif
	}


	Mutex& Mutex::operator = (const Mutex& rhs)
	{
		// We will recreate the mutex
		destroy();
		copy(rhs);
		return *this;
	}





} // namespace Yuni

