#pragma once

#include <string>
#include <memory>
#include "../optimisation/opt-period-as-string-base.h"


std::shared_ptr<optPeriodAsString> createOptPeriodAsString(bool isOptimizationWeekly,
                                                    unsigned int day,
                                                    unsigned int week,
                                                    unsigned int year);

std::string createCriterionFilename(std::shared_ptr<optPeriodAsString> opt_period_as_string, const unsigned int optNumber);
std::string createMPSfilename(std::shared_ptr<optPeriodAsString> opt_period_as_string, const unsigned int optNumber);
