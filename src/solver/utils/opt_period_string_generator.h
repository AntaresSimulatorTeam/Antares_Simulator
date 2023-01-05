#pragma once

#include "../optimisation/opt_period_string_generator_base.h"

/*
    MPS and criterion generated files wear the same time interval sequence in their names.
    This time interval sequence is either <year>-<week> (for a weekly optimization),
    or <year>-<week>-<day> (for a daily optimization).
    So, depending on whether the optimization is weekly or daily, the files are named either :
    - problem-<year>-<week>--optim-nb-<n>.mps
    - problem-<year>-<week>-<day>--optim-nb-<n>.mps

    The following class hierarchy intend to represent these time interval sequences (weekly or
   daily), and it turns them into a string when building the names of the previous files. This
   hierarchy is designed for polymorphism : whetever the time interval an object of that hierarchy
   represents, it is passed as a base class argument, and the right 'to_string()' function is
   called.
*/

// ------------------------------------
// Daily optimization
// ------------------------------------
class OptDailyStringGenerator final : public OptPeriodStringGenerator
{
public:
    OptDailyStringGenerator(unsigned int day, unsigned int week, unsigned int year);
    std::string to_string() const override;

private:
    unsigned int day_ = 0;
    unsigned int week_ = 0;
    unsigned int year_ = 0;
};

// ------------------------------------
// Weekly optimization
// ------------------------------------
class OptWeeklyStringGenerator final : public OptPeriodStringGenerator
{
public:
    OptWeeklyStringGenerator(unsigned int week, unsigned int year);
    std::string to_string() const override;

private:
    unsigned int week_ = 0;
    unsigned int year_ = 0;
};