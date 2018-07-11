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
#include "job.h"
#include "../core/math.h"



namespace Yuni
{
namespace Job
{

	IJob::IJob()
		: pThread(nullptr)
	{}


	IJob::~IJob()
	{
		assert(this != NULL and "IJob: Destructor: Oo `this' is null !?");
		assert(pThread == NULL and "A job can not be attached to a thread when destroyed");
	}


	bool IJob::suspend(uint delay) const
	{
		// This method must only be called from a thread
		assert(pThread and "Job: The pointer to the attached thread must not be NULL");

		// We can suspend the job only if it is running
		if (pState == stateRunning)
		{
			// It is important (for thread-safety reasons) that this method
			// does not modify the state.
			// This may lead to unwanted behaviors.
			// Sleeping for a while...
			bool r = pThread->suspend(delay);
			// The state may have changed while we were sleeping
			return (pCanceling or r);
		}
		return true;
	}


	void IJob::execute(Thread::IThread* t)
	{
		// note: \p t can be null

		// Reseting data
		// We will keep the state in `waiting` until we have properly set
		// all other values
		pThread = t;
		pCanceling = false;
		pProgression = 0;

		// Here we go !
		pState = stateRunning;

		// Execute the specific implementation of the job
		try
		{
			onExecute();
		}
		catch (...)
		{}

		// The state must be reset to idle as soon as possible while the
		// other values are still valid.
		pState = stateIdle;

		// Other values
		pThread = nullptr;
		pProgression = 100;
	}





} // namespace Job
} // namespace Yuni
