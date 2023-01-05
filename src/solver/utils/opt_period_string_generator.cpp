#include "opt_period_string_generator.h"


// ------------------------------------
// Daily optimization
// ------------------------------------
optDailyStringGenerator::optDailyStringGenerator(unsigned int day,
                                                 unsigned int week,
                                                 unsigned int year) :
 day_(day), week_(week), year_(year)
{
}

std::string optDailyStringGenerator::to_string() const
{
    std::string to_return = std::to_string(year_ + 1) + "-";
    to_return += std::to_string(week_ + 1) + "-";
    to_return += std::to_string(day_ + 1);
    return to_return;
}

// ------------------------------------
// Weekly optimization
// ------------------------------------
optWeeklyStringGenerator::optWeeklyStringGenerator(unsigned int week, unsigned int year) :
 week_(week), year_(year)
{
}

std::string optWeeklyStringGenerator::to_string() const
{
    std::string to_return = std::to_string(year_ + 1) + "-";
    to_return += std::to_string(week_ + 1);
    return to_return;
}