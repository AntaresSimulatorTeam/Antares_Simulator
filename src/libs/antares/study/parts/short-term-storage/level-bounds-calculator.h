#pragma once

#include <vector>
#include <map>
#include "level-bounds.h"

namespace Antares::Data::ShortTermStorage
{

class LevelBoundsCalculator
{
public:
    LevelBoundsCalculator(unsigned int firstHourOfWeek,
                          unsigned int lastHourOfWeek,
                          unsigned int cycleSize,
                          std::vector<double>& lowerRuleCurve,
                          std::vector<double>& upperRuleCurve) :
        firstHourOfWeek_(firstHourOfWeek),
        lastHourOfWeek_(lastHourOfWeek),
        cycleSize_(cycleSize),
        lowerRuleCurve_(lowerRuleCurve),
        upperRuleCurve_(upperRuleCurve)
    {}

    virtual void addBounds(std::map<unsigned int, Bounds>&) = 0;

protected:
    unsigned int firstHourOfWeek_ = 0;
    unsigned int lastHourOfWeek_ = 0;
    unsigned int cycleSize_ = 0;
    std::vector<double>& lowerRuleCurve_;
    std::vector<double>& upperRuleCurve_;
};

class LevelBoundsForWeeks : public LevelBoundsCalculator
{
public:
    LevelBoundsForWeeks(unsigned int firstHourOfWeek,
                        unsigned int lastHourOfWeek,
                        unsigned int cycleSize,
                        std::vector<double>& lowerRuleCurve,
                        std::vector<double>& upperRuleCurve) :
        LevelBoundsCalculator(firstHourOfWeek, lastHourOfWeek, cycleSize, lowerRuleCurve, upperRuleCurve)
    {}

    void addBounds(std::map<unsigned int, Bounds>& initLevelBounds) override;
};


class LevelBoundsForDays : public LevelBoundsCalculator
{
public:
    LevelBoundsForDays(unsigned int firstHourOfWeek,
                       unsigned int lastHourOfWeek,
                       unsigned int cycleSize,
                       std::vector<double>& lowerRuleCurve,
                       std::vector<double>& upperRuleCurve) :
        LevelBoundsCalculator(firstHourOfWeek, lastHourOfWeek, cycleSize, lowerRuleCurve, upperRuleCurve)
    {}

    void addBounds(std::map<unsigned int, Bounds>& initLevelBounds) override;
};
} // namespace Antares::Data::ShortTermStorage
