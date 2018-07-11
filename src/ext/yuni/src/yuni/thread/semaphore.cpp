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
#include "semaphore.h"
#include <cassert>
#include <iostream>
#include "../core/string.h"
#ifdef YUNI_OS_MAC
# include <unistd.h>
# include <semaphore.h>
#endif
#ifdef YUNI_OS_LINUX
# include <fcntl.h>    // for contants
# include <sys/stat.h>
# include <limits.h>
# include <semaphore.h>
#endif
#ifdef YUNI_HAS_ERRNO_H
# include <errno.h>
#endif
#ifdef YUNI_OS_WINDOWS
# include "../core/system/windows.hdr.h"
#endif


namespace Yuni
{

	// for keeping the method local
	void Semaphore::initialize(uint readers)
	{
		if (readers == 0) // can not be null
			readers = 1;
		assert(readers < 1024 * 1024 and "the number of readers seems quite high...");

		# ifndef YUNI_NO_THREAD_SAFE

		# ifdef YUNI_OS_WINDOWS

		pSemaphore = CreateSemaphore(nullptr, readers, readers, nullptr);
		if (not pSemaphore)
		{
			std::cerr << "impossible to create a new unamed semaphore" << std::endl;
			assert(false and "impossible to initialize a semaphore");
			exit(-1);
		}

		# else

		// In every cases, for all UNIX operating systems, readers must not be greater than SEM_VALUE_MAX
		if (readers > SEM_VALUE_MAX)
		{
			# ifndef NDEBUG
			std::cerr << "\nYuni::Semaphore: readers > SEM_VALUE_MAX\n";
			# endif
			readers = SEM_VALUE_MAX;
		}

		#	ifdef YUNI_OS_MAC

		// OS X does not support unnamed semaphores
		ShortString128 name;
		name << "yuni_" << getpid() << '-' << (void*) this;

		pSemaphore = sem_open(name.c_str(), O_CREAT, 0600, readers);
		if ((::sem_t*)pSemaphore == SEM_FAILED)
		{
			std::cerr << "impossible to create a new semaphore called " << name << std::endl;
			assert(false and "impossible to initialize a semaphore");
			exit(-1);
		}

		#	else

		// UNIX implementation - unamed semaphores
		// However it is not supported by OS X
		if (0 != sem_init(& pSemaphore, 0, readers))
		{
			switch (errno)
			{
				case EINVAL:
					{
						std::cerr << "`readers` exceeds SEM_VALUE_MAX (=" << (uint) (SEM_VALUE_MAX) << std::endl;
						assert(false and "impossible to initialize a semaphore: `readers` exceeds SEM_VALUE_MAX");
						break;
					}
				case ENOSYS:
					{
						std::cerr << "the system does not support process-shared semaphores\n";
						assert(false and "impossible to initialize a semaphore: the system does not support process-shared semaphores)");
						break;
					}
				default:
					{
						std::cerr << "impossible to initialize a semaphore\n";
						assert(false and "impossible to initialize a semaphore");
					}
			}
			exit(-1);
		}
		#	endif

		# endif
		# endif
	}


	// for keeping the method local
	void Semaphore::finalize()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		CloseHandle(pSemaphore);
		# else
		#	ifdef YUNI_OS_MAC
		sem_close((::sem_t*)pSemaphore);
		#	else
		if (0 != sem_destroy(& pSemaphore))
		{
			std::cerr << "impossible to destroy a semaphore\n";
			assert(false and "impossible to destroy a semaphore");
			exit(-1);
		}
		#	endif
		# endif
		# endif
	}

	void Semaphore::acquire()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		WaitForSingleObject(pSemaphore, INFINITE);
		# else
		#	ifdef YUNI_OS_MAC
		sem_wait((::sem_t*)pSemaphore);
		#	else
		sem_wait(& pSemaphore);
		#	endif
		# endif
		# endif
	}


	void Semaphore::acquire(uint n)
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		for (uint i = 0; i != n; ++i)
			WaitForSingleObject(pSemaphore, INFINITE);
		# else
		#	ifdef YUNI_OS_MAC
		for (uint i = 0; i != n; ++i)
			sem_wait((::sem_t*)pSemaphore);
		#	else
		for (uint i = 0; i != n; ++i)
			sem_wait(& pSemaphore);
		#	endif
		# endif
		# endif
	}


	void Semaphore::release()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		ReleaseSemaphore(pSemaphore, 1, nullptr);
		# else
		#	ifdef YUNI_OS_MAC
		sem_post((::sem_t*)pSemaphore);
		#	else
		sem_post(& pSemaphore);
		#	endif
		# endif
		# endif
	}


	void Semaphore::release(uint n)
	{
		# ifndef YUNI_NO_THREAD_SAFE
		# ifdef YUNI_OS_WINDOWS
		if (n > 0)
			ReleaseSemaphore(pSemaphore, n, nullptr);
		# else
		#	ifdef YUNI_OS_MAC
		for (uint i = 0; i != n; ++i)
			sem_post((::sem_t*)pSemaphore);
		#	else
		for (uint i = 0; i != n; ++i)
			sem_post(& pSemaphore);
		#	endif
		# endif
		# endif
	}




	Semaphore::Semaphore(uint readers)
		: pMaxReaders(readers)
	{
		initialize(readers);
	}


	Semaphore::Semaphore(const Semaphore& rhs)
		: pMaxReaders(rhs.pMaxReaders)
	{
		initialize(pMaxReaders);
	}


	Semaphore::~Semaphore()
	{
		finalize();
	}


	Semaphore& Semaphore::operator = (const Semaphore& copy)
	{
		uint readers = copy.maxReaders();
		if (readers != pMaxReaders)
		{
			finalize();
			initialize(readers);
		}
		return *this;
	}




} // namespace Yuni
