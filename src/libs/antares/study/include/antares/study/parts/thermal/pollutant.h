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
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_POLLUTANT_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_POLLUTANT_H__

#include <array>
#include <map>
#include <string>

namespace Antares::Data
{

class Pollutant
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
