/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

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
#include "remove.h"
#include "../smartptr/smartptr.h"

namespace Yuni::Static::Type
{
//! \name Fundamental types
//@{
/*!
** \brief Get if a type is fundamental
*/
template<typename T>
struct IsFundamental
{
    // Default to No
    enum
    {
        Yes = 0,
        No = 1
    };
};

//@}

//! \name SmartPtr
//@{
template<class T>
struct IsSmartPtr
{
    enum
    {
        Yes = 0,
        No = 1
    };
};

//@}

//! \name Small types (fits in a pointer size)
//@{
/*!
** \brief Get if a type size is <= to a pointer size
*/
template<typename T>
struct IsSmall
{
    enum
    {
        Yes = ((sizeof(T) <= sizeof(void*)) || IsSmartPtr<T>::Yes),
        No = !Yes,
    };
};

//! \name Compound types
//@{
/*!
** \brief Determine the compound type
*/
template<typename T>
struct Compound
{
    //! The original type
    typedef T BaseType;

    enum
    {
        IsPointer = 0,
        IsReference = 0,
        IsArray = 0,
        IsPtrMember = 0
    };
};

//@}

//! \name Constness
//@{
template<typename T>
struct IsConst
{
    enum
    {
        Yes = 0,
        No = 1
    };
};

//@}

//! \name Numeric types
//@{
template<typename T>
struct IsDecimal
{
    enum
    {
        Yes = 0,
        No = 1
    };
};

//@}

//! \name Comparison
//@{
/*!
** \brief Determine if two types are stricly identical
**
** \code
**	std::cout << StrictlyEqual<int, bool>::Yes << std::endl; // print 0
**	std::cout << StrictlyEqual<int, int>::Yes << std::endl; // print 1
**	std::cout << StrictlyEqual<int, int&>::Yes << std::endl; // print 0
**	std::cout << StrictlyEqual<int, const int>::Yes << std::endl; // print 0
** \endcode
*/
template<typename T, typename U>
struct StrictlyEqual
{
    // Two types are not equal by default
    enum
    {
        Yes = 0,
        No = 1
    };
};

/*!
** \brief Determine if two types (without any identifier) are identical
**
** \code
**	std::cout << Equal<int, bool>::Yes << std::endl; // print 0
**	std::cout << Equal<int, int>::Yes << std::endl; // print 1
**	std::cout << Equal<int, int&>::Yes << std::endl; // print 1
**	std::cout << Equal<int, const int>::Yes << std::endl; // print 1
** \endcode
*/
template<typename A, typename B>
struct Equal
{
    typedef typename Static::Remove::All<A>::Type BaseTypeA;
    typedef typename Static::Remove::All<B>::Type BaseTypeB;

    enum
    {
        Yes = StrictlyEqual < BaseTypeA,
        BaseTypeB > ::Yes,
        No = StrictlyEqual < BaseTypeA,
        BaseTypeB > ::No
    };
};

template<class T>
struct DefaultOrNull
{
    enum
    {
        Yes = 0,
        No = 1
    };
};

//@}

//! \name Overloads
//@{

// Those types are fundamental for sure
//
template<>
struct IsFundamental<bool>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<wchar_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<float>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<double>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<long double>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

// Signed int
#ifndef YUNI_TYPES_INT8_CHAR_ARE_IDENTICAL
template<>
struct IsFundamental<int8_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<int16_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};
#endif
template<>
struct IsFundamental<int32_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<int64_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

// Unsigned int
#ifndef YUNI_TYPES_INT8_CHAR_ARE_IDENTICAL
template<>
struct IsFundamental<uint8_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<uint16_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};
#endif
template<>
struct IsFundamental<uint32_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsFundamental<uint64_t>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<typename T>
struct IsFundamental<const T>
{
    enum
    {
        Yes = IsFundamental<T>::Yes,
        No = IsFundamental<T>::No
    };
};

template<typename T>
struct IsFundamental<T*>
{
    enum
    {
        Yes = IsFundamental<T>::Yes,
        No = IsFundamental<T>::No
    };
};

template<typename T>
struct IsFundamental<T&>
{
    enum
    {
        Yes = IsFundamental<T>::Yes,
        No = IsFundamental<T>::No
    };
};

template<typename T>
struct IsFundamental<const T*>
{
    enum
    {
        Yes = IsFundamental<T>::Yes,
        No = IsFundamental<T>::No
    };
};

template<typename T>
struct IsFundamental<const T&>
{
    enum
    {
        Yes = IsFundamental<T>::Yes,
        No = IsFundamental<T>::No
    };
};

// Specialization for references
template<typename T>
struct Compound<T&>
{
    enum
    {
        IsPointer = 0,
        IsReference = 1,
        IsArray = 0,
        IsPtrMember = 0
    };

    typedef typename Compound<T>::BaseType BaseType;
};

// Specialization for pointers
template<typename T>
struct Compound<T*>
{
    enum
    {
        IsPointer = 1,
        IsReference = 0,
        IsArray = 0,
        IsPtrMember = 0
    };

    typedef typename Compound<T>::BaseType BaseType;
};

// Specialization for empty arrays
template<typename T>
struct Compound<T[]>
{
    enum
    {
        IsPointer = 0,
        IsReference = 0,
        IsArray = 1,
        IsPtrMember = 0
    };

    typedef typename Compound<T>::BaseType BaseType;
};

// Specialization for arrays
template<typename T, size_t N>
struct Compound<T[N]>
{
    enum
    {
        IsPointer = 0,
        IsReference = 0,
        IsArray = 1,
        IsPtrMember = 0
    };

    typedef typename Compound<T>::BaseType BaseType;
};

// Specialization for pointer-to members
template<typename T, typename C>
struct Compound<T C::*>
{
    enum
    {
        IsPointer = 0,
        IsReference = 0,
        IsArray = 0,
        IsPtrMember = 1
    };

    typedef typename Compound<T>::BaseType BaseType;
};

template<typename W>
struct StrictlyEqual<W, W>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<typename T>
struct IsConst<const T>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct DefaultOrNull<None>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct DefaultOrNull<Default>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct DefaultOrNull<NullPtr>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<typename T,
         template<class>
         class OwspP,
         template<class>
         class ChckP,
         class ConvP,
         template<class>
         class StorP,
         template<class>
         class ConsP>
struct IsSmartPtr<SmartPtr<T, OwspP, ChckP, ConvP, StorP, ConsP>>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsDecimal<float>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};

template<>
struct IsDecimal<double>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};
#ifdef YUNI_HAS_LONG_DOUBLE
template<>
struct IsDecimal<long double>
{
    enum
    {
        Yes = 1,
        No = 0
    };
};
#endif

//@} // Overloads

} // namespace Yuni::Static::Type
