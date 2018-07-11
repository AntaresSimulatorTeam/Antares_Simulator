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
#include "../../../thread/thread.h"



namespace Yuni
{
namespace Private
{
namespace IO
{
namespace Directory
{
namespace Iterator
{

	// Forward declarations
	class Interface;
	class Options;
	class IDetachedThread;

	typedef Yuni::IO::Flow Flow;

	/*!
	** \brief Platform specific iplementation for traversing a folder
	*/
	void Traverse(Options& options, IDetachedThread* thread);

	Flow TraverseUnixFolder(const String&, Options& options, IDetachedThread* thread, bool files);
	Flow TraverseWindowsFolder(const String&, Options& options, IDetachedThread* thread, bool files);




	class Interface
	{
	public:
		Interface() {}
		virtual ~Interface() {}

	protected:
		virtual bool onStart(const String& root) = 0;

		virtual void onTerminate() = 0;

		virtual void onAbort() = 0;

		virtual Flow onBeginFolder(const String& filename, const String& parent, const String& name) = 0;
		virtual void onEndFolder(const String& filename, const String& parent, const String& name) = 0;

		virtual Flow onFile(const String& filename, const String& parent, const String& name, uint64 size) = 0;

		virtual Flow onError(const String& filename) = 0;

		virtual Flow onAccessError(const String& filename) = 0;

	public:
		friend void Traverse(Options&, IDetachedThread*);
		friend Flow TraverseUnixFolder(const String&, Options&, IDetachedThread*, bool);
		friend Flow TraverseWindowsFolder(const String&, Options&, IDetachedThread*, bool);
	}; // class Interface



	class Options final
	{
	public:
		//! Default constructor
		Options()
			: self(nullptr)
			# ifdef YUNI_OS_WINDOWS
			, wbuffer(nullptr)
			# endif
			, counter(0)
		{}

	public:
		/*!
		** \brief The root folder
		** \internal The shared access to this variable is already guaranted
		**   by the class IIterator
		*/
		String::VectorPtr rootFolder;
		//! Pointer to the parent class
		Interface* self;

		# ifdef YUNI_OS_WINDOWS
		wchar_t* wbuffer;
		# endif

		/*!
		** \brief Arbitrary counter to reduce the number of calls to suspend()
		*/
		uint counter;
	};


	# ifndef YUNI_NO_THREAD_SAFE
	class IDetachedThread : public Yuni::Thread::IThread
	{
	public:
		IDetachedThread() {}
		virtual ~IDetachedThread()
		{
			# ifdef YUNI_OS_WINDOWS
			delete[] options.wbuffer;
			options.wbuffer = nullptr;
			# endif
		}

		bool suspend()
		{
			return Yuni::Thread::IThread::suspend();
		}

	public:
		Options options;

	protected:
		virtual bool onExecute() override
		{
			Traverse(options, this);
			return false;
		}

		virtual void onStop() override
		{
			# ifdef YUNI_OS_WINDOWS
			delete[] options.wbuffer;
			options.wbuffer = nullptr;
			# endif
		}

		virtual void onKill() override
		{
			# ifdef YUNI_OS_WINDOWS
			delete[] options.wbuffer;
			options.wbuffer = nullptr;
			# endif
		}

	}; // class IDetachedThread

	# else
	class IDetachedThread {};
	# endif




} // namespace Iterator
} // namespace Directory
} // namespace IO
} // namespace Private
} // namespace Yuni

