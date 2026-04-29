// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_POLLUTANT_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_POLLUTANT_H__

#include <array>
#include <map>
#include <string>

namespace Antares::Data
{

class Pollutant final
{
public:
    enum PollutantEnum
    {
        CO2 = 0,
        NH3,
        SO2,
        NOX,
        PM2_5,
        PM5,
        PM10,
        NMVOC,
        OP1,
        OP2,
        OP3,
        OP4,
        OP5,
        POLLUTANT_MAX
    };

    static const std::map<enum PollutantEnum, const std::string> pollutantNamesOutputVariables;
    static const std::string& getPollutantName(int index);

    static const std::map<std::string, enum PollutantEnum> namesToEnum;

    std::array<double, POLLUTANT_MAX> factors{0};

}; // class Pollutant

} // namespace Antares::Data

#endif /* __ANTARES_LIBS_STUDY_PARTS_THERMAL_POLLUTANT_H__ */
