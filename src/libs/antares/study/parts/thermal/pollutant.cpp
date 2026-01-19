// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
