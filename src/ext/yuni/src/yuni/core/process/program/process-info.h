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
#include "program.h"
#include "../../noncopyable.h"
#include "../../../thread/thread.h"
#ifdef YUNI_OS_UNIX
# include <sys/types.h>
#endif
#include <signal.h>




namespace Yuni
{
namespace Process
{

	/*!
	** \brief Helper class for storing shared information on the program which is currently
	** launched by Process::Program
	**
	** \note This class may be shared by several threads
	*/
	class Program::ProcessSharedInfo final : public Yuni::NonCopyable<Program::ProcessSharedInfo>
	{
	public:
		//! Smart pointer
		// \note This type must match the definition of \p pEnv
		typedef SmartPtr<ProcessSharedInfo> Ptr;

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		ProcessSharedInfo();
		//! Destructor
		~ProcessSharedInfo();
		//@}


		/*!
		** \return True if the signal has been delivered
		*/
		template<bool WithLock> bool sendSignal(int value);

		/*!
		** \brief Create a thread dedicated to handle the execution timeout
		*/
		void createThreadForTimeoutWL();


	public:
		String executable;
		//! The command
		String::Vector arguments;
		//! The working directory
		String workingDirectory;
		//! Flag to know if the process is running
		bool running;
		//! PID
		int processID;
		//! input file descriptors
		int processInput;
		//! Thread
		ThreadPtr thread;
		//! Duration in seconds
		sint64 duration;
		//! Duration precision
		DurationPrecision durationPrecision;
		//! Timeout
		uint timeout;
		//! Exit status
		int exitstatus;
		//! Console
		bool redirectToConsole;
		//! Mutex
		mutable Mutex mutex;

		//! Optional thread for timeout
		Yuni::Thread::IThread* timeoutThread;

	}; // class Program::ProcessSharedInfo









} // namespace Process
} // namespace Yuni

#include "process-info.hxx"
