
#include "antares/mersenne-twister/mersenne-twister.h"
#include <cassert>

#define MATRIX_A 0x9908b0dfUL   // constant vector a
#define UPPER_MASK 0x80000000UL // most significant w-r bits
#define LOWER_MASK 0x7fffffffUL // least significant r bits

using namespace Yuni;

namespace Antares
{
MersenneTwister::MersenneTwister()
{
    reset();
}

MersenneTwister::~MersenneTwister()
{
}

void MersenneTwister::reset(uint seed)
{
    assert(sizeof(uint) == sizeof(uint32_t)
           && "This version of mersenne twister is dedicated to 32bits words");
    mt[0] = seed & 0xffffffffUL;
    for (mti = 1; mti < periodN; ++mti)
    {
        mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
        // See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier.
        // In the previous versions, MSBs of the seed affect
        // only MSBs of the array mt[]
        // 2002/01/09 modified by Makoto Matsumoto
        mt[mti] &= 0xffffffffUL;
        // for >32 bit machines
    }
}

MersenneTwister::Value MersenneTwister::next() const
{
    uint32_t y;
    static const uint32_t mag01[2] = {0x0UL, MATRIX_A};

    // mag01[x] = x * MATRIX_A  for x=0,1
    if (mti >= periodN) // generate N words at one time
    {
        if (mti == periodN + 1)
        {
            // if init_genrand() has not been called,
            // a default initial seed is used
            /// reset(5489UL);
            assert("Mersenne Twister should already been initialized !");
        }

        int kk;
        for (kk = 0; kk < periodN - periodM; ++kk)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + periodM] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        for (; kk < periodN - 1; ++kk)
        {
            y = (mt[kk] & UPPER_MASK) | (mt[kk + 1] & LOWER_MASK);
            mt[kk] = mt[kk + (periodM - periodN)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }

        y = (mt[periodN - 1] & UPPER_MASK) | (mt[0] & LOWER_MASK);
        mt[periodN - 1] = mt[periodM - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];
        mti = 0;
    }

    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y * (1.0 / 4294967295.0);
}

} // namespace Antares
