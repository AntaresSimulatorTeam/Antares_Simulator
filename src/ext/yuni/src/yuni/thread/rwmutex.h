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
#include "mutex.h"
#include "semaphore.h"




namespace Yuni
{

	/*!
	** \brief Read/Write Mutex (allows several readers but one writer)
	**
	** \ingroup Threads
	*/
	class YUNI_DECL RWMutex final : public NonMovable<RWMutex>
	{
	public:
		/*!
		** \brief A class-level locking mechanism
		**
		** A class-level locking operation locks all objects in a given class during that operation
		*/
		template<class T>
		class ClassLevelLockable
		{
		public:
			//! A dedicated mutex for the class T
			static RWMutex mutex;

		}; // class ClassLevelLockable


	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Default constructor
		*/
		explicit RWMutex(uint maxReaders = 64); // arbitrary limit
		/*!
		** \brief Copy constructor
		**
		** This constructor does actually nothing but it allows the compilation
		** of other classes which would implement a copy constructor
		*/
		RWMutex(const RWMutex&);
		//@}


		//! \name Read Lock & Unlock
		//@{
		/*!
		** \brief Lock the mutex for read access
		*/
		void readLock();

		/*!
		** \brief Release the lock for read access
		*/
		void readUnlock();
		//@}


		//! \name Write Lock & Unlock
		//@{
		/*!
		** \brief Lock the mutex for write access
		*/
		void writeLock();

		/*!
		** \brief Release the lock for write access
		*/
		void writeUnlock();
		//@}


		//! \name Operators
		//@{
		//! Operator = (do nothing)
		RWMutex& operator = (const RWMutex&);
		//@}


	private:
		# ifndef YUNI_NO_THREAD_SAFE
		//! Semaphore
		Semaphore pSemaphore;
		//! Real mutex
		Mutex pMutex;
		# endif

	}; // class RWMutex




	/*!
	** \brief Locks a mutex for read access in the constructor and unlocks it in the destructor (RAII).
	*/
	class ReadMutexLocker final
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		**
		** \param m The mutex to lock
		*/
		ReadMutexLocker(RWMutex& m);
		//! Destructor
		~ReadMutexLocker();
		//@}

	private:
		//! Reference to the real mutex
		RWMutex& pMutex;

	}; // ReadMutexLocker


	/*!
	** \brief Locks a mutex for write access in the constructor and unlocks it in the destructor (RAII).
	*/
	class WriteMutexLocker final
	{
	public:
		//! \name Constructor & Destructor
		//@{
		/*!
		** \brief Constructor
		**
		** \param m The mutex to lock
		*/
		WriteMutexLocker(RWMutex& m);
		//! Destructor
		~WriteMutexLocker();
		//@}

	private:
		//! Reference to the real mutex
		RWMutex& pMutex;

	}; // WriteMutexLocker




	//! All mutexes for each class
	template<class T> RWMutex RWMutex::ClassLevelLockable<T>::mutex;




} // namespace Yuni

#include "rwmutex.hxx"
