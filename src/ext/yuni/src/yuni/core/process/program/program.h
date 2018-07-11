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
#include "../../../yuni.h"
#include "../../../core/string.h"
#include "../../atomic/int.h"
#include "../../bind.h"
#include "stream.h"



namespace Yuni
{
namespace Process
{


	/*!
	** \brief Execute a command and wait for it
	**
	** \param commandline An arbitrary command line (example: "ls -l")
	** \param timeout Maximum execution time allowed for the command (in seconds - 0 means infinite)
	** \return True if the command has been executed and if the exit status is equal to 0
	*/
	bool Execute(const AnyString& commandline, uint timeout = 0u);


	/*!
	** \brief Execute a command into a given loop and wait for it
	**
	** \param mainloop A mainloop from where the program should be launched
	** \param commandline An arbitrary command line (example: "ls -l")
	** \param timeout Maximum execution time allowed for the command (in seconds - 0 means infinite)
	** \return True if the command has been executed and if the exit status is equal to 0
	*/
	template<class MainLoopT>
	bool Execute(MainLoopT& mainloop, const AnyString& commandline, uint timeout);


	/*!
	** \brief Execute a command and capture the outputs
	**
	** \param commandline An arbitrary command line (example: "ls -l")
	** \param timeout Maximum execution time allowed for the command (in seconds - 0 means infinite)
	** \return True if the command has been executed and if the exit status is equal to 0
	*/
	bool System(String* cout, String* cerr, const AnyString& commandline, uint timeout = 0u);

	/*!
	** \brief Execute a command and capture the outputs
	**
	** \param commandline An arbitrary command line (example: "ls -l")
	** \param timeout Maximum execution time allowed for the command (in seconds - 0 means infinite)
	** \return The command output (cout)
	*/
	String System(const AnyString& commandline, bool trim = true, uint timeout = 0u);




	/*!
	** \brief Start external programs and communicate with them
	**
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/core/process/program.h>
	**
	** int main()
	** {
	**	Process::Program process;
	**	process.program("ls");
	**	process.argumentAdd("-l");
	**	process.argumentAdd("/tmp");
	**	process.execute();
	**	return process.wait();
	** }
	** \endcode
	**
	** By default, the output of the underlying process is not redirected to the
	** current console, even if a stream handler is provided. `redirectToConsole()` can be
	** use to modify this behavior.
	**
	** This class (and all its public methods) is thread-safe.
	**
	** \internal The 'Stream' object may be shared and reused
	** \internal `std::cout` and `std::cerr` should be used instead of `write` when redirecting
	**  the outputs in order to share the same buffer and to have cleaner output
	*/
	class YUNI_DECL Program final
	{
	public:
		//! Callback for main loop
		typedef Bind<bool ()>  Callback;

		//! Precision used for calculating the time spent during the execution of the underlying process
		enum DurationPrecision
		{
			//! In seconds (the default)
			dpSeconds,
			//! In milliseconds
			dpMilliseconds,
			//! None (disabled, the duration will always be equal to zero)
			dpNone,
		};


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Program();
		//! Copy constructor
		Program(const Program& rhs);
		//! Destructor
		~Program();
		//@}

		//! \name Options & Arguments
		//@{
		/*!
		** \brief Reset all arguments from a complete command line (ex: `ls -l /somewhere`)
		*/
		void commandLine(AnyString cmd);

		//! Get the program that will be executed
		String program() const;
		//! Set the program that will be executed
		void program(const AnyString& prgm);

		//! Remove all arguments
		void argumentClear();
		//! Add a new argument for the program to execute
		void argumentAdd(const AnyString& arg);

		/*!
		** \brief Set the working directory when executing the process
		**
		** Empty will use the current directory.
		*/
		void workingDirectory(const AnyString& directory);
		//! Get the working directory that will be used for executing the process
		String workingDirectory() const;
		//@}


		//! \name Process Execution
		//@{
		/*!
		** \brief Execute the process
		**
		** Try to execute the process from the current thread.
		**
		** \param timeout Maximum execution time allowed for the command (in seconds - 0 means infinite)
		** \return True if the command has been launched
		*/
		bool execute(uint timeout = 0u);

		/*!
		** \brief Execute the command with the cooperation of a main loop
		**
		** The execution of the process will be done from the main thread
		** (actually the thread associated to the main loop, which should be
		** the main thread). Then you can wait for your process from any thread
		** with the method `wait()`
		**
		** \param timeout Maximum execution time allowed for the command (in seconds - 0 means infinite)
		** \return True if the command has been launched
		*/
		template<class MainLoopT> bool execute(MainLoopT& mainloop, uint timeout = 0u);
		//@}


		//! \name Process control
		//@{
		/*!
		** \brief Wait for the end of the sub-process
		**
		** This method can be called from any thread.
		**
		** \param[out] duration An output variable for storing the execution time
		**    (in seconds or milliseconds, according the precision required) [optional]
		** \return The exit status of the process
		** \see durationPrecision()
		*/
		int wait(sint64* duration = nullptr);

		/*!
		** \brief Terminate the execution of the process
		**
		** This is the prefered way to terminate a process. On Unix, SIGTERM
		** will be sent, which can be intercepted.
		*/
		void terminate();

		/*!
		** \brief Kill the process (cause immediate program termination)
		**
		** The method `terminate()` should be prefered for simply stopping a process.
		*/
		void kill();


		/*!
		** \brief Get if the process is currently running
		**
		** The returned value is only a hint, since the status of the sub-process
		** may change anytime.
		** \see Program::wait()
		*/
		bool running() const;


		/*!
		** \brief Send a signal to the sub-process (UNIX only)
		**
		** \note This routine has no effect on systems where signals are not supported
		** \see man 3 signal
		** \see Program::terminate()
		** \param sig The signal to send (SIGINT, SIGTERM, ...)
		*/
		void signal(int sig);
		//@}


		//! \name Stream - capturing output
		//@{
		//! Get if stdcout/stderr are redirected to the console (default: false)
		bool redirectToConsole() const;
		//! Set if stdcout/stderr are redirected to the console, even if a stream handler is provided
		// (only if the process is not running)
		void redirectToConsole(bool flag);

		//! Set the stream handler used for capturing events
		void stream(Stream::Ptr newstream);
		//! Get the stream used for capturing events
		Stream::Ptr stream() const;
		//@}


		//! \name Precision used for calculating the duration of the execution of the process
		//@{
		//! Get the precision currently in use
		DurationPrecision  durationPrecision() const;
		//! Set the precision used for calculating a duration (only if the process is not running)
		void durationPrecision(DurationPrecision precision);
		//@}


		//! \name Operators
		//@{
		//! Assignment
		Program& operator = (const Program& rhs);
		//@}


	private:
		//! Dispatch the execution of the process to an event loop
		bool dispatchExecution(const Bind<void (const Callback&)>& dispatcher, uint timeout);

	private:
		// forward declaration
		class ProcessSharedInfo;
		class ThreadMonitor;
		//! Alias to the thread monitor's smartptr
		typedef SmartPtr<Yuni::Process::Program::ThreadMonitor, Yuni::Policy::Ownership::COMReferenceCounted>  ThreadPtr;

		//! Information on the program currently executed
		// \note This class may be shared by several threads
		SmartPtr<ProcessSharedInfo> pEnv;
		//! Stream
		Stream::Ptr pStream;

	}; // class Program







} // namespace Process
} // namespace Yuni

#include "program.hxx"
