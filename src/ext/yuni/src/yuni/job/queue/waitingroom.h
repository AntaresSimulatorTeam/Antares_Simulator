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
#include "../job.h"
#include "../../core/slist/slist.h"
#include "../../core/atomic/bool.h"
#include <deque>



namespace Yuni
{
namespace Private
{
namespace QueueService
{

	/*!
	** \brief Container for all jobs waiting to be executed
	*/
	class YUNI_DECL WaitingRoom final
	{
	public:
		enum
		{
			//! Alias for The default priority
			priorityDefault = Yuni::Job::priorityDefault,
			//! Alias for The number of priorities
			priorityCount = Yuni::Job::priorityCount,
		};

	public:
		//! \name Constructor & Destructor
		//@{
		//! Destructor
		~WaitingRoom();
		//@}

		//! \name Container manipulation
		//@{
		/*!
		** \brief Get if the waiting room is empty
		*/
		bool empty() const;

		/*!
		** \brief Add a job into the waiting room, with a default priority
		**
		** The job will see its state changed to `stateWaiting`.
		** \param job The job to add
		*/
		void add(const Yuni::Job::IJob::Ptr& job);

		/*!
		** \brief Add a job into the waiting room
		**
		** The job will see its state changed to `stateWaiting`.
		** \param job The job to add
		** \param priority Its priority
		*/
		void add(const Yuni::Job::IJob::Ptr& job, Yuni::Job::Priority priority);

		/*!
		** \brief Get the next job to execute for a given priority
		**
		** \param[out] out Job to execute, if any. It will remain untouched if
		**   no job can be found.
		** \param priority The priority queue where to look for
		** \return True if a job is actually available, false otherwise
		*/
		bool pop(Yuni::Job::IJob::Ptr& out, const Yuni::Job::Priority priority);

		/*!
		** \brief Get the next job to execute
		**
		** \param[out] out Job to execute, if any. It will remain untouched if
		**   no job can be found.
		** \return True if a job is actually available, false otherwise
		*/
		bool pop(Yuni::Job::IJob::Ptr& out);

		//! Get the number of jobs waiting to be executed
		uint size() const;

		//! Remove all job waiting
		void clear();
		//@}


	private:
		//! Number of job waiting to be executed
		Atomic::Int<32>  pJobCount;
		//! List of waiting jobs by priority
		std::deque<Yuni::Job::IJob::Ptr>  pJobs[priorityCount];
		//! Mutexes, by priority to reduce congestion
		Mutex pMutexes[priorityCount];

	}; // class WaitingRoom






} // namespace QueueService
} // namespace Private
} // namespace Yuni

#include "waitingroom.hxx"
