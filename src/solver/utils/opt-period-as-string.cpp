#include "opt-period-as-string.h"


// ------------------------------------
// Daily optimization
// ------------------------------------
optDailyAsString::optDailyAsString(unsigned int day, unsigned int week, unsigned int year) :
 day_(day), week_(week), year_(year)
{
}

std::string optDailyAsString::to_string() const
{
    std::string to_return = std::to_string(year_ + 1) + "-";
    to_return += std::to_string(week_ + 1) + "-";
    to_return += std::to_string(day_ + 1);
    return to_return;
}

// ------------------------------------
// Weekly optimization
// ------------------------------------
optWeeklyAsString::optWeeklyAsString(unsigned int week, unsigned int year) :
 week_(week), year_(year)
{
}

std::string optWeeklyAsString::to_string() const
{
    std::string to_return = std::to_string(year_ + 1) + "-";
    to_return += std::to_string(week_ + 1);
    return to_return;
}