/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __LIB_ANTARES_RANDOM_MERSENNE_H__
#define __LIB_ANTARES_RANDOM_MERSENNE_H__

#include <yuni/yuni.h>
#include <yuni/core/math/random/distribution.h>

namespace Antares
{
/*!
** \brief MersenneTwister Pseudo random number generator
**
** The Mersenne twister is a pseudorandom number generator developed in 1997
** by Makoto Matsumoto (松本 眞) and Takuji Nishimura (西村 拓士).
** It provides for fast generation of very high-quality pseudorandom numbers,
** having been designed specifically to rectify many of the flaws found in
** older algorithms.
**
** \see http://en.wikipedia.org/wiki/Mersenne_twister
*/
class MersenneTwister final
{
public:
    // Name of the distribution
    static const char* Name()
    {
        return "Mersenne Twister random numbers";
    }

    //! Type of a single random
    using Value = double;

    enum
    {
        //! A default seed
        defaultSeed = 5489,
    };

public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default constructor
    */
    MersenneTwister();
    //! Destructor
    ~MersenneTwister();
    //@}

    //! \name Reset
    //@{
    //! Reset the generator
    void reset();
    //! Reset the generator with a custom seed
    void reset(uint seed);
    //@}

    //! \name Generator
    //@{
    /*!
    ** \brief Generate a new random number
    **
    ** \internal In an ideal world this method should be const to not make
    **   break the global design
    */
    Value next() const;
    //@}

    //! \name Bounds
    //@{
    //! Lower bound
    static Value min();
    //! Upper bound
    static Value max();
    //@}

    Value operator()();

private:
    enum
    {
        periodN = 624,
        periodM = 397,
    };

    //! State vector
    mutable uint32_t mt[periodN];
    //
    mutable int32_t mti;

}; // class MersenneTwister

} // namespace Antares

#endif // __LIB_ANTARES_RANDOM_MERSENNE_H__
