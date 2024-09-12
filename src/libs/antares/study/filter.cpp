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

#include "antares/study/filter.h"

using namespace Yuni;

namespace Antares::Data
{
std::string datePrecisionIntoString(uint datePrecisionFilter)
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

uint stringIntoDatePrecision(const AnyString& string)
{
    if (string.empty())
    {
        return filterNone;
    }

    uint flag = 0;

    string.words(",; \r\n\t",
                 [&flag](const AnyString& word) -> bool
                 {
                     ShortString16 s = word;
                     s.toLower();
                     if (s == "hourly")
                     {
                         flag |= filterHourly;
                         return true;
                     }
                     if (s == "daily")
                     {
                         flag |= filterDaily;
                         return true;
                     }
                     if (s == "weekly")
                     {
                         flag |= filterWeekly;
                         return true;
                     }
                     if (s == "monthly")
                     {
                         flag |= filterMonthly;
                         return true;
                     }
                     if (s == "annual")
                     {
                         flag |= filterAnnual;
                         return true;
                     }
                     return true;
                 });
    return flag;
}

uint addTimeIntervallToDatePrecisionFilter(const uint index)
{
    uint flag = 0;
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
