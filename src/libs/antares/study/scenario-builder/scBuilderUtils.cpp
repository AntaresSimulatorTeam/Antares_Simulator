// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/scenario-builder/scBuilderUtils.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>



namespace Antares::Data::ScenarioBuilder
{
std::string fromHydroLevelToString(double d)
{
    std::ostringstream stream;
    stream << std::setprecision(4);
    stream << d;
    return stream.str();
}

double fromStringToHydroLevel(const std::string& value, const double maxLevel)
{
    double result;
    try
    {
        result = stod(value);
    }
    catch (std::invalid_argument&)
    {
        return std::nan("");
    }

    return std::clamp(result, 0., maxLevel);
}

unsigned int fromStringToTSnumber(const std::string& value)
{
    unsigned int result;
    const unsigned int maxTSnumber = 10000;
    double result_dbl;

    try
    {
        result_dbl = stod(value);
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
            result = (unsigned int)result_dbl;
        }
    }
    return result;
}

} // namespace Antares::Data::ScenarioBuilder
