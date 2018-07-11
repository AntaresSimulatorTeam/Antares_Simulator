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
#include "job.h"



namespace Yuni
{
namespace Job
{

	inline enum Job::State IJob::state() const
	{
		return (enum Job::State) ((sint32) pState);
	}


	inline bool IJob::idle() const
	{
		return (0 != (pState & stateIdle));
	}


	inline bool IJob::waiting() const
	{
		return (0 != (pState & stateWaiting));
	}


	inline bool IJob::running() const
	{
		return (0 != (pState & stateRunning));
	}


	inline void IJob::cancel()
	{
		pCanceling = true;
	}


	inline bool IJob::canceling() const
	{
		return (0 != pCanceling);
	}


	inline void IJob::progression(const int p)
	{
		pProgression = ((p < 0) ? 0 : (p > 100 ? 100 : p));
	}


	inline bool IJob::finished() const
	{
		// The state must be at the very end
		return (pProgression >= 100 and pState == stateIdle);
	}


	inline bool IJob::shouldAbort() const
	{
		assert(pThread != NULL and "Job: The pointer to the attached thread must not be NULL");
		return (pCanceling or pThread->shouldAbort());
	}


	template<class T>
	void IJob::fillInformation(T& info)
	{
		// The first important value is the state
		info.state = (Yuni::Job::State) ((int) (pState));
		// Then, if the job is canceling its work
		info.canceling = (0 != pCanceling);

		info.progression = pProgression;
		info.name = caption();
	}


	inline String IJob::caption() const
	{
		return nullptr;
	}





} // namespace Job
} // namespace Yuni
