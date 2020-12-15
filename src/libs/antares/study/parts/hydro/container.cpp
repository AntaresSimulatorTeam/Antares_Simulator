/*
** Copyright 2007-2018 RTE
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

#include "../../study.h"
#include "container.h"
#include "../../../inifile.h"
#include "../../../array/array1d.h"

using namespace Antares;
using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
namespace Data
{
PartHydro::PartHydro() :
 interDailyBreakdown(0.),
 intraDailyModulation(2.),
 intermonthlyBreakdown(0),
 reservoirManagement(false),
 followLoadModulations(true),
 useWaterValue(false),
 hardBoundsOnRuleCurves(false),
 useHeuristicTarget(true),
 initializeReservoirLevelDate(0),
 reservoirCapacity(0.),
 useLeeway(false),
 leewayLowerBound(1.),
 leewayUpperBound(1.),
 pumpingEfficiency(1.),
 prepro(nullptr),
 series(nullptr),
 hydroModulable(false),
 powerToLevel(false)
{
}

PartHydro::~PartHydro()
{
    delete prepro;
    delete series;
}

void PartHydro::reset()
{
    intraDailyModulation = 24;
    interDailyBreakdown = 1.;
    intermonthlyBreakdown = 1.;

    reservoirManagement = false;
    reservoirCapacity = 0.;

    followLoadModulations = true;
    useWaterValue = false;
    hardBoundsOnRuleCurves = false;
    useHeuristicTarget = true;
    initializeReservoirLevelDate = 0;
    useLeeway = false;
    powerToLevel = false;
    leewayLowerBound = 1.;
    leewayUpperBound = 1.;

    inflowPattern.reset(1, DAYS_PER_YEAR, true);
    inflowPattern.fillColumn(0, 1.0);
    reservoirLevel.reset(3, DAYS_PER_YEAR, true);
    reservoirLevel.fillColumn(average, 0.5);
    reservoirLevel.fillColumn(maximum, 1.);
    waterValues.reset(101, DAYS_PER_YEAR, true);
    maxPower.reset(4, DAYS_PER_YEAR, true);
    maxPower.fillColumn(genMaxE, 24.);
    maxPower.fillColumn(pumpMaxE, 24.);
    creditModulation.reset(101, 2, true);
    creditModulation.fill(1);
    // reset of the hydro allocation - however we don't have any information
    // about the current area, which should be by default 1.
    // This work is done in Area::reset()
    allocation.clear();
    // allocation.fromArea(<current area>, 1.); // Area::reset()

    if (prepro)
        prepro->reset();
    if (series)
        series->reset();
}

bool PartHydro::LoadFromFolder(Study& study, const AnyString& folder)
{
    auto& buffer = study.bufferLoadingTS;
    bool ret = true;

    // Initialize all alpha values to 0
    study.areas.each([&](Data::Area& area) {
        area.hydro.interDailyBreakdown = 1.;
        area.hydro.intraDailyModulation = 24.;
        area.hydro.intermonthlyBreakdown = 1.;
        area.hydro.reservoirManagement = false;
        area.hydro.followLoadModulations = true;
        area.hydro.useWaterValue = false;
        area.hydro.hardBoundsOnRuleCurves = false;
        area.hydro.useHeuristicTarget = true;
        area.hydro.useLeeway = false;
        area.hydro.powerToLevel = false;
        area.hydro.leewayLowerBound = 1.;
        area.hydro.leewayUpperBound = 1.;
        area.hydro.initializeReservoirLevelDate = 0;
        area.hydro.reservoirCapacity = 0.;
        area.hydro.pumpingEfficiency = 1.;

        // maximum capacity expectation
        if (study.header.version < 390)
        {
            area.hydro.maxPower.reset(4, DAYS_PER_YEAR, true);

            buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                           << "maxcapacityexpectation_" << area.id << '.' << study.inputExtension;
            Matrix<uint> array;
            if (array.loadFromCSVFile(buffer,
                                      1,
                                      12,
                                      Matrix<>::optFixedSize | Matrix<>::optImmediate,
                                      &study.dataBuffer))
            {
                // days per month, immutable values for version prior to 3.9 for sure
                // these values was hard-coded before 3.9
                static const uint daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

                uint dayYear = 0;
                for (uint m = 0; m != 12; ++m)
                {
                    uint nbDays = daysPerMonth[m];
                    uint power = array.entry[0][m];

                    for (uint d = 0; d != nbDays; ++d, ++dayYear)
                    {
                        for (uint x = 0; x != area.hydro.maxPower.width; ++x)
                            area.hydro.maxPower.entry[x][dayYear] = power;
                    }
                }
            }
            else
                area.hydro.maxPower.reset(4, DAYS_PER_YEAR, true);
            area.hydro.maxPower.markAsModified();
        }
        else
        {
            buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
                           << area.id << '.' << study.inputExtension;

            //	GUI part patch :
            //		We need to know, when estimating the RAM required by the solver, if the current
            //area 		is hydro modulable. Therefore, reading the area's daily max power at this stage
            //is 		necessary.
            if (!study.usedByTheSolver)
            {
                bool enabledModeIsChanged = false;
                if (JIT::enabled)
                {
                    JIT::enabled = false; // Allowing to read the area's daily max power
                    enabledModeIsChanged = true;
                }

                if (study.header.version >= 620)
                {
                    ret = area.hydro.maxPower.loadFromCSVFile(
                            buffer, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
                          && ret;
                }
                else
                {
                    ret = area.hydro.maxPower.loadFromCSVFile(
                            buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
                          && ret;
                    double temp[DAYS_PER_YEAR];
                    auto& max = area.hydro.maxPower[area.hydro.maximum];
                    for (int i = 0; i < DAYS_PER_YEAR; i++)
                    {
                        temp[i] = max[i];
                    }
                    area.hydro.maxPower.reset(4, DAYS_PER_YEAR, true);
                    area.hydro.maxPower.fillColumn(area.hydro.genMaxE, 24.);
                    area.hydro.maxPower.fillColumn(area.hydro.pumpMaxP, 0.0);
                    area.hydro.maxPower.fillColumn(area.hydro.pumpMaxE, 24.);
                    auto& maxP = area.hydro.maxPower[area.hydro.genMaxP];
                    for (int i = 0; i < DAYS_PER_YEAR; i++)
                    {
                        maxP[i] = temp[i];
                    }
                }

                if (enabledModeIsChanged)
                    JIT::enabled = true; // Back to the previous loading mode.
            }
            else
            {
                if (study.header.version >= 620)
                {
                    ret = area.hydro.maxPower.loadFromCSVFile(
                            buffer, 4, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
                          && ret;
                }
                else
                {
                    ret = area.hydro.maxPower.loadFromCSVFile(
                            buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
                          && ret;
                    double temp[DAYS_PER_YEAR];
                    auto& max = area.hydro.maxPower[area.hydro.maximum];
                    for (int i = 0; i < DAYS_PER_YEAR; i++)
                    {
                        temp[i] = max[i];
                    }
                    area.hydro.maxPower.reset(4, DAYS_PER_YEAR, true);
                    area.hydro.maxPower.fillColumn(area.hydro.genMaxE, 24.);
                    area.hydro.maxPower.fillColumn(area.hydro.pumpMaxP, 0.0);
                    area.hydro.maxPower.fillColumn(area.hydro.pumpMaxE, 24.);
                    auto& maxP = area.hydro.maxPower[area.hydro.genMaxP];
                    for (int i = 0; i < DAYS_PER_YEAR; i++)
                    {
                        maxP[i] = temp[i];
                    }
                }
            }
        }

        if (study.header.version >= 620)
        {
            buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                           << "creditmodulations_" << area.id << '.' << study.inputExtension;
            ret = area.hydro.creditModulation.loadFromCSVFile(
                    buffer, 101, 2, Matrix<>::optFixedSize, &study.dataBuffer)
                  && ret;
        }
        else
        {
            area.hydro.creditModulation.reset(101, 2, true);
            area.hydro.creditModulation.fill(1);
        }

        if (study.header.version >= 620)
        {
            buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "reservoir_"
                           << area.id << '.' << study.inputExtension;
            ret = area.hydro.reservoirLevel.loadFromCSVFile(
                    buffer, 3, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
                  && ret;
        }
        else if (study.header.version >= 390)
        {
            bool enabledModeIsChanged = false;
            if (JIT::enabled)
            {
                JIT::enabled = false; // Allowing to read the area's daily max power
                enabledModeIsChanged = true;
            }
            buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "reservoir_"
                           << area.id << '.' << study.inputExtension;
            ret = area.hydro.reservoirLevel.loadFromCSVFile(
                    buffer, 3, 12, Matrix<>::optFixedSize, &study.dataBuffer)
                  && ret;

            double temp[3][DAYS_PER_YEAR];
            static const uint daysPerMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            static const uint daysPerMonthR[] = {study.calendar.months[0].days,
                                                 study.calendar.months[1].days,
                                                 study.calendar.months[2].days,
                                                 study.calendar.months[3].days,
                                                 study.calendar.months[4].days,
                                                 study.calendar.months[5].days,
                                                 study.calendar.months[6].days,
                                                 study.calendar.months[7].days,
                                                 study.calendar.months[8].days,
                                                 study.calendar.months[9].days,
                                                 study.calendar.months[10].days,
                                                 study.calendar.months[11].days};
            uint daysPerMonthDecals[12];
            for (int oldMonth = 0; oldMonth < 12; oldMonth++)
            {
                int realMonth = (oldMonth + study.parameters.firstMonthInYear) % 12;
                daysPerMonthDecals[oldMonth] = daysPerMonth[realMonth];
                if (study.parameters.leapYear)
                {
                    if (realMonth == 1) // February
                    {
                        daysPerMonthDecals[oldMonth]++;
                    }
                    if (oldMonth == 11) // Last month of the year
                    {
                        daysPerMonthDecals[oldMonth]--;
                    }
                }
            }
            uint firstDayMonth[13];
            firstDayMonth[0] = 0;
            for (int i = 1; i < 13; i++)
            {
                firstDayMonth[i] = daysPerMonthDecals[i - 1] + firstDayMonth[i - 1];
            }
            for (int x = area.hydro.minimum; x <= area.hydro.maximum; x++)
            {
                auto& col = area.hydro.reservoirLevel[x];
                for (int month = 0; month < 12; month++)
                {
                    int realMonth = (month + study.parameters.firstMonthInYear) % 12;
                    double valDiff;
                    if (realMonth < 11)
                    {
                        valDiff = col[realMonth + 1] - col[realMonth];
                    }
                    else
                    {
                        valDiff = col[0] - col[realMonth];
                    }
                    for (uint day = firstDayMonth[month]; day < firstDayMonth[month + 1]; day++)
                    {
                        temp[x][day] = Math::Round(
                          col[realMonth]
                            + (day - firstDayMonth[month]) * (valDiff / daysPerMonthDecals[month]),
                          3);
                    }
                }
            }
            area.hydro.reservoirLevel.reset(3, DAYS_PER_YEAR, true);
            for (int x = area.hydro.minimum; x <= area.hydro.maximum; x++)
            {
                auto& col = area.hydro.reservoirLevel[x];
                for (int i = 0; i < DAYS_PER_YEAR; i++)
                {
                    col[i] = temp[x][i];
                }
            }

            if (enabledModeIsChanged)
                JIT::enabled = true; // Back to the previous loading mode.
        }
        else
        {
            area.hydro.reservoirLevel.reset(3, DAYS_PER_YEAR, true);
            area.hydro.reservoirLevel.fillColumn(area.hydro.minimum, 0.0);
            area.hydro.reservoirLevel.fillColumn(area.hydro.average, 0.5);
            area.hydro.reservoirLevel.fillColumn(area.hydro.maximum, 1.);
            area.hydro.reservoirLevel.markAsModified();
        }

        if (study.header.version >= 620)
        {
            buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                           << "waterValues_" << area.id << '.' << study.inputExtension;
            ret = area.hydro.waterValues.loadFromCSVFile(
                    buffer, 101, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
                  && ret;
        }
        else
        {
            area.hydro.waterValues.reset(101, DAYS_PER_YEAR, true);
            area.hydro.waterValues.markAsModified();
        }

        if (study.header.version >= 620)
        {
            buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                           << "inflowPattern_" << area.id << '.' << study.inputExtension;
            ret = area.hydro.inflowPattern.loadFromCSVFile(
                    buffer, 1, DAYS_PER_YEAR, Matrix<>::optFixedSize, &study.dataBuffer)
                  && ret;
        }
        else
        {
            area.hydro.inflowPattern.reset(1, DAYS_PER_YEAR, true);
            area.hydro.inflowPattern.fillColumn(0, 1.0);
            area.hydro.inflowPattern.markAsModified();
        }

        if (study.usedByTheSolver)
        {
            auto& col = area.hydro.inflowPattern[0];
            bool errorInflow = false;
            for (int day = 0; day < DAYS_PER_YEAR; day++)
            {
                if (col[day] < 0 && !errorInflow)
                {
                    logs.error() << area.name << ": invalid inflow value";
                    errorInflow = true;
                    ret = false;
                }
            }
            bool errorLevels = false;
            auto& colMin = area.hydro.reservoirLevel[minimum];
            auto& colAvg = area.hydro.reservoirLevel[average];
            auto& colMax = area.hydro.reservoirLevel[maximum];
            for (int day = 0; day < DAYS_PER_YEAR; day++)
            {
                if (!errorLevels
                    && (colMin[day] < 0 || colAvg[day] < 0 || colMin[day] > colMax[day]
                        || colAvg[day] > 100 || colMax[day] > 100))
                {
                    logs.error() << area.name << ": invalid reservoir level value";
                    errorLevels = true;
                    ret = false;
                }
            }
            bool errorPowers = false;
            for (int i = 0; i < 4; i++)
            {
                auto& col = area.hydro.maxPower[i];
                for (int day = 0; day < DAYS_PER_YEAR; day++)
                {
                    if (!errorPowers && (col[day] < 0 || (i % 2 /*column hours*/ && col[day] > 24)))
                    {
                        logs.error() << area.name << ": invalid power or energy value";
                        errorPowers = true;
                        ret = false;
                    }
                }
            }
            for (int i = 0; i < 101; i++)
            {
                if ((area.hydro.creditModulation[i][0] < 0)
                    || (area.hydro.creditModulation[i][1] < 0))
                {
                    logs.error() << area.name << ": invalid credit modulation value";
                    ret = false;
                }
            }
        }
        else
        {
            // Is area hydro modulable ?
            auto& max = area.hydro.maxPower[area.hydro.genMaxP];

            for (uint y = 0; y != area.hydro.maxPower.height; ++y)
            {
                if (max[y] > 0.)
                {
                    area.hydro.hydroModulable = true;
                    break;
                }
            }
        }
    });

    IniFile ini;
    if (not ini.open(buffer.clear() << folder << SEP << "hydro.ini"))
        return false;

    // The section name
    // Before 3.3, the smoothing factor was called 'alpha', for historical
    // reasons.
    // But, since 3.3, it has been renamed 'inter-daily breakdown' and a new
    // value 'intra-daily modulation' was added
    const char* const sectionName
      = (study.header.version <= 320) ? "alpha" : "inter-daily-breakdown";

    IniFile::Section* section;
    IniFile::Property* property;

    if ((section = ini.find(sectionName)))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                Area* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<double>(area->hydro.interDailyBreakdown) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }

    if (study.header.version >= 330 && (section = ini.find("intra-daily-modulation")))
    {
        if ((property = section->firstProperty))
        {
            AreaName id;

            // Browse all properties
            for (; property; property = property->next)
            {
                id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                {
                    ret = property->value.to<double>(area->hydro.intraDailyModulation) && ret;
                    if (area->hydro.intraDailyModulation < 1.)
                    {
                        logs.error()
                          << area->id << ": Invalid intra-daily modulation. It must be >= 1.0, Got "
                          << area->hydro.intraDailyModulation << " (truncated to 1)";
                        area->hydro.intraDailyModulation = 1.;
                    }
                }
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }

    if (study.header.version >= 390 && (section = ini.find("reservoir")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<bool>(area->hydro.reservoirManagement) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }

    if (study.header.version >= 390 && (section = ini.find("reservoir capacity")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                {
                    ret = property->value.to<double>(area->hydro.reservoirCapacity) && ret;
                    if (area->hydro.reservoirCapacity < 1e-6)
                    {
                        logs.error() << area->id << ": Invalid reservoir capacity.";
                        area->hydro.reservoirCapacity = 0.;
                    }
                }
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }

    // Check on reservoir capacity (has to be done after reservoir management and capacity reading,
    // not before). Some areas reservoir capacities may not be printed in hydro ini file when saving
    // the study, because they are too small (< 1e-6). We cannot have reservoir management = yes and
    // capacity = 0 because of further division by capacity. reservoir management = no and capacity
    // = 0 is possible (no use of capacity further)
    study.areas.each([&](Data::Area& area) {
        if (area.hydro.reservoirCapacity < 1e-3 && area.hydro.reservoirManagement)
        {
            logs.error() << area.name << ": reservoir capacity not defined. Impossible to manage.";
            ret = false && ret;
        }
    });

    if (study.header.version >= 390 && (section = ini.find("inter-monthly-breakdown")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                {
                    ret = property->value.to<double>(area->hydro.intermonthlyBreakdown) && ret;
                    if (area->hydro.intermonthlyBreakdown < 0)
                    {
                        logs.error() << area->id << ": Invalid intermonthly breakdown";
                        area->hydro.intermonthlyBreakdown = 0.;
                    }
                }
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if (study.header.version >= 620 && (section = ini.find("follow load")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<bool>(area->hydro.followLoadModulations) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if (study.header.version >= 620 && (section = ini.find("use water")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<bool>(area->hydro.useWaterValue) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if (study.header.version >= 620 && (section = ini.find("hard bounds")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<bool>(area->hydro.hardBoundsOnRuleCurves) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if (study.header.version >= 620 && (section = ini.find("use heuristic")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<bool>(area->hydro.useHeuristicTarget) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if (study.header.version >= 620 && (section = ini.find("power to level")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<bool>(area->hydro.powerToLevel) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if (study.header.version >= 620 && (section = ini.find("initialize reservoir date")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                {
                    ret = property->value.to<int>(area->hydro.initializeReservoirLevelDate) && ret;
                    if (area->hydro.initializeReservoirLevelDate < 0)
                    {
                        logs.error() << area->id << ": Invalid initialize reservoir date";
                        area->hydro.initializeReservoirLevelDate = 0;
                    }
                }
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    // Leeways : use leeway bounds (upper and lower)
    if (study.header.version >= 620 && (section = ini.find("use leeway")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                    ret = property->value.to<bool>(area->hydro.useLeeway) && ret;
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if ((study.header.version >= 620 && study.header.version < 640
         && (section = ini.find("leeway below")))
        || (study.header.version >= 640 && (section = ini.find("leeway low"))))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                {
                    ret = property->value.to<double>(area->hydro.leewayLowerBound) && ret;
                    if (area->hydro.leewayLowerBound < 0.)
                    {
                        logs.error()
                          << area->id << ": Invalid leeway lower bound. It must be >= 0.0, Got "
                          << area->hydro.leewayLowerBound;
                        area->hydro.leewayLowerBound = 0.;
                    }
                }
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if ((study.header.version >= 620 && study.header.version < 640
         && (section = ini.find("leeway above")))
        || (study.header.version >= 640 && (section = ini.find("leeway up"))))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                {
                    ret = property->value.to<double>(area->hydro.leewayUpperBound) && ret;
                    if (area->hydro.leewayUpperBound < 0.)
                    {
                        logs.error()
                          << area->id << ": Invalid leeway upper bound. It must be >= 0.0, Got "
                          << area->hydro.leewayUpperBound;
                        area->hydro.leewayUpperBound = 0.;
                    }
                }
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }
    if (study.header.version >= 620)
    {
        // they are too small (< 1e-6). We cannot allow these areas to have reservoir management =
        // true.
        study.areas.each([&](Data::Area& area) {
            if (area.hydro.leewayLowerBound > area.hydro.leewayUpperBound)
                logs.error() << area.id << ": Leeway lower bound greater than leeway upper bound.";
        });
    }

    if (study.header.version >= 620 && (section = ini.find("pumping efficiency")))
    {
        if ((property = section->firstProperty))
        {
            // Browse all properties
            for (; property; property = property->next)
            {
                AreaName id = property->key;
                id.toLower();

                auto* area = study.areas.find(id);
                if (area)
                {
                    ret = property->value.to<double>(area->hydro.pumpingEfficiency) && ret;
                    if (area->hydro.pumpingEfficiency < 0)
                    {
                        logs.error() << area->id << ": Invalid pumping efficiency";
                        area->hydro.pumpingEfficiency = 0.;
                    }
                }
                else
                    logs.warning() << buffer << ": `" << property->value << "`: Unknown area";
            }
        }
    }

    study.areas.each([&](Data::Area& area) {
        if (not area.hydro.useHeuristicTarget && not area.hydro.useWaterValue)
        {
            logs.error() << area.name
                         << " : use water value = no conflicts with use heuristic target = no";
            ret = false && ret;
        }
    });

    return ret;
}

bool PartHydro::SaveToFolder(const AreaList& areas, const AnyString& folder)
{
    if (!folder)
    {
        logs.error() << "hydro: invalid empty folder";
        assert(false && "invalid empty folder");
        return false;
    }

    String buffer;
    buffer.clear() << folder << SEP << "common" << SEP << "capacity";

    // Init
    IniFile ini;
    auto* s = ini.addSection("inter-daily-breakdown");
    auto* smod = ini.addSection("intra-daily-modulation");
    auto* sIMB = ini.addSection("inter-monthly-breakdown");
    auto* sreservoir = ini.addSection("reservoir");
    auto* sreservoirCapacity = ini.addSection("reservoir capacity");
    auto* sFollowLoad = ini.addSection("follow load");
    auto* sUseWater = ini.addSection("use water");
    auto* sHardBounds = ini.addSection("hard bounds");
    auto* sInitializeReservoirDate = ini.addSection("initialize reservoir date");
    auto* sUseHeuristic = ini.addSection("use heuristic");
    auto* sUseLeeway = ini.addSection("use leeway");
    auto* sPowerToLevel = ini.addSection("power to level");
    auto* sLeewayLow = ini.addSection("leeway low");
    auto* sLeewayUp = ini.addSection("leeway up");
    auto* spumpingEfficiency = ini.addSection("pumping efficiency");

    // return status
    bool ret = true;

    // Add all alpha values for each area
    areas.each([&](const Data::Area& area) {
        s->add(area.id, area.hydro.interDailyBreakdown);
        smod->add(area.id, area.hydro.intraDailyModulation);
        sIMB->add(area.id, area.hydro.intermonthlyBreakdown);
        sInitializeReservoirDate->add(area.id, area.hydro.initializeReservoirLevelDate);
        sLeewayLow->add(area.id, area.hydro.leewayLowerBound);
        sLeewayUp->add(area.id, area.hydro.leewayUpperBound);
        spumpingEfficiency->add(area.id, area.hydro.pumpingEfficiency);
        if (area.hydro.reservoirCapacity > 1e-6)
            sreservoirCapacity->add(area.id, area.hydro.reservoirCapacity);
        if (area.hydro.reservoirManagement)
            sreservoir->add(area.id, true);
        if (!area.hydro.followLoadModulations)
            sFollowLoad->add(area.id, false);
        if (area.hydro.useWaterValue)
            sUseWater->add(area.id, true);
        if (area.hydro.hardBoundsOnRuleCurves)
            sHardBounds->add(area.id, true);
        if (!area.hydro.useHeuristicTarget)
            sUseHeuristic->add(area.id, false);
        if (area.hydro.useLeeway)
            sUseLeeway->add(area.id, true);
        if (area.hydro.powerToLevel)
            sPowerToLevel->add(area.id, true);
        // max power
        buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "maxpower_"
                       << area.id << ".txt";
        ret = area.hydro.maxPower.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
        // credit modulations
        buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                       << "creditmodulations_" << area.id << ".txt";
        ret = area.hydro.creditModulation.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
        // inflow pattern
        buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "inflowPattern_"
                       << area.id << ".txt";
        ret = area.hydro.inflowPattern.saveToCSVFile(buffer, /*decimal*/ 3) && ret;
        // reservoir
        buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "reservoir_"
                       << area.id << ".txt";
        ret = area.hydro.reservoirLevel.saveToCSVFile(buffer, /*decimal*/ 3) && ret;
        buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP << "waterValues_"
                       << area.id << ".txt";
        ret = area.hydro.waterValues.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
    });

    // Write the ini file
    buffer.clear() << folder << SEP << "hydro.ini";
    return ini.save(buffer) && ret;
}

bool PartHydro::invalidate(bool reload) const
{
    bool ret = true;
    ret = maxPower.invalidate(reload) && ret;
    ret = creditModulation.invalidate(reload) && ret;
    ret = inflowPattern.invalidate(reload) && ret;
    ret = reservoirLevel.invalidate(reload) && ret;
    ret = waterValues.invalidate(reload) && ret;

    if (series)
        ret = series->invalidate(reload) && ret;
    if (prepro)
        ret = prepro->invalidate(reload) && ret;

    return ret;
}

void PartHydro::markAsModified() const
{
    maxPower.markAsModified();
    inflowPattern.markAsModified();
    reservoirLevel.markAsModified();
    waterValues.markAsModified();
    creditModulation.markAsModified();

    if (series)
        series->markAsModified();
    if (prepro)
        prepro->markAsModified();
}

void PartHydro::copyFrom(const PartHydro& rhs)
{
    // max power
    {
        maxPower = rhs.maxPower;
        maxPower.unloadFromMemory();
        rhs.maxPower.unloadFromMemory();
    }
    // credit modulations
    {
        creditModulation = rhs.creditModulation;
        creditModulation.unloadFromMemory();
        rhs.creditModulation.unloadFromMemory();
    }
    // inflow pattern
    {
        inflowPattern = rhs.inflowPattern;
        inflowPattern.unloadFromMemory();
        rhs.inflowPattern.unloadFromMemory();
    }
    // reservoir levels
    {
        reservoirLevel = rhs.reservoirLevel;
        reservoirLevel.unloadFromMemory();
        rhs.reservoirLevel.unloadFromMemory();
    }
    // water values
    {
        waterValues = rhs.waterValues;
        waterValues.unloadFromMemory();
        rhs.waterValues.unloadFromMemory();
    }
    // values
    {
        interDailyBreakdown = rhs.interDailyBreakdown;
        intraDailyModulation = rhs.intraDailyModulation;
        intermonthlyBreakdown = rhs.intermonthlyBreakdown;
        reservoirManagement = rhs.reservoirManagement;
        reservoirCapacity = rhs.reservoirCapacity;
        followLoadModulations = rhs.followLoadModulations;
        useWaterValue = rhs.useWaterValue;
        hardBoundsOnRuleCurves = rhs.hardBoundsOnRuleCurves;
        useHeuristicTarget = rhs.useHeuristicTarget;
        initializeReservoirLevelDate = rhs.initializeReservoirLevelDate;
        useLeeway = rhs.useLeeway;
        powerToLevel = rhs.powerToLevel;
        leewayUpperBound = rhs.leewayUpperBound;
        leewayLowerBound = rhs.leewayLowerBound;
        pumpingEfficiency = rhs.pumpingEfficiency;
    }
}

void getWaterValue(const double& level /* format : in % of reservoir capacity */,
                   const Matrix<double>& waterValues,
                   const uint day,
                   h2oValueWorkVarsType& workVar,
                   double& waterValueToReturn)
{
    assert((level >= 0. && level <= 100.) && "getWaterValue function : invalid level");
    workVar.levelUp = ceil(level);
    workVar.levelDown = floor(level);

    if ((int)(workVar.levelUp) == (int)(workVar.levelDown))
        waterValueToReturn = waterValues[(int)(workVar.levelUp)][day];
    else
        waterValueToReturn
          = waterValues[(int)(workVar.levelUp)][day] * (level - workVar.levelDown)
            + waterValues[(int)(workVar.levelDown)][day] * (workVar.levelUp - level);
}

double getWeeklyModulation(const double& level /* format : in % of reservoir capacity */,
                           Matrix<double, double>& creditMod,
                           int modType)
{
    assert((level >= 0. && level <= 100.) && "getWeeklyModulation function : invalid level");
    double valueToReturn = 1.;
    double levelUp = ceil(level);
    double levelDown = floor(level);
    if ((int)(levelUp) == (int)(levelDown))
        valueToReturn = creditMod[(int)(levelUp)][modType];
    else
        valueToReturn = creditMod[(int)(levelUp)][modType] * (level - levelDown)
                        + creditMod[(int)(levelDown)][modType] * (levelUp - level);

    return valueToReturn;
}

} // namespace Data
} // namespace Antares
