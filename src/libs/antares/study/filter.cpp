// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/filter.h"

#include <string>

#include <antares/utils/utils.h>

namespace Antares::Data
{
std::string datePrecisionIntoString(unsigned int datePrecisionFilter)
{
    std::string to_return;
    if (datePrecisionFilter & filterHourly)
    {
        to_return += "hourly";
    }

    if (datePrecisionFilter & filterDaily)
    {
        if (!to_return.empty())
        {
            to_return += ", ";
        }
        to_return += "daily";
    }

    if (datePrecisionFilter & filterWeekly)
    {
        if (!to_return.empty())
        {
            to_return += ", ";
        }
        to_return += "weekly";
    }

    if (datePrecisionFilter & filterMonthly)
    {
        if (!to_return.empty())
        {
            to_return += ", ";
        }
        to_return += "monthly";
    }

    if (datePrecisionFilter & filterAnnual)
    {
        if (!to_return.empty())
        {
            to_return += ", ";
        }
        to_return += "annual";
    }

    return to_return;
}

unsigned int stringIntoDatePrecision(const std::string& string)
{
    if (string.empty())
    {
        return filterNone;
    }

    unsigned int flag = 0;

    const std::string separators = ",; \r\n\t";
    std::size_t begin = 0;
    while (begin <= string.size())
    {
        const std::size_t end = string.find_first_of(separators, begin);
        const std::string word = string.substr(begin, end == std::string::npos ? std::string::npos
                                                                               : end - begin);
        const std::string lower = Antares::stringToLower(word);
        if (lower == "hourly")
        {
            flag |= filterHourly;
        }
        else if (lower == "daily")
        {
            flag |= filterDaily;
        }
        else if (lower == "weekly")
        {
            flag |= filterWeekly;
        }
        else if (lower == "monthly")
        {
            flag |= filterMonthly;
        }
        else if (lower == "annual")
        {
            flag |= filterAnnual;
        }

        if (end == std::string::npos)
        {
            break;
        }
        begin = end + 1;
    }
    return flag;
}

unsigned int addTimeIntervallToDatePrecisionFilter(const unsigned int index)
{
    unsigned int flag = 0;
    switch (index)
    {
    case 0:
        return flag |= filterHourly;
    case 1:
        return flag |= filterDaily;
    case 2:
        return flag |= filterWeekly;
    case 3:
        return flag |= filterMonthly;
    case 4:
        return flag |= filterAnnual;
    default:
        return filterNone;
    }

    return flag;
}

} // namespace Antares::Data
