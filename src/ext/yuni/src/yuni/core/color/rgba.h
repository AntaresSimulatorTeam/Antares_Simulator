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
#include "fwd.h"

#ifdef RGB
#undef RGB
#endif // RGB

namespace Yuni::Color
{
/*!
** \brief 32Bits RGB Color Model (additive color model + transparency)
** \ingroup Color
*/
template<class T = uint8>
class YUNI_DECL RGBA final
{
public:
    //! Type used for channels
    typedef T Type;
    //! The most appropriate type for printing
    typedef typename Yuni::Private::Color::Print<T>::Type PrintType;

    //! The most appropriate type for calculations
    template<class V>
    struct Calculation
    {
        typedef typename Yuni::Private::Color::Calculation<T, V>::Type Type;
    };

    enum
    {
        //! A non-zero value if the class has a transparency channel
        hasTransparency = 1,
        //! The upper bound for this color model
        upperBound = Yuni::Private::Color::DefaultValues<T>::upperBound,
        //! The lower bound for this color model
        lowerBound = Yuni::Private::Color::DefaultValues<T>::lowerBound,
    };

public:
    //! \name Constructors
    //@{
    /*!
    ** \brief Default constructor
    */
    RGBA();
    /*!
    ** \brief Copy constructor
    */
    RGBA(const RGBA& rhs);
    /*!
    ** \brief Constructor with default values
    */
    template<class R, class G, class B, class A>
    RGBA(R r, G g, B b, A a);
    /*!
    ** \brief Constructor with default values
    */
    template<class R, class G, class B>
    RGBA(R r, G g, B b);
    /*!
    ** \brief Constructor with default values
    */
    template<class U>
    RGBA(const U& value);
    //@}

    //! \name Assign
    //@{
    /*!
    ** \brief Assign all channels at once
    */
    template<class R, class G, class B, class A>
    void assign(R r, G g, B b, A a);
    /*!
    ** \brief Reset all channels at once
    **
    ** The alpha channel will have a default value (opaque)
    */
    template<class R, class G, class B>
    void assign(R r, G g, B b);
    /*!
    ** \brief Reset all channels at once from a single value
    */
    template<class U>
    void assign(const U& value);
    //@}

    //! \name Misc
    //@{
    /*!
    ** \brief Get if the color will be visible
    **
    ** This method is equivalent to manually check the alpha channel
    ** with the lower bound
    */
    bool visible() const;

    /*!
    ** \brief Check if all channels are valid
    **
    ** This method is only useful when the channels are modified manually.
    */
    bool valid() const;
    //@}

    //! \name Stream printing
    //@{
    //! Print the color value
    template<class StreamT>
    void print(StreamT& out) const;
    //@}

    //! \name Operators
    //@{
    //! Increase or decrease all channels
    template<class U>
    RGBA& operator+=(const U& value);
    //! Increase or decrease all channels
    template<class U>
    RGBA& operator-=(const U& value);
    //! Assignment
    RGBA& operator=(const RGBA& rhs);
    //! Assignment
    RGBA& operator=(const Yuni::NullPtr&);
    //! Assignment
    template<class U>
    RGBA& operator=(const U& rhs);
    //! Comparison
    template<class U>
    bool operator==(const U& rhs) const;
    //! Comparison
    template<class U>
    bool operator!=(const U& rhs) const;
    //! Index access
    T& operator[](const uint i);
    //@}

public:
    //! The red channel
    Type red;
    //! The green channel
    Type green;
    //! The blue channel
    Type blue;
    //! The alpha channel (transparency)
    Type alpha;

}; // class RGBA

} // namespace Yuni::Color

#include "rgba.hxx"
