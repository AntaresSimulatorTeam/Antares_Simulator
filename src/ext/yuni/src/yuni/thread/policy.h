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

/*!
** \file
** \brief Threading policies
*/

# include "../yuni.h"
# ifdef YUNI_OS_WINDOWS
#	include "../core/system/windows.hdr.h"
# endif
# include "mutex.h"



namespace Yuni
{
namespace Policy
{

	/*!
	** \name Threading policies
	** \ingroup Policies
	*/
	//@{

	namespace // anonymous
	{

		/*!
		** \brief Helper class for retrieving the reference to the mutex in any situations
		*/
		template<class T>
		class MutexExtractor final
		{
		public:
			static Mutex& Reference(const T& rhs)
			{
				return const_cast<Mutex&>(rhs.pMutex);
			}
		};

		template<>
		class MutexExtractor<Yuni::Mutex> final
		{
		public:
			static Mutex& Reference(const Mutex& rhs)
			{
				return const_cast<Mutex&>(rhs);
			}
		};

	} // anonymous namespace





	/*!
	** \brief Implementation of the Threading Model policy in a single-threaded environnement
	** \ingroup Policies
	**
	** No synchronization is required in this case
	*/
	template<class Host>
	class SingleThreaded
	{
	public:
		//! The threading policy for another class
		template<class T> struct Class { typedef SingleThreaded<T>  Type; };

	public:
		/*!
		** \brief Locks a mutex in the constructor and unlocks it in the destructor.
		*/
		class MutexLocker final
		{
		public:
			MutexLocker() {}
			template<class C> MutexLocker(const C&) {}
			~MutexLocker() {}
		}; // class MutexLocker


		/*!
		** \brief A volative type
		*/
		template<typename U> struct Volatile { typedef U Type; };

		//! Get if this policy is thread-safe
		enum { threadSafe = false };

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		SingleThreaded() {}
		//! Copy constructor
		SingleThreaded(const SingleThreaded&) {}
		//! Destructor
		~SingleThreaded() {}
		//@}

	}; // class SingleThreaded



	/*!
	** \brief Implementation of the Threading Model policy in a multi-threaded environnement (one recursive mutex per object)
	**
	** The inner mutex is recursive.
	** \ingroup Policies
	*/
	template<class Host>
	class ObjectLevelLockable
	{
	public:
		//! The threading policy for another class
		template<class T> struct Class { typedef ObjectLevelLockable<T>  Type; };

	public:
		/*!
		** \brief Locks a mutex in the constructor and unlocks it in the destructor.
		*/
		class MutexLocker final
		{
		public:
			template<class C> MutexLocker(const C& h) :
				pHostToLock(MutexExtractor<C>::Reference(h))
			{
				pHostToLock.lock();
			}

			~MutexLocker()
			{
				pHostToLock.unlock();
			}

		private:
			Mutex& pHostToLock;
		}; // class MutexLocker

		/*!
		** \brief A volative type
		*/
		template<typename U> struct Volatile { typedef volatile U Type; };

		//! Get if this policy is thread-safe
		enum { threadSafe = true };

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		ObjectLevelLockable() : pMutex(true /*recursive*/) {}
		//! Copy constructor
		ObjectLevelLockable(const ObjectLevelLockable&) :pMutex(true /*recursive*/) {}
		//! Destructor
		~ObjectLevelLockable() {}
		//@}

	protected:
		//! Mutex
		mutable Mutex pMutex;
		template<class> friend class MutexExtractor;

	}; // class ObjectLevelLockable



	/*!
	** \brief Implementation of the Threading Model policy in a multi-threaded environnement (one mutex per object, not recursive)
	** \ingroup Policies
	*/
	template<class Host>
	class ObjectLevelLockableNotRecursive
	{
	public:
		//! The threading policy for another class
		template<class T> struct Class { typedef ObjectLevelLockableNotRecursive<T>  Type; };

	public:
		/*!
		** \brief Locks a mutex in the constructor and unlocks it in the destructor.
		*/
		class MutexLocker final
		{
		public:
			template<class C> MutexLocker(const C& h) :
				pHostToLock(MutexExtractor<C>::Reference(h))
			{
				pHostToLock.lock();
			}

			~MutexLocker()
			{
				pHostToLock.unlock();
			}

		private:
			Mutex& pHostToLock;
		}; // class MutexLocker

		/*!
		** \brief A volative type
		*/
		template<typename U> struct Volatile { typedef volatile U Type; };

		//! Get if this policy is thread-safe
		enum { threadSafe = true };

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		ObjectLevelLockableNotRecursive() : pMutex(false) {}
		//! Copy constructor
		ObjectLevelLockableNotRecursive(const ObjectLevelLockableNotRecursive&) : pMutex(false) {}
		//! Destructor
		~ObjectLevelLockableNotRecursive() {}
		//@}

	protected:
		//! Mutex
		mutable Mutex pMutex;
		template<class> friend class MutexExtractor;

	}; // class ObjectLevelLockableNotRecursive




	/*!
	** \brief Implementation of the Threading Model policy in a multi-threaded environnement (one mutex per class)
	** \ingroup Policies
	*/
	template<class Host>
	class ClassLevelLockable
	{
	public:
		//! The threading policy for another class
		template<class T> struct Class { typedef ClassLevelLockable<T>  Type; };

	public:
		/*!
		** \brief Locks a mutex in the constructor and unlocks it in the destructor.
		*/
		class MutexLocker final
		{
		public:
			MutexLocker()
			{
				Mutex::ClassLevelLockable<Host>::mutex.lock();
			}
			MutexLocker(const MutexLocker&) {}
			template<class C> MutexLocker(const C&)
			{
				Mutex::ClassLevelLockable<Host>::mutex.lock();
			}
			~MutexLocker()
			{
				Mutex::ClassLevelLockable<Host>::mutex.unlock();
			}
		}; // class MutexLocker

		/*!
		** \brief A volative type
		*/
		template<typename U> struct Volatile { typedef volatile U Type; };

		//! Get if this policy is thread-safe
		enum { threadSafe = true };

	public:
		//! \name Constructor & Destructor
		//@{
		//! Default constructor
		ClassLevelLockable() :pMutex(Mutex::ClassLevelLockable<Host>::mutex) {}
		//! Copy constructor
		ClassLevelLockable(const ClassLevelLockable&) :pMutex(Mutex::ClassLevelLockable<Host>::mutex) {}
		//! Destructor
		~ClassLevelLockable() {}
		//@}

	private:
		//! Mutex
		Mutex& pMutex;

	}; // class ClassLevelLockable



	//@} // Threading Policies




} // namespace Policy
} // namespace Yuni

