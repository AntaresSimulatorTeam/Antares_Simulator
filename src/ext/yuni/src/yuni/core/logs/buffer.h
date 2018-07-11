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
#include "../string.h"



namespace Yuni
{
namespace Private
{
namespace LogImpl
{

	// Forward declaration
	template<class LogT, class V, int E = V::enabled> class Buffer;




	/*!
	** \brief The buffer for the message
	**
	** \internal This is an intermediate class that handles a temporary buffer where
	** the message can be built. The message will be dispatched to the static list
	** of handlers when this class is destroyed. The method `internalFlush()` is called
	** , which ensures thread-safety (if required) while the message is passing through
	** the handlers.
	**
	** \tparam V The verbosity level of the message
	** \tparam E A non-zero value if the message must be managed
	*/
	template<class LogT, class V, int E>
	class YUNI_DECL Buffer final
	{
	public:
		//! Type of the calling logger
		typedef LogT LoggerType;

	public:
		//! \name Constructos & Destructor
		//@{
		inline Buffer(const LoggerType& l) :
			pLogger(l)
		{}

		template<typename U>
		inline Buffer(const LoggerType& l, U u) :
			pLogger(l)
		{
			pBuffer.append(u);
		}

		~Buffer()
		{
			// Dispatching the messages to the handlers
			// For example, the buffer will be written to the output
			pLogger.template dispatchMessageToHandlers<V>(pBuffer);
		}
		//@}

		template<typename U> Buffer& operator << (const U& u)
		{
			// Appending the piece of message to the buffer
			pBuffer.append(u);
			return *this;
		}

		void appendFormat(const char f[], ...)
		{
			va_list parg;
			va_start(parg, f);
			pBuffer.vappendFormat(f, parg);
			va_end(parg);
		}

		void vappendFormat(const char f[], va_list parg)
		{
			pBuffer.vappendFormat(f, parg);
		}


	private:
		//! Reference to the original logger
		const LoggerType& pLogger;

		/*!
		** \brief Buffer that contains the message
		**
		** The chunk size can not be merely the default one; Log entries often
		** contain path of filename for example.
		*/
		Yuni::CString<1024> pBuffer;

	}; // class Buffer






	// Specialization when a verbosty level is disabled
	template<class LogT, class V>
	class YUNI_DECL Buffer<LogT, V, 0> final
	{
	public:
		//! Type of the calling logger
		typedef LogT LoggerType;

	public:
		Buffer(const LoggerType&) {}

		template<typename U> Buffer(const LoggerType&, U) {}

		~Buffer()
		{}

		template<typename U> const Buffer& operator << (const U&) const
		{
			// Do nothing - Disabled
			return *this;
		}

		void appendFormat(const char [], ...)
		{
			// Do nothing
		}

		void vappendFormat(const char [], va_list)
		{
			// Do nothing
		}

	}; // class Buffer





} // namespace LogImpl
} // namespace Private
} // namespace Yuni

