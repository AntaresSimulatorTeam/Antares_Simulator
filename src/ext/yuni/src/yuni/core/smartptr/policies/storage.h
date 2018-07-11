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
** \brief Storage policies
*/


namespace Yuni
{
namespace Policy
{

/*!
** \brief Storage policies
** \ingroup Policies
*/
namespace Storage
{

	/*!
	** \brief Implementation of the default storage policy
	** \ingroup Policies
	**
	** \tparam T The data type
	*/
	template<class T>
	class Pointer
	{
	public:
		//! The type stored
		typedef T* StoredType;
		//! The type used by the operator ->
		typedef T* PointerType;
		//! The type used by the operator *
		typedef T& ReferenceType;

		//! The default value for this type
		static StoredType DefaultValue() {return nullptr;}

	public:
		//! \name Constructors
		//@{
		Pointer() :pData(DefaultValue()) {}
		Pointer(const Pointer&) :pData(0) {}
		template<class U> Pointer(const Pointer<U>&) :pData(0) {}
		Pointer(const StoredType& p) :pData(p) {}
		//@}

		//! Swap the data
		void swapPointer(Pointer& rhs) {std::swap(pData, rhs.pData);}

		//! \name Get the data
		//@{
		friend PointerType storagePointer(const Pointer& rhs) {return rhs.pData;}
		friend StoredType& storageReference(Pointer& rhs) {return rhs.pData;}
		friend const StoredType& storageReference(const Pointer& rhs) {return rhs.pData;}
		//@}

		//! \name Operators
		//@{
		//! The operator ->
		PointerType   operator -> () const {return pData;}
		//! The operator *
		ReferenceType operator * () const {return *pData;}
		//@}

	protected:
		//! Destroy the inner data
		inline void destroy()
		{
			// [from the standard]
			// If the object being deleted has incomplete class type at the point of deletion
			// and the complete class has a non-trivial destructor or a deallocation function,
			// the behavior is undefined.
			if (0 < sizeof (T)) // won't compile for incomplete type
				delete pData;
		}

	private:
		//! The data
		StoredType pData;

	}; // class Pointer





	/*!
	** \brief Implementation of the Array storage policy
	** \ingroup Policies
	**
	** \tparam T The data type
	*/
	template<class T>
	class Array
	{
	public:
		//! The type stored
		typedef T* StoredType;
		//! The type used by the operator ->
		typedef T* PointerType;
		//! The type used by the operator *
		typedef T& ReferenceType;

		//! The default value for this type
		static StoredType DefaultValue() {return nullptr;}

	public:
		//! \name Constructors
		//@{
		Array() :pData(DefaultValue()) {}
		Array(const Array&) :pData(0) {}
		template<class U> Array(const Pointer<U>&) :pData(0) {}
		Array(const StoredType& p) :pData(p) {}
		//@}

		//! Swap the data
		void swapPointer(Array& rhs) {std::swap(pData, rhs.pData);}

		//! \name Get the data
		//@{
		friend PointerType storagePointer(const Array& rhs) {return rhs.pData;}
		friend StoredType& storageReference(Array& rhs) {return rhs.pData;}
		friend const StoredType& storageReference(const Array& rhs) {return rhs.pData;}
		//@}

		//! \name Operators
		//@{
		//! The operator ->
		PointerType   operator -> () const {return pData;}
		//! The operator *
		ReferenceType operator * () const {return *pData;}
		//@}

	protected:
		//! Destroy the inner data
		inline void destroy()
		{
			// [from the standard]
			// If the object being deleted has incomplete class type at the point of deletion
			// and the complete class has a non-trivial destructor or a deallocation function,
			// the behavior is undefined.
			if (0 < sizeof (T)) // won't compile for incomplete type
				delete[] pData;
		}

	private:
		//! The data
		StoredType pData;

	}; // class Array





} // namespace Storage
} // namespace Policy
} // namespace Yuni

