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
#include "../string.h"
#include "bit.h"
#include <iostream>

namespace Yuni::Bit
{
/*!
** \brief Array of bits
** \ingroup Bit
**
** This class provides routines for handling an array of bits and is especially
** efficient for a medium or large amount of bits.
**
** Performance Tip: For small arrays of bits (for example < 50), a typical
**   char[] may be faster, but it will use more memory.
**
**
** \code
** Bit::Array bits(50);
** std::cout << bits << "\n";
**
** bits.set(12);
** bits.set(27);
** bits.set(48);
** std::cout << bits << "\n";
**
** bits.unset(27);
** std::cout << bits << "\n";
**
** bits.unset();
** std::cout << bits << "\n";
** \endcode
*/
class YUNI_DECL Array final
{
public:
    //! Size
    typedef uint Size;
    //! Buffer Type
    typedef CString<20, true> BufferType;

    enum
    {
        //! Invalide offset
        npos = (uint)-1,
    };

public:
    //! \name Constructors & Destructor
    //@{
    //! Default constructor
    Array();

    //! Constructor, with an initial number of bits
    explicit Array(uint n);

    //! Constructor, with an initial number of bits and a value for each of them
    Array(uint n, bool value);

    //! Copy constructor
    Array(const Array& copy);
    //@}

    //! \name Bit manipulation
    //@{
    /*!
    ** \brief Unset all bits at once
    */
    void reset();

    /*!
    ** \brief Set/Unset all bits at once
    */
    void reset(bool value);

    /*!
    ** \brief Set the Ith bit
    */
    void set(uint i);

    /*!
    ** \brief Set/Unset the Ith bit
    */
    void set(uint i, bool value);

    /*!
    ** \brief Unset the Ith bit
    */
    void unset(uint i);

    /*!
    ** \brief Unset all bits at once
    **
    ** Equivalent to reset().
    */
    void unset();

    /*!
    ** \brief Get if the Ith bit is set
    */
    bool get(uint i) const;

    /*!
    ** \brief Get if the Ith bit is set
    **
    ** This routine is provided for STL compatibility.
    */
    bool test(uint i) const;

    /*!
    ** \brief Test if any bit is set
    */
    bool any() const;

    /*!
    ** \brief Test if no bit is set
    */
    bool none() const;

    /*!
    ** \brief Test if all bit are set
    */
    bool all() const;
    //@}

    //! \name Import/Export
    //@{
    /*!
    ** \brief Load an array of bit from a buffer
    **
    ** \param buffer Any buffer
    */
    void loadFromBuffer(const AnyString& buffer);

    /*!
    ** \brief Save the array of bits into another buffer (which may use any type)
    **
    ** \param u A buffer
    */
    template<class AnyBufferT>
    void saveToBuffer(AnyBufferT& u);
    //@}

    //! \name Lookup
    //@{
    /*!
    ** \brief Find the first bit set or unset from a given offset
    **
    ** Find the first item which is set :
    ** \code
    ** uint firstItem = mybitArray.find<true>();
    ** \endcode
    **
    ** \param offset The offset where to start from
    ** \return The bit index. npos if not found
    ** \tparam ValueT True to find the first bit set, false for the first unset
    */
    template<bool ValueT>
    uint find(uint offset = 0) const;

    /*!
    ** \brief Find the first N bit set or unset from a given offset
    **
    ** Find the 3th item which is set :
    ** \code
    ** uint firstItem = mybitArray.findN<true>(3);
    ** \endcode
    **
    ** \param count  The number of consecutive bits
    ** \param offset The offset where to start from
    ** \return The bit index. npos if not found
    ** \tparam ValueT True to find the first bit set, false for the first unset
    */
    template<bool ValueT>
    uint findN(uint count, uint offset = 0) const;
    //@}

    //! \name Memory management
    //@{
    //! The number of bits within the buffer
    uint size() const;

    //! The number of bits within the buffer
    uint count() const;

    /*!
    ** \brief The size in bytes needed to store all bits within the buffer
    **
    ** This value greater of equal to the value returned by size().
    */
    uint sizeInBytes() const;

    //! Reserve an amount of bits
    void reserve(uint n);

    //! Truncate to an amount of bits
    void truncate(uint n);

    //! Resize the internal buffer to a given amount of bit
    void resize(uint n);
    //@}

    const char* c_str() const;
    const char* data() const;
    char* data();

    //! \name Stream
    //@{
    template<class U>
    void print(U& out) const;
    //@}

    //! \name Operators
    //@{
    //! The operator `=`
    Array& operator=(const Array& rhs);
    //! The operator `=`
    Array& operator=(const AnyString& rhs);
    //@}

private:
    //! Number of bits into the buffer, requested by the caller
    // This value is lower or equal to the buffer size
    uint pCount;
    //! Internal buffer
    BufferType pBuffer;

}; // class Array

} // namespace Yuni::Bit

#include "array.hxx"
