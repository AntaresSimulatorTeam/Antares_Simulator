/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#ifndef __ANTARES_LIBS_STUDY_PARTS_THERMAL_POLLUTANT_H__
#define __ANTARES_LIBS_STUDY_PARTS_THERMAL_POLLUTANT_H__

#include <string>
#include <array>
#include <map>

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
