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

#include "mersenne-twister.hxx"

#endif // __LIB_ANTARES_RANDOM_MERSENNE_H__
