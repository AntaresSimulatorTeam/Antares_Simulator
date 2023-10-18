#include "pair-of-integers.h"
#include <algorithm>


PairOfIntegers::PairOfIntegers(unsigned int numberOfTS_1, unsigned int numberOfTS_2)
    : numberOfTS_1_(numberOfTS_1), numberOfTS_2_(numberOfTS_2)
{
    numberOfTSsup_ = std::max(numberOfTS_1_, numberOfTS_2_);
    numberOfTSinf_ = std::min(numberOfTS_1_, numberOfTS_2_);
}

bool PairOfIntegers::same() const
{
    return (numberOfTS_1_ == numberOfTS_2_) ? true : false;
}

bool PairOfIntegers::bothGreaterThanOne() const
{
    return (numberOfTSinf_ > 1);
}