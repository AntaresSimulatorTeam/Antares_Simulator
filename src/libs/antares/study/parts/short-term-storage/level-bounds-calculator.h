#pragma once

#include <vector>
#include <map>
#include "level-bounds.h"

namespace Antares::Data::ShortTermStorage
{

class LevelBoundsCalculator
{
public:
    LevelBoundsCalculator(unsigned int cycleSize,
                          std::vector<double>& lowerRuleCurve,
                          std::vector<double>& upperRuleCurve) :
        cycleSize_(cycleSize),
        lowerRuleCurve_(lowerRuleCurve),
        upperRuleCurve_(upperRuleCurve)
    {}

    virtual std::vector<Bounds> getBoundsOverTheWeekStartingAtHour(unsigned int firstHourOfWeek) = 0;

protected:
    unsigned int cycleSize_ = 0;
    std::vector<double>& lowerRuleCurve_;
    std::vector<double>& upperRuleCurve_;
};

class LevelBoundsForWeeks : public LevelBoundsCalculator
{
public:
    LevelBoundsForWeeks(unsigned int cycleSize,
                        std::vector<double>& lowerRuleCurve,
                        std::vector<double>& upperRuleCurve) :
        LevelBoundsCalculator(cycleSize, lowerRuleCurve, upperRuleCurve)
    {}

    std::vector<Bounds> getBoundsOverTheWeekStartingAtHour(unsigned int firstHourOfWeek) override;
};


class LevelBoundsForDays : public LevelBoundsCalculator
{
public:
    LevelBoundsForDays(unsigned int cycleSize,
                       std::vector<double>& lowerRuleCurve,
                       std::vector<double>& upperRuleCurve) :
        LevelBoundsCalculator(cycleSize, lowerRuleCurve, upperRuleCurve)
    {}

    std::vector<Bounds> getBoundsOverTheWeekStartingAtHour(unsigned int firstHourOfWeek) override;
};
} // namespace Antares::Data::ShortTermStorage
