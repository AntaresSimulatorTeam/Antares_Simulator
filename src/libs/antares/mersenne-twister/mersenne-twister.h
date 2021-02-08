/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
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
class MersenneTwister final : public Yuni::Math::Random::ADistribution<double, MersenneTwister>
{
public:
    // Name of the distribution
    static const char* Name()
    {
        return "Mersenne Twister random numbers";
    }
    //! Type of a single random
    typedef double Value;
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

private:
    enum
    {
        periodN = 624,
        periodM = 397,
    };
    //! State vector
    mutable Yuni::uint32 mt[periodN];
    //
    mutable Yuni::sint32 mti;

}; // class MersenneTwister

} // namespace Antares

#include "mersenne-twister.hxx"

#endif // __LIB_ANTARES_RANDOM_MERSENNE_H__
