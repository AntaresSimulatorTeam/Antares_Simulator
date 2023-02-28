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

#include "pollutant.h"

namespace Antares::Data
{

const std::map<enum Pollutant::PollutantList, std::string> Pollutant::pollutantNames =
{
    {CO2, "CO2 EMIS."},
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
    {OP5, "OP5 EMIS."},
    {POLLUTANT_MAX, "POLLUTANT_MAX"}
};

const std::string Pollutant::getPollutantName(int index)
{
    return pollutantNames.at(PollutantList(index));
}

const char* Pollutant::getPollutantNameChar(int index)
{
    return pollutantNames.at(PollutantList(index)).c_str();
}

void Pollutant::saveEmissionsFactor(IniFile::Section* s) const
{
    s->add("co2", emissionFactors[CO2]);
    s->add("nh3", emissionFactors[NH3]);
    s->add("so2", emissionFactors[SO2]);
    s->add("nox", emissionFactors[NOX]);
    s->add("pm2_5", emissionFactors[PM2_5]);
    s->add("pm5", emissionFactors[PM5]);
    s->add("pm10", emissionFactors[PM10]);
    s->add("nmvoc", emissionFactors[NMVOC]);
    s->add("op1", emissionFactors[OP1]);
    s->add("op2", emissionFactors[OP2]);
    s->add("op3", emissionFactors[OP3]);
    s->add("op4", emissionFactors[OP4]);
    s->add("op5", emissionFactors[OP5]);
}

} //namespace Antares::Data
