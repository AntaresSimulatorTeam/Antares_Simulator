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
#include "../thread/utility.h"
#include "../datetime/timestamp.h"
#ifdef YUNI_HAS_CPP_MOVE
# include <utility>
#endif



namespace Yuni
{

	class EveryTimer final : public Thread::Timer
	{
	public:
		EveryTimer(uint ms, const Bind<bool ()>& callback) :
			Thread::Timer(ms),
			pCallback(callback)
		{}

		virtual ~EveryTimer()
		{
			// mandatory, the user is unlikely to call it when using 'every'
			if (started())
			{
				gracefulStop();
				wait(); // wait indefinitively if needed
			}
		}

	protected:
		virtual bool onInterval(uint /*cycle*/) override
		{
			return pCallback();
		}

	private:
		Bind<bool ()> pCallback;
	};



	Thread::Timer::Ptr  every(uint ms, const Bind<bool ()>& callback, bool autostart)
	{
		# ifdef YUNI_HAS_CPP_MOVE
		Thread::Timer* timer = new EveryTimer(ms, std::move(callback));
		# else
		Thread::Timer* timer = new EveryTimer(ms, callback);
		# endif
		if (autostart)
			timer->start();
		return timer;
	}





	template<bool PreciseT>
	class EveryTimerElapsed final : public Thread::Timer
	{
	public:
		EveryTimerElapsed(uint ms, const Bind<bool (uint64)>& callback) :
			Thread::Timer(ms),
			pCallback(callback)
		{
			pLastTimestamp = DateTime::NowMilliSeconds();
		}

		virtual ~EveryTimerElapsed()
		{
			// mandatory, the user is unlikely to call it when using 'every'
			stop();
		}

	protected:
		virtual bool onInterval(uint /*cycle*/) override
		{
			if (PreciseT)
			{
				// current timestamp in ms
				auto now = DateTime::NowMilliSeconds();

				// callback
				bool shouldContinue = pCallback((uint64)(now - pLastTimestamp));

				// fetch again the current to avoid taking into consideration
				// the time spent in the callback
				pLastTimestamp = DateTime::NowMilliSeconds();

				return shouldContinue;
			}
			else
			{
				// current timestamp in ms
				auto now = DateTime::NowMilliSeconds();
				uint64 elapsed = (uint64) (now - pLastTimestamp);
				pLastTimestamp = now;

				return pCallback(elapsed);
			}
		}


	private:
		sint64 pLastTimestamp;
		Bind<bool (uint64)> pCallback;
	};



	Thread::Timer::Ptr  every(uint ms, bool precise, const Bind<bool (uint64)>& callback, bool autostart)
	{
		Thread::Timer* timer;

		if (precise)
		{
			# ifdef YUNI_HAS_CPP_MOVE
			timer = new EveryTimerElapsed<true>(ms, std::move(callback));
			# else
			timer = new EveryTimerElapsed<true>(ms, callback);
			# endif
		}
		else
		{
			# ifdef YUNI_HAS_CPP_MOVE
			timer = new EveryTimerElapsed<false>(ms, std::move(callback));
			# else
			timer = new EveryTimerElapsed<false>(ms, callback);
			# endif
		}

		if (autostart)
			timer->start();
		return timer;
	}




} // namespace Yuni

