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

#include "antares/study/scenario-builder/scBuilderUtils.h"

#include <algorithm>
#include <iomanip>
#include <sstream>
#include <string>

#include <yuni/core/string/string.h>

namespace Antares::Data::ScenarioBuilder
{
std::string fromHydroLevelToString(double d)
{
    std::ostringstream stream;
    stream << std::setprecision(4);
    stream << d;
    return stream.str();
}

double fromStringToHydroLevel(const Yuni::String& value, const double maxLevel)
{
    double result;
    std::string val = value.to<std::string>();
    try
    {
        result = stod(val);
    }
    catch (std::invalid_argument&)
    {
        return std::nan("");
    }

    return std::clamp(result, 0., maxLevel);
}

uint fromStringToTSnumber(const Yuni::String& value)
{
    uint result;
    const uint maxTSnumber = 10000;
    double result_dbl;

    std::string val = value.to<std::string>();
    try
    {
        result_dbl = stod(val);
    }
    catch (std::invalid_argument&)
    {
        return 0;
    }

    if (result_dbl < 0.)
    {
        result = 0;
    }
    else
    {
        if (result_dbl > maxTSnumber)
        {
            result = maxTSnumber;
        }
        else
        {
            result = (uint)result_dbl;
        }
    }
    return result;
}

} // namespace Antares::Data::ScenarioBuilder
