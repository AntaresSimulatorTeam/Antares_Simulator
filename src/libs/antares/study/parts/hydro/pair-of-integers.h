#pragma once

class PairOfIntegers
{
public:
    PairOfIntegers(unsigned int numberOfTS_1, unsigned int numberOfTS_2);

    bool bothZero() const;
    bool same() const;
    bool bothGreaterThanOne() const;
    unsigned int sup() const { return numberOfTSsup_; }

private:
    unsigned int numberOfTS_1_;
    unsigned int numberOfTS_2_;
    unsigned int numberOfTSsup_{ 0 };
    unsigned int numberOfTSinf_{ 0 };
};
