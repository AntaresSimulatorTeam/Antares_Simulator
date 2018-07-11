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
#include "../../yuni.h"
#include "../../thread/policy.h"
#include "null.h"
#include "verbosity.h"

// Default Handler
#include "handler/stdcout.h"
#include "handler/file.h"
// Default decorators
#include "decorators/verbositylevel.h"
#include "decorators/time.h"
#include "decorators/message.h"
#include "../noncopyable.h"
#include "buffer.h"


// The default verbosity level according the target mode (debug/release)
#ifdef NDEBUG
# define YUNI_LOGS_DEFAULT_VERBOSITY  Verbosity::Compatibility::level
#else
# define YUNI_LOGS_DEFAULT_VERBOSITY  Verbosity::Debug::level
#endif


/*!
** \brief Convenient macro for debugging
**
** \code
** logs.error() << YN_LOGENV << "my message";
** \endcode
*/
#define YN_LOGENV  __FILE__ << ':' << __LINE__ << ": "




namespace Yuni
{
namespace Logs
{

	/*!
	** \brief A customizable log facility
	**
	** A simple hello world :
	** \code
	** #include <yuni/core/logs.h>
	**
	** int main()
	** {
	**	// The logger
	**	Yuni::Logs::Logger<>  logs;
	**	// Hello world !
	**	logs.notice() << "Hello world !";
	**	return 0;
	** }
	** \endcode
	**
	** This class uses a static decorator pattern to modify its behavior. By default,
	** the output has the following format :
	** \code
	** [date][color][verbosity level][/color] <msg>
	** \endcode
	** The color output from the shell is available for both Unix (Terminal) and
	** Windows (cmd.exe).
	**
	**
	** \tparam TP The Threading Policy
	** \tparam Handlers A static list of all message handlers
	** \tparam Decorators A static list of all decorators
	*/
	template<
		class Handlers = StdCout<>,                             // List of all static handles
		class Decorators = Time< VerbosityLevel<Message<> > >,  // List of all static decorators
		template<class> class TP = Policy::ObjectLevelLockableNotRecursive // The Threading Policy
		>
	class YUNI_DECL Logger final :
		public TP<Logger<Handlers,Decorators,TP> >,             // inherits from the Threading Policy
		public Decorators,                                      // inherits from all decorators
		public Handlers,                                        // inherits from all handlers
		private NonCopyable<Logger<Handlers, Decorators, TP> >  // noncopyable
	{
	public:
		//! The full prototype of the logger
		typedef Logger<Handlers, Decorators, TP>  LoggerType;
		//! The Threading Policy
		typedef TP<LoggerType>  ThreadingPolicy;

		//! Handlers
		typedef Handlers HandlersType;
		//! Decorators
		typedef Decorators  DecoratorsType;

		/*!
		** \brief Settings for the logger
		*/
		enum Settings
		{
			//! The default verbose level
			defaultVerbosityLevel = YUNI_LOGS_DEFAULT_VERBOSITY,
		};

	private:
		// Aliases (for code clarity)
		//! Alias for the CheckpointBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Checkpoint>    CheckpointBuffer;
		//! Alias for the NoticeBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Notice>        NoticeBuffer;
		//! Alias for the NoticeBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Info>          InfoBuffer;
		//! Alias for the WarningBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Warning>       WarningBuffer;
		//! Alias for the ErrorBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Error>         ErrorBuffer;
		//! Alias for the ProgressBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Progress>      ProgressBuffer;
		//! Alias for the CompatibilityBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Compatibility> CompatibilityBuffer;
		//! Alias for the FatalBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Fatal>         FatalBuffer;
		//! Alias for the DebugBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Debug>         DebugBuffer;
		//! Alias for a dummy writer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Info, 0>       DummyBuffer;
		//! Alias for the UnknownBuffer
		typedef Private::LogImpl::Buffer<LoggerType, Verbosity::Unknown>       UnknownBuffer;

	public:
		//! \name Constructors & Destructor
		//@{
		/*!
		** \brief Default Constructor
		*/
		Logger();
		/*!
		** \brief Destructor
		*/
		~Logger();
		//@}


		//! \name Checkpoint
		//@{
		CheckpointBuffer checkpoint() const;
		template<class U> CheckpointBuffer checkpoint(const U& u) const;
		//@}

		//! \name Notice
		//@{
		NoticeBuffer notice() const;
		template<class U> NoticeBuffer notice(const U& u) const;
		//@}

		//! \name Info
		//@{
		InfoBuffer info() const;
		template<class U> InfoBuffer info(const U& u) const;
		//@}

		//! \name Warning
		//@{
		WarningBuffer warning() const;
		template<class U> WarningBuffer warning(const U& u) const;
		//@}

		//! \name Error
		//@{
		ErrorBuffer error() const;
		template<class U> ErrorBuffer error(const U& u) const;
		//@}

		//! \name Progress
		//@{
		ProgressBuffer progress() const;
		template<class U> ProgressBuffer progress(const U& u) const;
		//@}

		//! \name Fatal
		//@{
		FatalBuffer fatal() const;
		template<class U> FatalBuffer fatal(const U& u) const;
		//@}

		//! \name Compatibility notice
		//@{
		CompatibilityBuffer compatibility() const;
		template<class U> CompatibilityBuffer compatibility(const U& u) const;
		//@}

		//! \name Debug (disabled if NDEBUG defined)
		//@{
		DebugBuffer debug() const;
		template<class U> DebugBuffer debug(const U& u) const;
		//@}


		//! Start a custom verbosity level message
		template<class C> Private::LogImpl::Buffer<LoggerType,C,C::enabled> custom() const;

		//! Start a message with no verbosity level (always displayed)
		template<class U> UnknownBuffer operator << (const U& u) const;
		//@}


	public:
		/*!
		** \brief The current maximum verbosity level allowed to be displayed
		**
		** \code
		** Logs::Logger<> logs;
		**
		** // Starting with default verbosity level
		** // The following messages will be displayed
		** logs.error() << "An error";
		** logs.notice() << "Hello world";
		**
		** // Changing the verbosity level
		** logs.verbosityLevel = Logs::Verbosity::Error::level;
		** // Only the 'error' message will be displayed
		** logs.error() << "An error";
		** logs.notice() << "Hello world";
		** \endcode
		*/
		int verbosityLevel;

	private:
		/*!
		** \brief Transmit a message to all handlers
		*/
		template<class VerbosityType>
		void dispatchMessageToHandlers(const AnyString& message) const;

		// A friend !
		template<class, class, int> friend class Private::LogImpl::Buffer;

	}; // class Logger





} // namespace Logs
} // namespace Yuni

#include "logs.hxx"
