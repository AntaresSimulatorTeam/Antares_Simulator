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
#ifdef YUNI_HAS_CPP_MOVE
# include <utility>
#endif



namespace Yuni
{

	class SpawnThread final : public Thread::IThread
	{
	public:
		SpawnThread(const Bind<void ()>& callback) :
			pCallback(callback)
		{}

		virtual ~SpawnThread()
		{
			// mandatory, the user is unlikely to call it when using 'Async'
			if (started())
			{
				gracefulStop();
				wait(); // wait indefinitely if needed
			}
		}

	protected:
		virtual bool onExecute() override
		{
			pCallback();
			return false; // this thread is unlikely to be reused
		}

	private:
		Bind<void ()> pCallback;
	};




	Thread::IThread::Ptr  spawn(const Bind<void ()>& callback, bool autostart)
	{
		# ifdef YUNI_HAS_CPP_MOVE
		Thread::IThread* thread = new SpawnThread(std::move(callback));
		# else
		Thread::IThread* thread = new SpawnThread(callback);
		# endif

		if (autostart)
			thread->start();
		return thread;
	}




} // namespace Yuni

