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
#include "smartptr.h"



namespace Yuni
{

	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline T* SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::WeakPointer(const SmartPtr& ptr)
	{
		return storagePointer(ptr);
	}


	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline T* SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::WeakPointer(T* wptr)
	{
		return wptr;
	}



	// Default constructor
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr()
	{
		// Checking if a null value is allowed, or not
		CheckingPolicy::onDefault(storagePointer(*this));
	}


	// Explicit
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::ExplicitStoredType ptr) :
		StoragePolicy(ptr)
	{
		// Check if the checking policy allows the given value
		CheckingPolicy::onInit(storagePointer(*this));
		OwnershipPolicy::initFromRawPointer(ptr);
	}


	// Implicit
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::ImplicitStoredType ptr) :
		StoragePolicy(ptr)
	{
		// Check if the checking policy allows the given value
		CheckingPolicy::onInit(storagePointer(*this));
		OwnershipPolicy::initFromRawPointer(ptr);
	}


	// Null
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(const NullPtr&) :
		StoragePolicy(NULL)
	{
		CheckingPolicy::onInit(storagePointer(*this));
	}


	// Copy constructor
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::CopyType& rhs) :
		StoragePolicy(rhs),
		OwnershipPolicy(rhs),
		CheckingPolicy(rhs),
		ConversionPolicy(rhs)
	{
		// Cloning the stored data
		storageReference(*this) = OwnershipPolicy::clone(storageReference(rhs));
	}


	// Copy constructor
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) :
		StoragePolicy(rhs),
		OwnershipPolicy(rhs),
		CheckingPolicy(rhs),
		ConversionPolicy(rhs)
	{
		// Cloning the stored data
		storageReference(*this) = OwnershipPolicy::clone(storageReference(rhs));
	}


	// Copy constructor
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) :
		StoragePolicy(rhs),
		OwnershipPolicy(rhs),
		CheckingPolicy(rhs),
		ConversionPolicy(rhs)
	{
		// Cloning the stored data
		storageReference(*this) = OwnershipPolicy::clone(storageReference(rhs));
	}


	// Copy constructor
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs,
		const typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::DynamicCastArg&) :
		StoragePolicy(rhs),
		OwnershipPolicy(rhs),
		CheckingPolicy(rhs),
		ConversionPolicy(rhs)
	{
		// Cloning the stored data
		storageReference(*this) = OwnershipPolicy::clone(dynamic_cast<typename StoragePolicy::StoredType>(storageReference(rhs)));
	}



	// Move constructor
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::SmartPtr(Static::MoveConstructor<SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP> > rhs) :
		StoragePolicy(rhs),
		OwnershipPolicy(rhs),
		CheckingPolicy(rhs),
		ConversionPolicy(rhs)
	{}


	// Destructor
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::~SmartPtr()
	{
		// Shall we destroy the data ?
		if (OwnershipPolicy::release(storagePointer(*static_cast<StoragePolicy*>(this))))
		{
			// Really destroy the data
			StoragePolicy::destroy();
		}
	}


	// Operator =
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>&
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator = (typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::CopyType& rhs)
	{
		SmartPtr tmp(rhs);
		tmp.swap(*this);
		return *this;
	}

	// Operator =
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>&
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator = (const NullPtr&)
	{
		SmartPtr tmp;
		tmp.swap(*this);
		return *this;
	}



	// Operator =
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>&
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator = (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs)
	{
		SmartPtr tmp(rhs);
		tmp.swap(*this);
		return *this;
	}


	// Operator =
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>&
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator = (SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs)
	{
		SmartPtr tmp(rhs);
		tmp.swap(*this);
		return *this;
	}


	// Operator =
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline void
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::swap(SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>& rhs)
	{
		OwnershipPolicy::swapPointer(rhs);
		ConversionPolicy::swapPointer(rhs);
		CheckingPolicy::swapPointer(rhs);
		StoragePolicy::swapPointer(rhs);
	}


	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::PointerType
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::pointer()
	{
		CheckingPolicy::onDereference(storageReference(*this));
		return StoragePolicy::operator -> ();
	}

	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::ConstPointerType
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::pointer() const
	{
		CheckingPolicy::onDereference(storageReference(*this));
		return StoragePolicy::operator -> ();
	}


	// Operator ->
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::ConstPointerType
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator -> () const
	{
		CheckingPolicy::onDereference(storageReference(*this));
		return StoragePolicy::operator -> ();
	}


	// Operator ->
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::PointerType
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator -> ()
	{
		CheckingPolicy::onDereference(storageReference(*this));
		return StoragePolicy::operator -> ();
	}


	// Operator ->
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::ConstReferenceType
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator * () const
	{
		CheckingPolicy::onDereference(storageReference(*this));
		return StoragePolicy::operator * ();
	}


	// Operator ->
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline typename SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::ReferenceType
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator * ()
	{
		CheckingPolicy::onDereference(storageReference(*this));
		return StoragePolicy::operator * ();
	}


	// Operator ->
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator ! () const
	{
		return storagePointer(*this) == NULL;
	}


	// Operator ==
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator == (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const
	{
		return (storagePointer(*this) == storagePointer(rhs));
	}


	// Operator ==
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator == (const T* rhs) const
	{
		return (storagePointer(*this) == rhs);
	}


	// Operator ==
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator == (const NullPtr&) const
	{
		return (storagePointer(*this) == NULL);
	}


	// Operator ==
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator != (const NullPtr&) const
	{
		return (storagePointer(*this) != NULL);
	}



	// Operator !=
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator != (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const
	{
		return (storagePointer(*this) != storagePointer(rhs));
	}


	// Operator !=
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator != (const T* rhs) const
	{
		return (storagePointer(*this) != rhs);
	}


	// Operator <
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator < (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const
	{
		return (storagePointer(*this) < storagePointer(rhs));
	}


	// Operator >
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator > (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const
	{
		return (storagePointer(*this) > storagePointer(rhs));
	}


	// Operator <=
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator <= (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const
	{
		return (storagePointer(*this) <= storagePointer(rhs));
	}


	// Operator >=
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<typename T1, template <class> class OwspP1, template <class> class ChckP1,
		class ConvP1,
		template <class> class StorP1, template <class> class ConsP1>
	inline bool
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::operator >= (const SmartPtr<T1,OwspP1,ChckP1,ConvP1,StorP1,ConsP1>& rhs) const
	{
		return (storagePointer(*this) >= storagePointer(rhs));
	}


	// Operator >
	template<typename T, template <class> class OwspP, template <class> class ChckP,
		class ConvP,
		template <class> class StorP, template <class> class ConsP>
	template<class S1>
	inline S1
	SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP>::DynamicCast(SmartPtr<T,OwspP,ChckP,ConvP,StorP,ConsP> rhs)
	{
		typedef typename S1::DynamicCastArg  DA;
		return S1(rhs, DA());
	}





} // namespace Yuni

