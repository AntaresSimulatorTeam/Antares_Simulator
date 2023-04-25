#include "level-bounds-calculator.h"
#include "antares/constants.h"


namespace Antares::Data::ShortTermStorage
{
// =============================
// class LevelBoundsForWeeks
// =============================
void LevelBoundsForWeeks::addBounds(std::map<unsigned int, Bounds>& initLevelBounds)
{
    for (unsigned int hour = firstHourOfWeek_;
         hour < firstHourOfWeek_ + Constants::nbHoursInAWeek;
         hour += cycleSize_)
    {        
        auto bounds = Bounds(lowerRuleCurve_[hour], upperRuleCurve_[hour]);
        initLevelBounds.insert(std::pair<unsigned int, Bounds>(hour, bounds));
    }
}


// =============================
// class LevelBoundsForDays
// =============================
void LevelBoundsForDays::addBounds(std::map<unsigned int, Bounds>& initLevelBounds)
{
    for (unsigned int firstHourOfDay = firstHourOfWeek_;
         firstHourOfDay < firstHourOfWeek_ + Constants::nbHoursInAWeek;
         firstHourOfDay += Constants::nbHoursInDay)
    { 
        for (unsigned int hour = firstHourOfDay;
            hour < firstHourOfDay + Constants::nbHoursInDay;
            hour += cycleSize_)
        {
            auto bounds = Bounds(lowerRuleCurve_[hour], upperRuleCurve_[hour]);
            initLevelBounds.insert(std::pair<unsigned int, Bounds>(hour, bounds));
        }
    }
}


} // namespace Antares::Data::ShortTermStorage