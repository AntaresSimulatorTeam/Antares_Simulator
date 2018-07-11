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
#include "timer.h"
#include <cassert>


namespace Yuni
{
namespace Thread
{

	Timer::Timer() :
		IThread(),
		pTimeInterval(defaultInterval),
		pCycleCount(infinite)
	{}


	Timer::Timer(uint interval) :
		IThread(),
		pTimeInterval(interval),
		pCycleCount(infinite)
	{}


	Timer::Timer(uint interval, uint cycles) :
		IThread(),
		pTimeInterval(interval),
		pCycleCount(cycles)
	{}


	Timer::~Timer()
	{
		assert(started() == false);
	}


	Timer::Timer(const Timer& rhs) :
		IThread()
	{
		rhs.pTimerMutex.lock();
		pTimeInterval = rhs.pTimeInterval;
		pCycleCount = rhs.pCycleCount;
		rhs.pTimerMutex.unlock();
	}


	Timer& Timer::operator = (const Timer& rhs)
	{
		pTimerMutex.lock();
		rhs.pTimerMutex.lock();
		pTimeInterval = rhs.pTimeInterval;
		pCycleCount = rhs.pCycleCount;
		pTimerMutex.unlock();
		rhs.pTimerMutex.unlock();
		return *this;
	}


	bool Timer::internalRunInfiniteLoop()
	{
		uint nnTimeInterval = pTimeInterval;
		pTimerMutex.unlock();

		do
		{
			if (IThread::suspend(nnTimeInterval))
				break;
			if (not onInterval(infinite /* no cycle */))
				break;
			if (pShouldReload)
				return false;
		}
		while (true);

		return true;
	}


	bool Timer::internalRunFixedNumberOfCycles()
	{
		uint cycleIndex = 0;
		uint nnTimeInterval = pTimeInterval;
		pTimerMutex.unlock();

		do
		{
			// Wait then execute the timer
			if (suspend(nnTimeInterval) or not onInterval(cycleIndex))
				return true;
			if (++cycleIndex >= pCycleCount) // the maximum number of cycle is reached
				return true;
			if (pShouldReload)
				return false;
		}
		while (true);

		return true;
	}


	bool Timer::onExecute()
	{
		pShouldReload = false;
		do
		{
			// Note : pTimerMutex will be unlocked by internalRunInfiniteLoop
			// and internalRunFixedNumberOfCycles
			pTimerMutex.lock();

			// No cycle to do, aborting now
			if (0 == pCycleCount)
			{
				pTimerMutex.unlock();
				return false;
			}

			// infinite loop
			if (infinite == pCycleCount)
			{
				// pTimerMutex will be unlocked by internalRunInfiniteLoop
				if (internalRunInfiniteLoop())
				{
					// Stopping the thread
					return false;
				}
			}
			else
			{
				// pTimerMutex will be unlocked by internalRunFixesNumberOfCycles
				if (internalRunFixedNumberOfCycles())
				{
					// Stopping the thread
					return false;
				}
			}
		}
		while (true);

		return false;
	}


	void Timer::interval(uint milliseconds)
	{
		pTimerMutex.lock();
		pTimeInterval = milliseconds;
		pTimerMutex.unlock();
	}


	void Timer::cycleCount(uint n)
	{
		pTimerMutex.lock();
		pCycleCount = n;
		pTimerMutex.unlock();
	}


	void Timer::reload(uint milliseconds)
	{
		pTimerMutex.lock();
		pTimeInterval = milliseconds;
		pTimerMutex.unlock();
		pShouldReload = true;
	}


	void Timer::reload(uint milliseconds, uint cycles)
	{
		pTimerMutex.lock();
		pTimeInterval = milliseconds;
		pCycleCount   = cycles;
		pTimerMutex.unlock();
		pShouldReload = true;
	}




} // namespace Thread
} // namespace Yuni

