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
#include "../../yuni.h"
#include "../../thread/policy.h"
#include "../noncopyable.h"
#include "policies/creation.h"
#include "policies/lifetime.h"


namespace Yuni
{

	/*!
	** \brief Holder for a singleton class
	**
	** Manages creation, deletion and access in a MT environment
	**
	** For protection, some operations will fail at compile time.
	** For example, supposing MySingleton inherits Singleton<> :
	** \code
	** int main(void)
	** {
	**     MySingleton& instance = MySingleton::Instance();
	**     // Here we viciously try to delete the instance
	**     delete &instance;
	**     return 0;
	** }
	** \endcode
	*/
	template <class T,
		template <class> class CreationT = Policy::Creation::EmptyConstructor,
		template <class> class LifetimeT = Policy::Lifetime::Normal,
		template <class> class ThreadingT = Policy::ClassLevelLockable>
	class Singleton :
		public ThreadingT<Singleton<T, CreationT, LifetimeT, ThreadingT> >,
		private NonCopyable<Singleton<T, CreationT, LifetimeT, ThreadingT> >
	{
	public:
		//! Stored singleton type
		typedef T StoredType;
		//! Creation policy
		typedef CreationT<T> CreationPolicy;
		//! Lifetime policy
		typedef LifetimeT<T> LifetimePolicy;
		//! Threading policy
		typedef ThreadingT<Singleton<T, CreationT, LifetimeT, ThreadingT> > ThreadingPolicy;
		//! Type as stored in the singleton (volatile if necessary)
		typedef T& Reference;
		//! Volatile pointer
		typedef typename ThreadingPolicy::template Volatile<T*>::Type VolatilePtr;

	public:
		/*!
		** \brief Get the instance of this singleton
		*/
		static Reference Instance();

	public:
		/*!
		** \brief Address-of operator will fail at link time
		*/
		Singleton<T, CreationT, LifetimeT, ThreadingT>* operator & ();

		/*!
		** \brief Address-of operator will fail at link time
		*/
		const Singleton* operator & () const;


	protected:
		/*!
		** \brief Protected constructor !
		*/
		Singleton();

	private:
		//! Destroy the stored instance
		static void DestroyInstance();

	private:
		//! Unique instance of the class
		static VolatilePtr pInstance;
		//! Has the instance already been destroyed once ?
		static bool pDestroyed;

	}; // class Singleton






} // namespace Yuni

#include "singleton.hxx"
