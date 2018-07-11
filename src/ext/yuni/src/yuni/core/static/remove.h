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


namespace Yuni
{
namespace Static
{

/*!
** \brief Remove some part about a single type
*/
namespace Remove
{

	//! \name Reference
	//@{
	//! Remove the reference of a type (* or &)
	template <typename T> struct Reference { typedef T Type; };
	template <typename T> struct Reference<T&> { typedef T Type; };
	template <typename T> struct Reference<volatile T&> { typedef volatile T Type; };
	template <typename T> struct Reference<T*> { typedef T Type; };
	template <typename T> struct Reference<T* const> { typedef T Type; };
	template <typename T> struct Reference<volatile T*> { typedef volatile T Type; };
	template <typename T, int N> struct Reference<T[N]> { typedef T Type; };
	template <typename T> struct Reference<const T*> { typedef const T Type; };
	template <typename T> struct Reference<const T* const> { typedef const T Type; };
	template <typename T> struct Reference<const volatile T*> { typedef const volatile T Type; };
	template <typename T, int N> struct Reference<const T[N]> { typedef const T Type; };
	template <typename T> struct Reference<const T&> { typedef const T Type; };
	template <typename T> struct Reference<const volatile T&> { typedef const volatile T Type; };

	template <typename T> struct RefOnly { typedef T Type; };
	template <typename T> struct RefOnly<T&> { typedef T Type; };
	template <typename T> struct RefOnly<volatile T&> { typedef volatile T Type; };
	template <typename T> struct RefOnly<const T&> { typedef const T Type; };
	template <typename T> struct RefOnly<const volatile T&> { typedef const volatile T Type; };

	template <typename T> struct PntOnly { typedef T Type; };
	template <typename T> struct PntOnly<T*> { typedef T Type; };
	template <typename T> struct PntOnly<volatile T*> { typedef volatile T Type; };
	template <typename T> struct PntOnly<const T*> { typedef const T Type; };
	template <typename T> struct PntOnly<const T* const> { typedef const T Type; };
	template <typename T> struct PntOnly<T* const> { typedef const T Type; };
	template <typename T> struct PntOnly<const volatile T*> { typedef const volatile T Type; };
	//@}

	//! \name Volatile
	//@{
	template <typename T> struct Volatile { typedef T Type; };
	template <typename T> struct Volatile<volatile T*> { typedef T* Type; };
	template <typename T> struct Volatile<const volatile T*> { typedef const T* Type; };
	template <typename T> struct Volatile<const volatile T* const> { typedef const T* const Type; };
	template <typename T> struct Volatile<volatile T&> { typedef T& Type; };
	template <typename T> struct Volatile<const volatile T&> { typedef const T& Type; };
	//@}


	//! \name Const
	//@{
	//! Remove the "Const" part of a type
	template <typename T> struct Const { typedef T Type; };
	template <typename T> struct Const<T*> { typedef T* Type; };
	template <typename T> struct Const<T* const> { typedef T* Type; };
	template <typename T> struct Const<T&> { typedef T& Type; };
	template <typename T, int N> struct Const<T[N]> { typedef T Type[N]; };
	template <typename T> struct Const<const T>  { typedef T Type; };
	template <typename T> struct Const<const T*> { typedef T* Type; };
	template <typename T> struct Const<const T* const> { typedef T* Type; };
	template <typename T> struct Const<const T&> { typedef T& Type; };
	template <typename T, int N> struct Const<const T[N]> { typedef T Type[N]; };
	//@}

	template <typename T> struct All
	{
		typedef typename Remove::Volatile<
			typename Remove::Const<
				typename Remove::Reference<T>::Type >::Type>::Type   Type;
	};





} // namespace Remove
} // namespace Static
} // namespaec Yuni
