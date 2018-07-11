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




namespace Yuni
{
namespace Core
{
namespace EventLoop
{
namespace Statistics
{


	template<class EventLoopT>
	class YUNI_DECL None
	{
	public:
		//! Type of the event loop
		typedef EventLoopT EventLoopType;

	public:
		//! \name Constructor
		//@{
		/*!
		** \brief The default constructor
		*/
		None() {}
		//@}


	protected:
		//! \name Events triggered by the public interface of the event loop (from any thread)
		//@{
		/*!
		** \brief The event loop has just started
		**
		** The event loop is locked when this method is called
		*/
		static void onStart() {}

		/*!
		** \brief The event loop has just stopped
		**
		** The event loop is locked when this method is called
		*/
		static void onStop() {}

		/*!
		** \brief A new request has just been added into the queue
		**
		** The event loop is locked when this method is called
		** \param request The request (bind, see EventLoopType::RequestType)
		*/
		template<class U> static void onRequestPosted(const U& request)
		{ (void) request; }
		//@}


		//! \name Events triggered from the main thread of the event loop
		//@{
		/*!
		** \brief The event loop has started a new cycle
		**
		** This method is called from the main thread of the event loop.
		** No lock is provided.
		*/
		static void onNewCycle() {}

		/*!
		** \brief The event loop is processing a request
		**
		** This method is called from the main thread of the event loop.
		** No lock is provided.
		** \param request The request (bind, see EventLoopType::RequestType)
		*/
		template<class U> static void onProcessRequest(const U& request)
		{ (void) request; }

		/*!
		** \brief The event loop has just finished a cycle
		**
		** This method is called from the main thread of the event loop.
		** No lock is provided.
		*/
		static void onEndOfCycle() {}
		//@}

	}; // class None<>






} // namespace Statistics
} // namespace EventLoop
} // namespace Core
} // namespace Yuni
