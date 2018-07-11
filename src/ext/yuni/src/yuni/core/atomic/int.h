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
#include "../system/windows.hdr.h"
#include "traits.h"




namespace Yuni
{
/*!
** \brief Atomic types
** \ingroup Atomic
*/
namespace Atomic
{

	/*!
	** \brief An atomic scalar type
	** \ingroup Atomic
	**
	** An atomic scalar value is a value that may be updated atomically (means
	** without the use of a mutex).
	**
	** \code
	** Atomic::Int<32> i;
	** ++i;
	** \endcode
	**
	** \tparam Size Size (in bits) of the scalar type (16 or 32 or 64)
	** \tparam TP A threading policy to guarantee thread-safety or not
	*/
	template<
		int Size = 8 * sizeof(void*), /* 32 or 64Bits */        // Size in Bits of the scalar type
		template<class> class TP = Policy::ObjectLevelLockable  // The threading policy
		>
	class YUNI_DECL Int final YUNI_ATOMIC_INHERITS
	{
	public:
		enum
		{
			//! Get if the class must be thread-safe
			threadSafe = TP<Int<Size,TP> >::threadSafe,
		};
		enum
		{
			//! Get if we have to guarantee ourselves the thread-safety
			useMutex  = YUNI_ATOMIC_MUST_USE_MUTEX,
			//! Get if we have a boolean type
			isBoolean = (Size == 1),
		};

		/*!
		** \brief The Threading Policy
		**
		** The threading policy will be Policy::SingleThreaded in all cases, except
		** when the compiler or the operating system can not provide methods
		** on atomic scalar types. It is the case when the version of gcc is < 4.1
		** for example.
		*/
		typedef typename Private::AtomicImpl::ThreadingPolicy<threadSafe,Int<Size,TP> >::Type  ThreadingPolicy;
		//! The scalar type
		typedef typename Private::AtomicImpl::TypeFromSize<Size>::Type  Type;
		//! The scalar type
		typedef Type  ScalarType;

		enum
		{
			//! Size (in bits) of the scalar type
			size = Private::AtomicImpl::TypeFromSize<Size>::size
		};

		/*!
		** \brief Type of the inner variable
		**
		** Most of the time the keyword `volatile` is required (to avoid dangerous
		** optimizations by the compiler), except when there is no need for
		** thread-safety or when a mutex is used.
		*/
		typedef typename Private::AtomicImpl::Volatile<threadSafe,Type>::Type InnerType;


	public:
		//! \name Constructors
		//@{
		/*!
		** \brief Default Constructor
		*/
		Int();
		/*!
		** \brief Constructor with an initial value (int16)
		*/
		Int(sint16 v);
		/*!
		** \brief Constructor with an initial value (int32)
		*/
		Int(sint32 v);
		/*!
		** \brief Constructor with an initial value (int64)
		*/
		Int(sint64 v);

		/*!
		** \brief Copy constructor
		*/
		Int(const Int& v);
		/*!
		** \brief Copy constructor from another type and another threading policy
		*/
		template<int Size2, template<class> class TP2>
		Int(const Int<Size2,TP2>& v);
		//@}


		//! \name Convenient routines
		//@{
		//! Reset to zero
		void zero();
		//@}


		//! \name Operators
		//@{
		//! Pre increment operator
		ScalarType operator ++ ();
		//! Pre decrement operator
		ScalarType operator -- ();
		//! Post increment operator
		ScalarType operator ++ (int);
		//! Post decrement operator
		ScalarType operator -- (int);

		Int& operator = (const ScalarType& v);

		//! Increment
		Int& operator += (const ScalarType& v);
		//! Decrement
		Int& operator -= (const ScalarType& v);

		//! Cast operator
		operator ScalarType () const;

		//! not
		bool operator ! () const;
		//@}


	private:
		//! The real variable
		mutable InnerType pValue;
		// A friend !
		template<int, template<class> class> friend struct Yuni::Private::AtomicImpl::Operator;

	}; // class Int




	//! Convenient typedef for atomic int32
	typedef Atomic::Int<32>  Int32;

	//! Convenient typedef for atomic int64
	typedef Atomic::Int<64>  Int64;




} // namespace Atomic
} // namespace Yuni

#include "int.hxx"
#include "string.hxx"
