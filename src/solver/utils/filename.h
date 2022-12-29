#pragma once

#include <string>
#include <memory>


/*
    MPS and criterion generated files wear the same time interval sequences in their names.
    These time interval sequence are associated to an optimization period, which can be daily or weekly. 
    Depending on whether the optimization is weekly or daily, the files are named either :
    - problem-<year>-<week>--optim-nb-<n>.mps
    - problem-<year>-<week>-<day>--optim-nb-<n>.mps
    So the time interval sequence is either <year>-<week> (for a weekly optimization), 
    or <year>-<week>-<day> (for a daily optimization).

    The following class hierarchy intend to reprensent these time interval sequences (weekly or daily),
    and it turns them into a string when building the names of the previous files.
    This hierarchy is designed for polymorphism : whetever the time interval an object of
    that hierarchy represents, it is passed as a base class argument, and the right 'to_string()'
    function is called.
*/
// ------------------------------------
// Optimization period : base class
// ------------------------------------
class optPeriodAsString
{
public:
    virtual std::string to_string() const = 0;
    virtual ~optPeriodAsString() = default;
};


// ------------------------------------
// Daily optimization
// ------------------------------------
class optDailyAsString final : public optPeriodAsString
{
public:
    optDailyAsString(unsigned int day, unsigned int week, unsigned int year);
    std::string to_string() const override;

private:
    unsigned int day_ = 0;
    unsigned int week_ = 0;
    unsigned int year_ = 0;
};


// ------------------------------------
// Weekly optimization
// ------------------------------------
class optWeeklyAsString final : public optPeriodAsString
{
public:
    optWeeklyAsString(unsigned int week, unsigned int year);
    std::string to_string() const override;

private:
    unsigned int week_ = 0;
    unsigned int year_ = 0;
};

std::shared_ptr<optPeriodAsString> createOptimizationPeriod(bool isOptimizationWeekly,
                                                    unsigned int day,
                                                    unsigned int week,
                                                    unsigned int year); 

std::string createOptimizationFilename(const std::string& title,
                                       std::shared_ptr<optPeriodAsString> opt_period_as_string,
                                       const unsigned int optNumber,
                                       const std::string& extension);

std::string createCriterionFilename(std::shared_ptr<optPeriodAsString> opt_period_as_string, const unsigned int optNumber);
std::string createMPSfilename(std::shared_ptr<optPeriodAsString> opt_period_as_string, const unsigned int optNumber);