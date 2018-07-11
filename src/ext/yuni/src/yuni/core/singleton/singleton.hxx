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
#include "singleton.h"
#include "../static/assert.h"



namespace Yuni
{

	template <typename T,
		template <class> class CreationT,
		template <class> class LifetimeT,
		template <class> class ThreadingT>
	typename Singleton<T, CreationT, LifetimeT, ThreadingT>::VolatilePtr
	Singleton<T, CreationT, LifetimeT, ThreadingT>::pInstance = NULL;


	template <typename T,
		template <class> class CreationT,
		template <class> class LifetimeT,
		template <class> class ThreadingT>
	bool Singleton<T, CreationT, LifetimeT, ThreadingT>::pDestroyed = false;







	template <typename T,
		template <class> class CreationT,
		template <class> class LifetimeT,
		template <class> class ThreadingT>
	inline Singleton<T, CreationT, LifetimeT, ThreadingT>::Singleton()
	{}


	template <typename T,
		template <class> class CreationT,
		template <class> class LifetimeT,
		template <class> class ThreadingT>
	typename Singleton<T, CreationT, LifetimeT, ThreadingT>::Reference
	Singleton<T, CreationT, LifetimeT, ThreadingT>::Instance()
	{
		// Double-checked locking pattern
		// Avoids locking on each call, only tests twice on first call
		if (!pInstance)
		{
			typename ThreadingPolicy::MutexLocker lock;
			if (!pInstance)
			{
				// If the instance was destroyed, the lifetime policy
				// is responsible for what to do.
				if (pDestroyed)
				{
					// Manage dead reference
					LifetimePolicy::OnDeadReference();
					// Mark the instance as valid again (e.g. for Phoenix singleton)
					pDestroyed = false;
				}
				// Create the instance
				pInstance = CreationPolicy::Create();
				// Let the lifetime policy manage when to destroy
				LifetimePolicy::ScheduleDestruction(&DestroyInstance);
			}
		}
		return *pInstance;
	}



	template <typename T,
		template <class> class CreationT,
		template <class> class LifetimeT,
		template <class> class ThreadingT>
	void Singleton<T, CreationT, LifetimeT, ThreadingT>::DestroyInstance()
	{
		CreationPolicy::Destroy(pInstance);
		pInstance = NULL;
		pDestroyed = true;
	}





} // namespace Yuni
