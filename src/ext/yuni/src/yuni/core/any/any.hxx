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
#include "any.h"




namespace Yuni
{

	template <typename T>
	Any::Any(const T& source)
	{
		pTable = Private::Any::Table<T>::Get();
		if (sizeof(T) <= sizeof(void*))
			new (&pObject) T(source);
		else
			pObject = new T(source);
	}


	template <typename T>
	Any& Any::assign(const T& rhs)
	{
		// Are we copying between the same type of variants ?
		Private::Any::TypeManipulationTable* rhsTable = Private::Any::Table<T>::Get();

		if (pTable == rhsTable)
		{
			// Yes, so we can avoid reallocating, and re-use memory.
			if (sizeof(T) <= sizeof(void*))
			{
				// Call the destructor on the object to clean up.
				reinterpret_cast<T*>(&pObject)->~T();
				// Create copy on-top of object pointer itself
				new (&pObject) T(rhs);
			}
			else
			{
				// Call the destructor on the old object, but do not deallocate memory !
				reinterpret_cast<T*>(pObject)->~T();
				// Create copy on-top of the old version
				new (pObject) T(rhs);
			}
		}
		else
		{
			// No, this was not the same type.
			reset();
			if (sizeof(T) <= sizeof(void*))
			{
				// Create copy on-top of object pointer itself
				new (&pObject) T(rhs);
				// Update table pointer
				pTable = rhsTable;
			}
			else
			{
				pObject = new T(rhs);
				pTable = rhsTable;
			}
		}
		return *this;
	}


	template <typename T>
	inline const T& Any::to() const
	{
		if (type() != typeid(T))
			throw Exceptions::BadCast(type(), typeid(T));

		if (sizeof(T) <= sizeof(void*))
			return *reinterpret_cast<T const*>(&pObject);

		return *reinterpret_cast<T const*>(pObject);
	}


	template <typename T>
	inline void Any::initFromCString(T source)
	{
		pTable = Private::Any::Table<String>::Get();
		if (sizeof(String) <= sizeof(void*))
			new (&pObject) String(source);
		else
			pObject = new String(source);
	}




} // namespace Yuni
