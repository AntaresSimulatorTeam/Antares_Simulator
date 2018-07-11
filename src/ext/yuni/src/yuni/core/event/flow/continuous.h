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
#include "../../../thread/thread.h"



namespace Yuni
{
namespace Core
{
namespace EventLoop
{
namespace Flow
{


	template<class EventLoopT>
	class YUNI_DECL Continuous
	{
	public:
		//! Type of the event loop
		typedef EventLoopT EventLoopType;

	public:
		//! \name Constructor
		//@{
		/*!
		** \brief Default constructor
		*/
		Continuous() {}
		//@}

	protected:
		//! \name Events triggered by the public interface of the event loop (from any thread)
		//@{
		/*!
		** \brief The event loop has just started
		**
		** The event loop is locked when this method is called
		*/
		static bool onStart() {return true;}

		/*!
		** \brief The event loop has just stopped
		**
		** The event loop is locked when this method is called
		*/
		static bool onStop() {return true;}

		/*!
		** \brief A new request has just been added into the queue
		**
		** The event loop is locked when this method is called
		** \param request The request (bind, see EventLoopType::RequestType)
		** \return True to allow the request to be posted
		*/
		template<class U> static bool onRequestPosted(const U& request)
		{ (void) request; return true; }
		//@}

		//! \name Events triggered from the main thread of the event loop
		//@{
		/*!
		** \brief The event loop has started a new cycle
		**
		** This method is called from the main thread of the event loop.
		** No lock is provided.
		*/
		static bool onNewCycle()
		{
			// Do not wait, directly execute the cycle
			return true;
		}
		//@}

		/*!
		** \brief Event triggered from the constructor of the event loop
		** \param e Pointer to the original event loop
		*/
		static void onInitialize(EventLoopType* e)
		{
			(void) e;
			// Do nothing
		}

	}; // class Continuous<>





} // namespace Flow
} // namespace EventLoop
} // namespace Core
} // namespace Yuni
