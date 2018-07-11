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
#include "iterator.h"
#include "../../../core/traits/cstring.h"
#include "../../../core/traits/length.h"
#include "../../../core/static/remove.h"
#include "../../io.h"




namespace Yuni
{
namespace IO
{
namespace Directory
{

	# ifndef YUNI_NO_THREAD_SAFE
	template<bool DetachedT>
	inline IIterator<DetachedT>::DetachedThread::DetachedThread()
	{}

	template<bool DetachedT>
	inline IIterator<DetachedT>::DetachedThread::~DetachedThread()
	{
		stop();
	}
	# endif



	template<bool DetachedT>
	inline IIterator<DetachedT>::IIterator()
		# ifndef YUNI_NO_THREAD_SAFE
		:pThread(NULL)
		# endif
	{
	}

	template<bool DetachedT>
	inline IIterator<DetachedT>::IIterator(const IIterator& rhs)
		# ifndef YUNI_NO_THREAD_SAFE
		:pThread(NULL)
		# endif
	{
		typename ThreadingPolicy::MutexLocker locker(rhs);
		pRootFolder = rhs.pRootFolder;
	}

	template<bool DetachedT>
	IIterator<DetachedT>& IIterator<DetachedT>::operator = (const IIterator<DetachedT>& rhs)
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		typename ThreadingPolicy::MutexLocker locker2(rhs);
		pRootFolder = rhs.pRootFolder;
		return *this;
	}


	template<bool DetachedT>
	inline IIterator<DetachedT>::~IIterator()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		stop();
		delete pThread;
		# endif
	}


	template<bool DetachedT>
	inline void IIterator<DetachedT>::clear()
	{
		typename ThreadingPolicy::MutexLocker locker(*this);
		pRootFolder.clear();
	}



	template<bool DetachedT>
	inline void IIterator<DetachedT>::add(const AnyString& folder)
	{
		if (not folder.empty())
		{
			// Pushing it into the list
			String* item = new String();
			IO::Canonicalize(*item, folder);

			typename ThreadingPolicy::MutexLocker locker(*this);
			pRootFolder.push_back(item);
		}
	}


	template<bool DetachedT>
	bool IIterator<DetachedT>::start()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		if (detached)
		{
			// Lock
			typename ThreadingPolicy::MutexLocker locker(*this);

			// Early detection of an invalid root folder
			if (pRootFolder.empty())
				return false;

			if (nullptr == pThread)
			{
				pThread = new DetachedThread();
			}
			else
			{
				// Do nothing if already started
				if (pThread->started())
					return false;
			}

			// Providing a reference to ourselves for events
			pThread->options.self = this;
			pThread->options.rootFolder = pRootFolder; // copy

			// Starting the thread
			return (Thread::errNone == pThread->start());
		}
		else
		# endif
		{
			using namespace Yuni::Private::IO::Directory::Iterator;
			Options opts;
			opts.self = this; // Providing a reference to ourselves for events
			{
				// Lock
				typename ThreadingPolicy::MutexLocker locker(*this);
				// Early detection of an invalid root folder
				if (pRootFolder.empty())
					return false;

				opts.rootFolder = pRootFolder;
			}

			// The calling thread will block until the traversing is complete
			Traverse(opts, NULL);
		}
		return true;
	}


	template<bool DetachedT>
	bool IIterator<DetachedT>::stop(uint timeout)
	{
		# ifndef YUNI_NO_THREAD_SAFE
		if (detached)
		{
			typename ThreadingPolicy::MutexLocker locker(*this);
			if (pThread)
			{
				bool success = (Yuni::Thread::errNone == pThread->stop(timeout));
				if (success)
				{
					delete pThread; // be resources friendly
					pThread = nullptr;
				}
				return success;
			}
		}
		# else
		(void) timeout; // to avoid compiler warning
		# endif
		return true;
	}


	template<bool DetachedT>
	inline void IIterator<DetachedT>::wait()
	{
		# ifndef YUNI_NO_THREAD_SAFE
		if (detached)
		{
			// Lock
			typename ThreadingPolicy::MutexLocker locker(*this);
			if (pThread)
			{
				pThread->wait();
				delete pThread; // be resources friendly
				pThread = nullptr;
			}
		}
		# endif
	}


	template<bool DetachedT>
	inline void IIterator<DetachedT>::wait(uint timeout)
	{
		# ifndef YUNI_NO_THREAD_SAFE
		if (detached)
		{
			// Lock
			typename ThreadingPolicy::MutexLocker locker(*this);
			if (pThread)
				pThread->wait(timeout);
		}
		# else
		(void) timeout;
		# endif
	}



	template<bool DetachedT>
	inline Flow
	IIterator<DetachedT>::onBeginFolder(const String&, const String&, const String&)
	{
		return flowContinue; // Do nothing
	}

	template<bool DetachedT>
	inline void
	IIterator<DetachedT>::onEndFolder(const String&, const String&, const String&)
	{
		// Do nothing
	}

	template<bool DetachedT>
	inline Flow
	IIterator<DetachedT>::onFile(const String&, const String&, const String&, uint64)
	{
		return flowContinue; // Do nothing
	}

	template<bool DetachedT>
	inline Flow
	IIterator<DetachedT>::onError(const String&)
	{
		return flowContinue; // Do nothing
	}


	template<bool DetachedT>
	inline Flow
	IIterator<DetachedT>::onAccessError(const String&)
	{
		return flowContinue;
	}


	template<bool DetachedT>
	inline void IIterator<DetachedT>::onAbort()
	{
		// Do nothing
	}


	template<bool DetachedT>
	inline void IIterator<DetachedT>::onTerminate()
	{
		// Do nothing
	}


	template<bool DetachedT>
	inline bool IIterator<DetachedT>::onStart(const String&)
	{
		// Do nothing
		return true;
	}





} // namespace Directory
} // namespace IO
} // namespace Yuni

