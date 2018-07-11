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
#include "../static/if.h"
#ifdef YUNI_OS_WINDOWS
#	include "../system/windows.hdr.h"
#endif


// Determine if we must use a mutex or not
#if defined(YUNI_OS_WINDOWS) || YUNI_OS_GCC_VERSION >= 40102 || defined(YUNI_OS_CLANG)
#	if !defined(YUNI_OS_WINDOWS) && !defined(YUNI_HAS_SYNC_ADD_AND_FETCH)
#		define YUNI_ATOMIC_MUST_USE_MUTEX 1
#	else
#		define YUNI_ATOMIC_MUST_USE_MUTEX 0
#	endif
#else
#	define YUNI_ATOMIC_MUST_USE_MUTEX 1
#endif

#if YUNI_ATOMIC_MUST_USE_MUTEX == 1
#	define YUNI_ATOMIC_INHERITS  : public TP<Int<Size,TP> >
#else
#	define YUNI_ATOMIC_INHERITS
#endif



namespace Yuni
{
namespace Atomic
{

	// Forward declaration
	template<int Size, template<class> class TP> class Int;


} // namespace Atomic
} // namespace Yuni



namespace Yuni
{
namespace Private
{
namespace AtomicImpl
{


	template<int ThreadSafe, class C>
	struct ThreadingPolicy final
	{
		#if YUNI_ATOMIC_MUST_USE_MUTEX == 1
		// If the class must be thread-safe, we have to provide a lock
		// mecanism to ensure thread-safety
		typedef typename Static::If<ThreadSafe,
			Policy::ObjectLevelLockable<C>, Policy::SingleThreaded<C> >::ResultType Type;
		# else
		// No lock is required, the operating system or the compiler already
		// provides all we need
		typedef Policy::SingleThreaded<C> Type;
		#endif

	}; // class ThreadingPolicy




	template<int ThreadSafe, class T>
	struct Volatile final
	{
		#if YUNI_ATOMIC_MUST_USE_MUTEX == 1
		// We have to use our own mutex, we don't care of the volatile keyword
		typedef T Type;
		# else
		// The operating system or the compiler already provides methods
		// to deal with atomic types. However the volatile keyword is
		// required to avoid dangerous optimizations by the compiler
		// when the class must be thread-safe (to avoid cache-optimisations
		// SMP processors for example)
		typedef typename Static::If<ThreadSafe, volatile T, T>::ResultType Type;
		#endif

	}; // class Volatile




	template<int Size>
	struct TypeFromSize final {};


	// bool
	template<> struct TypeFromSize<1> final
	{
		// We should use a signed 32 bits integer for boolean
		enum { size = 32 };
		typedef sint32 Type;
	};

	// Int16
	template<> struct TypeFromSize<16> final
	{
		// On OS X, there are only routines for int32_t and int64_t
		// With MinGW, it simply does not exist
		// It seems that the best solution is to use int32 everywhere
		enum { size = 32 };
		typedef sint32 Type;
	};

	// Int32
	template<> struct TypeFromSize<32> final
	{
		enum { size = 32 };
		typedef sint32 Type;
	};

	// Int64
	template<> struct TypeFromSize<64> final
	{
		enum { size = 64 };
		typedef sint64 Type;
	};




	// Thread-safe operations
	template<int Size, template<class> class TP>
	struct Operator final {};


	template<template<class> class TP>
	struct Operator<32, TP> final
	{
		template<class T>
		static typename Yuni::Atomic::Int<32,TP>::Type Increment(T& t)
		{
			#ifdef YUNI_OS_WINDOWS
			return ::InterlockedIncrement((LONG*)&t.pValue);
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (++t.pValue);
			#	else
			return __sync_add_and_fetch(&t.pValue, 1);
			#	endif
			#endif
		}

		template<class T>
		static typename Yuni::Atomic::Int<32,TP>::Type Increment(const T& t, typename T::ScalarType value)
		{
			#ifdef YUNI_OS_WINDOWS
			return InterlockedExchange((LONG*)&t.pValue, (LONG)(t.pValue + value));
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (t.pValue += value);
			#	else
			return __sync_add_and_fetch(&t.pValue, value);
			#	endif
			#endif
		}

		template<class T>
		static typename Yuni::Atomic::Int<32,TP>::Type Decrement(T& t)
		{
			#ifdef YUNI_OS_WINDOWS
			#   ifdef YUNI_OS_MINGW
			return ::InterlockedDecrement((LONG*)&t.pValue);
			#   else
			return _InterlockedDecrement((LONG*)&t.pValue);
			#   endif
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (--t.pValue);
			#	else
			return __sync_add_and_fetch(&t.pValue, -1);
			#	endif
			#endif
		}

		template<class T>
		static typename Yuni::Atomic::Int<32,TP>::Type Decrement(T& t, typename T::ScalarType value)
		{
			#ifdef YUNI_OS_WINDOWS
			return InterlockedExchange((LONG*)&t.pValue, (LONG)(t.pValue - value));
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (t.pValue -= value);
			#	else
			return __sync_add_and_fetch(&t.pValue, -value);
			#	endif
			#endif
		}

		template<class T>
		static void Zero(T& t)
		{
			#ifdef YUNI_OS_WINDOWS
			::InterlockedExchange((LONG*)&t.pValue, 0);
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			t.pValue = 0;
			#	else
			__sync_and_and_fetch(&t.pValue, 0);
			#	endif
			#endif
		}

		template<class T>
		static void Set(T& t, sint32 newvalue)
		{
			#ifdef YUNI_OS_WINDOWS
			::InterlockedExchange((LONG*)&t.pValue, newvalue);
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			t.pValue = newvalue;
			#	else
			__sync_synchronize();
			t.pValue = newvalue;
			#	endif
			#endif
		}

	}; // class Operator<32, TP>





	template<template<class> class TP>
	struct Operator<64, TP> final
	{
		template<class T>
		static typename Yuni::Atomic::Int<64,TP>::Type Increment(T& t)
		{
			#ifdef YUNI_OS_WINDOWS
			#	ifdef YUNI_OS_MINGW32
			YUNI_STATIC_ASSERT(false, AtomicOperator_NotImplementedWithMinGW);
			#	else
			return _InterlockedIncrement64((LONGLONG*)&t.pValue);
			#	endif
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (++t.pValue);
			#	else
			return __sync_add_and_fetch(&t.pValue, 1);
			#	endif
			#endif
		}

		template<class T>
		static typename Yuni::Atomic::Int<64,TP>::Type Increment(const T& t, typename T::ScalarType value)
		{
			#ifdef YUNI_OS_WINDOWS
			#	ifdef YUNI_OS_MINGW32
			YUNI_STATIC_ASSERT(false, AtomicOperator_NotImplementedWithMinGW);
			#	else
			return InterlockedExchange64((LONGLONG*)&t.pValue, (LONGLONG)(t.pValue + value));
			#   endif
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (t.pValue += value);
			#	else
			return __sync_add_and_fetch(&t.pValue, value);
			#	endif
			#endif
		}

		template<class T>
		static typename Yuni::Atomic::Int<64,TP>::Type Decrement(T& t)
		{
			#ifdef YUNI_OS_WINDOWS
			#	ifdef YUNI_OS_MINGW32
			YUNI_STATIC_ASSERT(false, AtomicOperator_NotImplementedWithMinGW);
			#	else
			return _InterlockedDecrement64((LONGLONG*)&t.pValue);
			#	endif
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (--t.pValue);
			#	else
			return __sync_add_and_fetch(&t.pValue, -1);
			#	endif
			#endif
		}

		template<class T>
		static typename Yuni::Atomic::Int<64,TP>::Type Decrement(T& t, typename T::ScalarType value)
		{
			#ifdef YUNI_OS_WINDOWS
			#	ifdef YUNI_OS_MINGW32
			YUNI_STATIC_ASSERT(false, AtomicOperator_NotImplementedWithMinGW);
			#	else
			return InterlockedExchange64((LONGLONG*)&t.pValue, (LONGLONG)(t.pValue - value));
			#   endif
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			return (t.pValue -= value);
			#	else
			return __sync_add_and_fetch(&t.pValue, -value);
			#	endif
			#endif
		}

		template<class T>
		static void Zero(T& t)
		{
			#ifdef YUNI_OS_WINDOWS
			#	ifdef YUNI_OS_MINGW32
			YUNI_STATIC_ASSERT(false, AtomicOperator_NotImplementedWithMinGW);
			#	else
			::InterlockedExchange64((LONGLONG*)&t.pValue, 0);
			#   endif
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			t.pValue = 0;
			#	else
			__sync_and_and_fetch(&t.pValue, 0);
			#	endif
			#endif
		}

		template<class T>
		static void Set(T& t, sint64 newvalue)
		{
			#ifdef YUNI_OS_WINDOWS
			#	ifdef YUNI_OS_MINGW32
			YUNI_STATIC_ASSERT(false, AtomicOperator_NotImplementedWithMinGW);
			#	else
			::InterlockedExchange64((LONGLONG*)&t.pValue, newvalue);
			#   endif
			# else
			#	if YUNI_ATOMIC_MUST_USE_MUTEX == 1
			typename T::ThreadingPolicy::MutexLocker locker(t);
			t.pValue = newvalue;
			#	else
			__sync_synchronize();
			t.pValue = newvalue;
			#	endif
			#endif
		}

	}; // class Operator<64, TP>





} // namespace AtomicImpl
} // namespace Private
} // namespace Yuni
