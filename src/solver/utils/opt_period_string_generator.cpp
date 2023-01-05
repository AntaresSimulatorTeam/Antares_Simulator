#include "opt_period_string_generator.h"


// ------------------------------------
// Daily optimization
// ------------------------------------
OptDailyStringGenerator::OptDailyStringGenerator(unsigned int day,
                                                 unsigned int week,
                                                 unsigned int year) :
 day_(day), week_(week), year_(year)
{
}

std::string OptDailyStringGenerator::to_string() const
{
    std::string to_return = std::to_string(year_ + 1) + "-";
    to_return += std::to_string(week_ + 1) + "-";
    to_return += std::to_string(day_ + 1);
    return to_return;
}

// ------------------------------------
// Weekly optimization
// ------------------------------------
OptWeeklyStringGenerator::OptWeeklyStringGenerator(unsigned int week, unsigned int year) :
 week_(week), year_(year)
{
}

std::string OptWeeklyStringGenerator::to_string() const
{
    std::string to_return = std::to_string(year_ + 1) + "-";
    to_return += std::to_string(week_ + 1);
    return to_return;
}