/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_COMMON_ACTION_ACTION_H__
# define __ANTARES_COMMON_ACTION_ACTION_H__

# include <yuni/yuni.h>
# include <yuni/job/job.h>
# include <yuni/core/bind.h>
# include <yuni/core/event.h>



namespace Antares
{
/*!
** \brief An ASynchroneous Job Dispatcher
*/
namespace Dispatcher
{
namespace GUI
{
	/*!
	** \brief post a new event to run in the main loop
	**
	** note: this method will be called from the main thread
	*/
	template<class C>
	void Post(const C* object, void (C::* method)(void));

	/*!
	** \brief post a new event to run in the main loop
	**
	** note: this method will be called from the main thread
	*/
	template<class C>
	void Post(const C* object, void (C::* method)(void), uint delay);

	/*!
	** \brief Post a new event to run in the main loop
	**
	** Note: This method will be called from the main thread
	*/
	void Post(const Yuni::Job::IJob::Ptr& job);

	/*!
	** \brief Post a new event to run in the main loop
	**
	** Note: This method will be called from the main thread
	*/
	void Post(const Yuni::Job::IJob::Ptr& job, uint delay);

	/*!
	** \brief Post a new event to run in the main loop
	**
	** Note: This method will be called from the main thread
	*/
	void Post(const Yuni::Bind<void ()>& job);

	/*!
	** \brief Post a new event to run in the main loop (with a delay)
	**
	** Note: This method will be called from the main thread
	*/
	void Post(const Yuni::Bind<void ()>& job, uint delay);
	
} // namespace GUI




	/*!
	** \brief Post a new event to run in the thread pool
	**
	** Note: This method may run from any thread
	*/
	template<class C>
	void Post(const C* object, void (C::* method)());

	/*!
	** \brief Post a new event to run in the thread pool
	**
	** Note: This method may run from any thread
	*/
	template<class C, class UserDataT>
	void Post(const C* object, void (C::* method)(), const UserDataT& userdata);

	/*!
	** \brief Post a new job in the thread pool
	**
	** Note: This job may run from any thread
	*/
	void Post(const Yuni::Job::IJob::Ptr& job);

	/*!
	** \brief Post a new event to run in the thread pool
	**
	** Note: This method will be called from the any thread
	*/
	void Post(const Yuni::Bind<void ()>& job);



	/*!
	** \brief Start the ASync Dispatcher
	*/
	bool Start();

	/*!
	** \brief Stop the ASync Dispatcher
	**
	** \param timeout The timeout (in milliseconds) for waiting for the working jobs
	*/
	void Stop(uint timeout = 10000 /*10s*/);


	/*!
	** \brief Wait for the end of all jobs
	*/
	void Wait();

	/*!
	** \brief Get if some jobs are still running
	*/
	bool Empty();



} // namespace Dispatcher
} // namespace Antares

# include "action.hxx"

#endif // __ANTARES_COMMON_ACTION_ACTION_H__
