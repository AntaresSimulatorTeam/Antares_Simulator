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
#ifndef __YUNI_MEDIA_LOOP_H__
# define __YUNI_MEDIA_LOOP_H__

# include "../yuni.h"
# include "../core/event/loop.h"



namespace Yuni
{
namespace Media
{

	//! Forward Declaration
	class QueueService;


	/*!
	** \brief The media loop is where every media event is run
	*/
	class Loop: public Core::EventLoop::IEventLoop<Loop, Core::EventLoop::Flow::Timer,
		Core::EventLoop::Statistics::None, true>
	{
	public:
		//! The loop itself
		typedef Core::EventLoop::IEventLoop<Loop,
			Core::EventLoop::Flow::Timer,       // The flow policy
			Core::EventLoop::Statistics::None,  // The statistics policy
			true>                               // Use a separate thread
			LoopType;

		typedef LoopType::RequestType RequestType;

	public:
		/*!
		** \brief Constructor
		**
		** \param queueservice Media queue service corresponding to this loop
		*/
		Loop(QueueService* queueservice);

		//! Destructor (not virtual, this is static inheritance)
		~Loop() { stop(); }

		/*!
		** \brief Redefinition of the onLoop() callback
		*/
		bool onLoop();

		void beginClose();

		void endClose();

	private:
		//! Media queue service corresponding to this loop
		QueueService* pMediaService;
		//! When closing we must not execute onLoop()
		bool pClosing;

	}; // class Loop






} // namespace Media
} // namespace Yuni

#endif // __YUNI_MEDIA_LOOP_H__
