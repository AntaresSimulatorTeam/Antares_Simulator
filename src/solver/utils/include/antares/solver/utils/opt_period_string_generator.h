/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once
#include <string>

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

class OptPeriodStringGenerator
{
public:
    virtual std::string to_string() const = 0;
    virtual ~OptPeriodStringGenerator() = default;
};

// ------------------------------------
// Daily optimization
// ------------------------------------
class OptDailyStringGenerator final: public OptPeriodStringGenerator
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
class OptWeeklyStringGenerator final: public OptPeriodStringGenerator
{
public:
    OptWeeklyStringGenerator(unsigned int week, unsigned int year);
    std::string to_string() const override;

private:
    unsigned int week_ = 0;
    unsigned int year_ = 0;
};
