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
namespace Job
{

	/*!
	** \brief Set of possible states for a single job
	**
	** \ingroup Jobs
	*/
	enum State
	{
		//! The job does nothing (can be already done)
		stateIdle = 0,
		//! The job is waiting for being executed
		stateWaiting,
		//! The job is currently running
		stateRunning,
	};



	enum Priority
	{
		//! Low priority
		priorityLow = 0,
		//! Default priority
		priorityDefault = 1,
		//! High priority
		priorityHigh = 2,

		priorityCount = 3,
	};




} // namespace Job
} // namespace Yuni
