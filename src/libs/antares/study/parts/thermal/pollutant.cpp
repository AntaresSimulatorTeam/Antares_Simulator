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

#include "antares/study/parts/thermal/pollutant.h"

namespace Antares::Data
{

const std::map<enum Pollutant::PollutantEnum, const std::string>
  Pollutant::pollutantNamesOutputVariables = {{CO2, "CO2 EMIS."},
                                              {NH3, "NH3 EMIS."},
                                              {SO2, "SO2 EMIS."},
                                              {NOX, "NOX EMIS."},
                                              {PM2_5, "PM2_5 EMIS."},
                                              {PM5, "PM5 EMIS."},
                                              {PM10, "PM10 EMIS."},
                                              {NMVOC, "NMVOC EMIS."},
                                              {OP1, "OP1 EMIS."},
                                              {OP2, "OP2 EMIS."},
                                              {OP3, "OP3 EMIS."},
                                              {OP4, "OP4 EMIS."},
                                              {OP5, "OP5 EMIS."}};

const std::string& Pollutant::getPollutantName(int index)
{
    return pollutantNamesOutputVariables.at(PollutantEnum(index));
}

const std::map<std::string, enum Pollutant::PollutantEnum> Pollutant::namesToEnum = {{"co2", CO2},
                                                                                     {"nh3", NH3},
                                                                                     {"so2", SO2},
                                                                                     {"nox", NOX},
                                                                                     {"pm2_5",
                                                                                      PM2_5},
                                                                                     {"pm5", PM5},
                                                                                     {"pm10", PM10},
                                                                                     {"nmvoc",
                                                                                      NMVOC},
                                                                                     {"op1", OP1},
                                                                                     {"op2", OP2},
                                                                                     {"op3", OP3},
                                                                                     {"op4", OP4},
                                                                                     {"op5", OP5}};

} // namespace Antares::Data
