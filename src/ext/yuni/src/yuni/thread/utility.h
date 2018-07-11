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
#include "../yuni.h"
#include "timer.h"
#include "../job/job.h"
#include "../core/bind.h"

namespace Yuni { namespace Job { class QueueService; }}
namespace Yuni { namespace Job { class Taskgroup; }}


namespace Yuni
{


	/*!
	** \brief Convenient wrapper for executing some code into another thread
	**
	** The callback will be executed into another thread.
	**
	** From sample:
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/thread/utility.h>
	** #include <yuni/core/system/suspend.h>
	** #include <iostream>
	** using namespace Yuni;
	**
	** int main()
	** {
	**	auto thread1 = spawn([&] () {
	**		std::cout << "thread1: some complex computations here..." << std::endl;
	**		// suspend the execution of the thread for 3 seconds for the demo
	**		thread1->suspend(3000);
	**		std::cout << "thread1: done here !" << std::endl;
	**	});
	**
	**	auto thread2 = spawn([&] () {
	**		std::cout << "thread2: some complex computations here..." << std::endl;
	**		// suspend the execution of the thread for 3 seconds for the demo
	**		thread2->suspend(2000);
	**		std::cout << "thread2: done here !" << std::endl;
	**	});
	**
	**	// Suspend the execution of the main thread until all threads are terminated
	**	thread1->wait();
	**	thread2->wait();
	**	return 0;
	** }
	** \endcode
	** \param callback The callback to execute
	** \param autostart True to automatically start the timer
	*/
	Thread::IThread::Ptr  spawn(const Bind<void ()>& callback, bool autostart = true);


	/*!
	** \brief Convenient wrapper for executing some code every X milliseconds
	**
	** The code will be executed into another thread.
	** From sample:
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/thread/thread.h>
	** #include <yuni/core/system/suspend.h>
	** #include <iostream>
	** using namespace Yuni;
	**
	** int main()
	** {
	**	uint counter = 0;
	**	auto timer = every(1000, [&] () -> bool {
	**		std::cout << ++counter << std::endl;
	**		return true; // continue looping
	**	});
	**
	**	Suspend(5); // wait for 5 seconds
	**	return 0;
	** }
	** \endcode
	**
	** \param ms An amount of time, in milliseconds
	** \param callback The callback to execute
	** \param autostart True to automatically start the timer
	*/
	Thread::Timer::Ptr  every(uint ms, const Bind<bool ()>& callback, bool autostart = true);


	/*!
	** \brief Convenient wrapper for executing some code every X milliseconds and
	** getting the time elapsed between each tick
	**
	** The code will be executed into another thread.
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/thread/thread.h>
	** #include <yuni/core/system/suspend.h>
	** #include <iostream>
	** using namespace Yuni;
	**
	** int main()
	** {
	**	auto timer = every(1000, false, [&] (uint64 elapsed) -> bool {
	**		std::cout << elapsed << "ms since the last time" << std::endl;
	**		return true; // continue looping
	**	});
	**
	**	Suspend(5); // wait for 5 seconds
	**	return 0;
	** }
	** \endcode
	**
	** \param ms An amount of time, in milliseconds
	** \param precise True to not take into consideration the time spent in the callback (with performance penalty)
	** \param callback The callback to execute
	** \param autostart True to automatically start the timer
	*/
	Thread::Timer::Ptr  every(uint ms, bool precise, const Bind<bool (uint64 /*elapsed*/)>& callback, bool autostart = true);


	/*!
	** \brief Convenient wrapper for executing a new job
	**
	** \code
	** #include <yuni/yuni.h>
	** #include <yuni/thread/utility.h>
	** #include <yuni/core/system/suspend.h>
	** #include <iostream>
	** using namespace Yuni;
	**
	** int main()
	** {
	**	async([&] () {
	**		std::cout << "thread1: some complex computations here..." << std::endl;
	**		std::cout << "thread1: done here !" << std::endl;
	**	});
	**	Suspend(2);
	**	return 0;
	** }
	** \endcode
	**
	** \param queueservice The queueservice to dispatch the new job
	** \param callback The callback to execute
	*/
	Job::IJob::Ptr  async(Job::QueueService& queueservice, const Bind<void ()>& callback);


	/*!
	** \brief Convenient wrapper for creating a job from a lambda
	**
	** Since no queueservice is provided, the job will only be created and not added in
	** any queue (and thus not executed).
	**
	** \param callback The callback to execute
	*/
	Job::IJob::Ptr  async(const Bind<bool (Job::IJob&)>& callback);


	/*!
	** \brief Convenient wrapper for creating a job from a lambda into a taskgroup
	*/
	void async(Job::Taskgroup& task, const Bind<bool (Job::IJob&)>& callback);




} // namespace Yuni
