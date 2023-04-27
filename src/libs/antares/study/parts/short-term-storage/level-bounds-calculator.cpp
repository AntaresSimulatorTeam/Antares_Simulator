
#include <algorithm>
#include <limits>
#include "level-bounds-calculator.h"
#include "antares/constants.h"


namespace Antares::Data::ShortTermStorage
{
// =============================
// class LevelBoundsForWeeks
// =============================

std::vector<Bounds>
LevelBoundsForWeeks::getBoundsOverTheWeekStartingAtHour(unsigned int firstHourOfWeek)
{
    std::vector<Bounds> to_return;
    double maxLowerBound = 0;
    double minUpperBound = std::numeric_limits<double>::max();

    for (unsigned int hour = firstHourOfWeek;
        hour < firstHourOfWeek + Constants::nbHoursInAWeek;
        hour += cycleSize_)
    {
        // Reduce the interval if necessary
        maxLowerBound = std::max(maxLowerBound, lowerRuleCurve_[hour]);
        minUpperBound = std::min(minUpperBound, upperRuleCurve_[hour]);
    }
    to_return.push_back(Bounds(maxLowerBound, minUpperBound));
    return to_return;
}

// =============================
// class LevelBoundsForDays
// =============================

std::vector<Bounds>
LevelBoundsForDays::getBoundsOverTheWeekStartingAtHour(unsigned int firstHourOfWeek)
{
    std::vector<Bounds> to_return;

    for (unsigned int firstHourOfDay = firstHourOfWeek;
         firstHourOfDay < firstHourOfWeek + Constants::nbHoursInAWeek;
         firstHourOfDay += Constants::nbHoursInDay)
    { 
        double maxLowerBound = 0;
        double minUpperBound = std::numeric_limits<double>::max();

        for (unsigned int hour = firstHourOfDay;
            hour < firstHourOfDay + Constants::nbHoursInDay;
            hour += cycleSize_)
        {
            maxLowerBound = std::max(maxLowerBound, lowerRuleCurve_[hour]);
            minUpperBound = std::min(minUpperBound, upperRuleCurve_[hour]);
        }

        to_return.push_back(Bounds(maxLowerBound, minUpperBound));
    }

    return to_return;
}


} // namespace Antares::Data::ShortTermStorage