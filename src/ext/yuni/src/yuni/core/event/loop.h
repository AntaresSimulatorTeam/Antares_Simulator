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
#include "event.h"
#include "../atomic/bool.h"
#include "../slist/slist.h"
#include "flow/continuous.h"
#include "flow/timer.h"
#include "statistics/none.h"
#include "../system/suspend.h"
#include "../static/assert.h"
#include "../static/inherit.h"
#include <cassert>
#include "../../thread/thread.h"
#include "loop.fwd.h"



namespace Yuni
{
namespace Core
{
/*!
**
** \brief Event loops
** \ingroup Core
*/
namespace EventLoop
{

	/*!
	** \brief A Synchronous Event Demultiplexer (Reactor pattern)
	** \ingroup Core
	**
	** This class handles requests (from any thread or process) from a single
	** thread. It is designed to run code that can not be thread-safe, like
	** calls to OpenGL or OpenAL routines.
	** If those requests must be run from the main thread (e.g. OpenGL), the
	** event loop must not be detached (see the template parameter 'DetachedT')
	** and the method 'start()' must be executed from the main thread.
	**
	** Here is a sample to create a minimalist event loop
	** \code
	** class MyEventLoop : public Core::EventLoop::IEventLoop<MyEventLoop>
	** {
	** public:
	**	bool onLoop()
	**	{
	**		std::cout << "loop !\n";
	**	}
	** };
	** \endcode
	**
	** \tparam ParentT The parent class (CRTP)
	** \tparam FlowT The flow policy
	** \tparam StatsT The statistics policy
	** \tparam DetachedT A non-zero value to use a separate thread than the calling
	**   one for running the event loop.
	**   This value should always be true for code safety unless if you want to
	**   execute the event loop from the calling thread (e.g. the main thread).
	**   In this case, you _have_ to take care about blocking system calls that
	**   would prevent the loop to stop.
	*/
	template<class ParentT,                               // The parent class (CRTP)
		template<class> class FlowT  = Flow::Continuous,  // The flow policy
		template<class> class StatsT = Statistics::None,  // The statistics policy
		bool DetachedT = true                             // Use a separate thread or not
		>
	class YUNI_DECL IEventLoop
		: public Policy::ObjectLevelLockableNotRecursive<IEventLoop<ParentT,FlowT,StatsT,DetachedT> >
		, public FlowT<IEventLoop<ParentT,FlowT,StatsT,DetachedT> >
		, public StatsT<IEventLoop<ParentT,FlowT,StatsT,DetachedT> >
		, private NonCopyable<IEventLoop<ParentT,FlowT,StatsT,DetachedT> >
	{
	public:
		//! Parent
		typedef ParentT  ParentType;
		//! A request
		typedef Bind<bool ()>  RequestType;
		//! List of requests
		typedef LinkedList<RequestType>  RequestListType;

		//! The Event loop
		typedef IEventLoop<ParentType, FlowT, StatsT, DetachedT>  EventLoopType;
		//! The most suitable smart pointer for the class
		typedef SmartPtr<EventLoopType>  Ptr;
		//! The threading policy
		typedef Policy::ObjectLevelLockableNotRecursive<EventLoopType>  ThreadingPolicy;
		//! The thread used for the inner loop
		typedef Yuni::Private::Core::EventLoop::Thread<EventLoopType>  ThreadType;

		//! The flow policy
		typedef FlowT<EventLoopType>  FlowPolicy;
		//! The statistics policy
		typedef StatsT<EventLoopType>  StatisticsPolicy;

		enum
		{
			/*!
			** \brief A non-zero value if a separate thread must be used for running the
			**   event loop instead of the calling one
			*/
			detached = DetachedT,
		};


	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		IEventLoop();
		//! Destructor
		~IEventLoop();
		//@}


		//! \name Loop management
		//@{
		/*!
		** \brief Start the event loop
		**
		** This method will be non-blocking in detached mode and the event loop
		** will be ran from another thread. Otherwise it will block
		** until the loop is stopped by a call to `stop()` from another thread
		** or a request that had failed.
		*/
		void start();

		/*!
		** \brief Ask to Stop the execution of the event loop as soon as possible
		**
		** This method will not wait the event loop termination
		*/
		void gracefulStop();

		/*!
		** \brief Gracefully stop the execution of the event loop, and wait for it
		**
		** In detached mode, the method will wait for the end of the execution
		** of the detached thread, and will kill it if the timeout expires.
		** When the event loop is not detached, there is no guarantee that the event loop
		** is really stopped after the call to this method, since there is no control
		** on the thread that had called start().
		**
		** \param timeout The number of milliseconds to wait before killing the event loop
		*/
		void stop(uint timeout = 5000 /* 5 seconds */);

		//! Is the event loop running ?
		bool running() const;
		//@}


		//! \name Event dispatching
		//@{
		/*!
		** \brief Post a new request into the queue
		**
		** \param request A request, which is merely a delegate via the class Bind<>)
		*/
		void dispatch(const RequestType& request);
		//@}


		//! \name Operators
		//@{
		//! The operator += : Dispatch a new request
		IEventLoop& operator += (const RequestType& request);
		//! The operator << : Dispatch a new request
		IEventLoop& operator << (const RequestType& request);
		//@}

	public:
		/*!
		** \brief On loop event handler
		**
		** \warning Has to be redefined by children classes !
		** \return False to stop the loop
		*/
		static bool onLoop();


	protected:
		//! Suspend the thread
		void suspend(uint timeout);

	private:
		//! Run an infinite loop
		void runInfiniteLoopWL();

		//! Perform all requests synchronously
		bool performAllRequestsWL();

		//! Run incoming events if any and one cycle
		bool runCycleWL();

	private:
		//! The number of events that wait to be triggered from the inner thread
		Atomic::Bool pHasRequests;
		//! List of incoming request
		RequestListType* pRequests;
		//! True if the event loop is running
		bool pIsRunning;
		//! External thread when ran in detached mode
		ThreadType* pThread;

		// Our friends
		friend FlowT<EventLoopType>;
		template<class EventLoopT> friend class Yuni::Private::Core::EventLoop::Thread;

	}; // class IEventLoop<>






	//! Empty method which always returns false, only used to stop the running event loop
	bool RequestStop();



} // namespace EventLoop
} // namespace Core
} // namespace Yuni

#include "loop.hxx"

