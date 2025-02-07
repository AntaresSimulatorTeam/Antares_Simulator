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

#include "antares/study/parts/hydro/container.h"

#include <antares/inifile/inifile.h>
#include "antares/study/parts/hydro/hydromaxtimeseriesreader.h"
#include "antares/study/study.h"

namespace fs = std::filesystem;

#define SEP Yuni::IO::Separator

namespace Antares::Data
{
PartHydro::PartHydro():
    interDailyBreakdown(0.),
    intraDailyModulation(2.),
    intermonthlyBreakdown(0),
    reservoirManagement(false),
    followLoadModulations(true),
    useWaterValue(false),
    hardBoundsOnRuleCurves(false),
    useHeuristicTarget(true),
    reservoirCapacity(0.),
    initializeReservoirLevelDate(0),
    useLeeway(false),
    powerToLevel(false),
    leewayLowerBound(1.),
    leewayUpperBound(1.),
    pumpingEfficiency(1.),
    series(nullptr)
{
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
    dailyNbHoursAtGenPmax.reset(1, DAYS_PER_YEAR, true);
    dailyNbHoursAtGenPmax.fillColumn(0, 24.);
    dailyNbHoursAtPumpPmax.reset(1, DAYS_PER_YEAR, true);
    dailyNbHoursAtPumpPmax.fillColumn(0, 24.);
    creditModulation.reset(101, 2, true);
    creditModulation.fill(1);
    // reset of the hydro allocation - however we don't have any information
    // about the current area, which should be by default 1.
    // This work is done in Area::reset()
    allocation.clear();
    // allocation.fromArea(<current area>, 1.); // Area::reset()

    if (prepro)
    {
        prepro->reset();
    }
    if (series)
    {
        series->reset();
    }
}

template<class T>
static bool loadProperties(Study& study,
                           IniFile::Property* property,
                           const fs::path& filename,
                           T PartHydro::*ptr)
{
    if (!property)
    {
        return false;
    }

    bool ret = true;

    // Browse all properties
    for (; property; property = property->next)
    {
        AreaName id = property->key;
        id.toLower();

        Area* area = study.areas.find(id);
        if (area)
        {
            ret = property->value.to<T>(area->hydro.*ptr) && ret;
        }
        else
        {
            logs.warning() << filename << ": `" << id << "`: Unknown area";
            return false;
        }
    }
    return ret;
}

bool PartHydro::LoadFromFolder(Study& study, const fs::path& folder)
{
    bool ret = true;

    // Initialize all alpha values to 0
    study.areas.each(
      [&ret, &study, &folder](Data::Area& area)
      {
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
          area.hydro.deltaBetweenFinalAndInitialLevels.resize(study.parameters.nbYears);

          if (study.parameters.compatibility.hydroPmax
              == Parameters::Compatibility::HydroPmax::Hourly)
          {
              // GUI part patch :
              // We need to know, when estimating the RAM required by the solver, if the current
              // area is hydro modulable. Therefore, reading the area's daily max power at this
              // stage is necessary.

              if (!study.usedByTheSolver)
              {
                  bool enabledModeIsChanged = false;
                  if (JIT::enabled)
                  {
                      JIT::enabled = false; // Allowing to read the area's daily max power
                      enabledModeIsChanged = true;
                  }

                  ret = area.hydro.LoadDailyMaxEnergy(folder.string(), area.id) && ret;

                  if (enabledModeIsChanged)
                  {
                      JIT::enabled = true; // Back to the previous loading mode.
                  }
              }
              else
              {
                  ret = area.hydro.LoadDailyMaxEnergy(folder.string(), area.id) && ret;

                  // Check is moved here, because in case of old study
                  // dailyNbHoursAtGenPmax and dailyNbHoursAtPumpPmax are not yet initialized.

                  ret = area.hydro.CheckDailyMaxEnergy(area.name) && ret;
              }
          }

          fs::path capacityPath = folder / "common" / "capacity";

          std::string creditId = "creditmodulations_" + area.id + ".txt";
          fs::path creditPath = capacityPath / creditId;
          ret = area.hydro.creditModulation.loadFromCSVFile(creditPath.string(),
                                                            101,
                                                            2,
                                                            Matrix<>::optFixedSize,
                                                            &study.dataBuffer)
                && ret;

          std::string reservoirId = "reservoir_" + area.id + ".txt";
          fs::path reservoirPath = capacityPath / reservoirId;
          ret = area.hydro.reservoirLevel.loadFromCSVFile(reservoirPath.string(),
                                                          3,
                                                          DAYS_PER_YEAR,
                                                          Matrix<>::optFixedSize,
                                                          &study.dataBuffer)
                && ret;

          std::string waterValueId = "waterValues_" + area.id + ".txt";
          fs::path waterValuePath = capacityPath / waterValueId;
          ret = area.hydro.waterValues.loadFromCSVFile(waterValuePath.string(),
                                                       101,
                                                       DAYS_PER_YEAR,
                                                       Matrix<>::optFixedSize,
                                                       &study.dataBuffer)
                && ret;

          std::string inflowId = "inflowPattern_" + area.id + ".txt";
          fs::path inflowPath = capacityPath / inflowId;
          ret = area.hydro.inflowPattern.loadFromCSVFile(inflowPath.string(),
                                                         1,
                                                         DAYS_PER_YEAR,
                                                         Matrix<>::optFixedSize,
                                                         &study.dataBuffer)
                && ret;
      });

    IniFile ini;
    auto path = folder / "hydro.ini";
    if (not ini.open(path))
    {
        return false;
    }

    if (IniFile::Section* section = ini.find("inter-daily-breakdown"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::interDailyBreakdown)
              && ret;
    }

    if (IniFile::Section* section = ini.find("intra-daily-modulation"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::intraDailyModulation)
              && ret;
    }

    if (IniFile::Section* section = ini.find("reservoir"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::reservoirManagement)
              && ret;
    }

    if (IniFile::Section* section = ini.find("reservoir capacity"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::reservoirCapacity)
              && ret;
    }

    if (IniFile::Section* section = ini.find("follow load"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::followLoadModulations)
              && ret;
    }

    if (IniFile::Section* section = ini.find("use water"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::useWaterValue) && ret;
    }

    if (IniFile::Section* section = ini.find("hard bounds"))
    {
        ret = loadProperties(study,
                             section->firstProperty,
                             path,
                             &PartHydro::hardBoundsOnRuleCurves)
              && ret;
    }

    if (IniFile::Section* section = ini.find("use heuristic"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::useHeuristicTarget)
              && ret;
    }

    if (IniFile::Section* section = ini.find("power to level"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::powerToLevel) && ret;
    }

    if (IniFile::Section* section = ini.find("initialize reservoir date"))
    {
        ret = loadProperties(study,
                             section->firstProperty,
                             path,
                             &PartHydro::initializeReservoirLevelDate)
              && ret;
    }

    if (IniFile::Section* section = ini.find("use leeway"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::useLeeway) && ret;
    }

    if (IniFile::Section* section = ini.find("leeway low"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::leewayLowerBound)
              && ret;
    }

    if (IniFile::Section* section = ini.find("leeway up"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::leewayUpperBound)
              && ret;
    }

    if (IniFile::Section* section = ini.find("pumping efficiency"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::pumpingEfficiency)
              && ret;
    }

    return ret;
}

bool PartHydro::checkReservoirLevels(const Study& study)
{
    bool ret = true;

    for (const auto& [areaName, area]: study.areas)
    {
        if (!study.usedByTheSolver)
        {
            return true;
        }

        auto& col = area->hydro.inflowPattern[0];
        bool errorInflow = false;
        for (unsigned int day = 0; day < DAYS_PER_YEAR; day++)
        {
            if (col[day] < 0 && !errorInflow)
            {
                logs.error() << areaName << ": invalid inflow value";
                errorInflow = true;
                ret = false;
            }
        }
        bool errorLevels = false;
        auto& colMin = area->hydro.reservoirLevel[minimum];
        auto& colAvg = area->hydro.reservoirLevel[average];
        auto& colMax = area->hydro.reservoirLevel[maximum];
        for (unsigned int day = 0; day < DAYS_PER_YEAR; day++)
        {
            if (!errorLevels
                && (colMin[day] < 0 || colAvg[day] < 0 || colMin[day] > colMax[day]
                    || colAvg[day] > 100 || colMax[day] > 100))
            {
                logs.error() << areaName << ": invalid reservoir level value";
                errorLevels = true;
                ret = false;
            }
        }

        for (int i = 0; i < 101; i++)
        {
            if ((area->hydro.creditModulation[i][0] < 0)
                || (area->hydro.creditModulation[i][1] < 0))
            {
                logs.error() << areaName << ": invalid credit modulation value";
                ret = false;
            }
        }
    }

    return ret;
}

bool PartHydro::checkProperties(Study& study)
{
    bool ret = true;

    // Check on reservoir capacity (has to be done after reservoir management and capacity reading,
    // not before). Some areas reservoir capacities may not be printed in hydro ini file when saving
    // the study, because they are too small (< 1e-6). We cannot have reservoir management = yes and
    // capacity = 0 because of further division by capacity. reservoir management = no and capacity
    // = 0 is possible (no use of capacity further)
    study.areas.each(
      [&ret](Data::Area& area)
      {
          if (area.hydro.reservoirCapacity < 1e-3 && area.hydro.reservoirManagement)
          {
              logs.error() << area.name
                           << ": reservoir capacity not defined. Impossible to manage.";
              ret = false;
          }

          if (!area.hydro.useHeuristicTarget && !area.hydro.useWaterValue)
          {
              logs.error() << area.name
                           << " : use water value = no conflicts with use heuristic target = no";
              ret = false;
          }

          if (area.hydro.intraDailyModulation < 1.)
          {
              logs.error() << area.id << ": Invalid intra-daily modulation. It must be >= 1.0, Got "
                           << area.hydro.intraDailyModulation << " (truncated to 1)";
              area.hydro.intraDailyModulation = 1.;
          }

          if (area.hydro.reservoirCapacity < 0)
          {
              logs.error() << area.id << ": Invalid reservoir capacity.";
              area.hydro.reservoirCapacity = 0.;
          }

          if (area.hydro.intermonthlyBreakdown < 0)
          {
              logs.error() << area.id << ": Invalid intermonthly breakdown";
              area.hydro.intermonthlyBreakdown = 0.;
          }

          if (area.hydro.initializeReservoirLevelDate < 0)
          {
              logs.error() << area.id << ": Invalid initialize reservoir date";
              area.hydro.initializeReservoirLevelDate = 0;
          }

          if (area.hydro.leewayLowerBound < 0.)
          {
              logs.error() << area.id << ": Invalid leeway lower bound. It must be >= 0.0, Got "
                           << area.hydro.leewayLowerBound;
              area.hydro.leewayLowerBound = 0.;
          }

          if (area.hydro.leewayUpperBound < 0.)
          {
              logs.error() << area.id << ": Invalid leeway upper bound. It must be >= 0.0, Got "
                           << area.hydro.leewayUpperBound;
              area.hydro.leewayUpperBound = 0.;
          }

          if (area.hydro.leewayLowerBound > area.hydro.leewayUpperBound)
          {
              logs.error() << area.id << ": Leeway lower bound greater than leeway upper bound.";
          }

          if (area.hydro.pumpingEfficiency < 0)
          {
              logs.error() << area.id << ": Invalid pumping efficiency";
              area.hydro.pumpingEfficiency = 0.;
          }
      });

    return ret;
}

bool PartHydro::validate(Study& study)
{
    bool ret = checkReservoirLevels(study);
    return checkProperties(study) && ret;
}

bool PartHydro::SaveToFolder(const AreaList& areas, const AnyString& folder)
{
    if (!folder)
    {
        logs.error() << "hydro: invalid empty folder";
        assert(false && "invalid empty folder");
        return false;
    }

    Yuni::String buffer;
    buffer.clear() << folder << SEP << "common" << SEP << "capacity";

    struct AllSections
    {
        IniFile::Section* s;
        IniFile::Section* smod;
        IniFile::Section* sIMB;
        IniFile::Section* sreservoir;
        IniFile::Section* sreservoirCapacity;
        IniFile::Section* sFollowLoad;
        IniFile::Section* sUseWater;
        IniFile::Section* sHardBounds;
        IniFile::Section* sInitializeReservoirDate;
        IniFile::Section* sUseHeuristic;
        IniFile::Section* sUseLeeway;
        IniFile::Section* sPowerToLevel;
        IniFile::Section* sLeewayLow;
        IniFile::Section* sLeewayUp;
        IniFile::Section* spumpingEfficiency;

        AllSections(IniFile& ini):
            s(ini.addSection("inter-daily-breakdown")),
            smod(ini.addSection("intra-daily-modulation")),
            sIMB(ini.addSection("inter-monthly-breakdown")),
            sreservoir(ini.addSection("reservoir")),
            sreservoirCapacity(ini.addSection("reservoir capacity")),
            sFollowLoad(ini.addSection("follow load")),
            sUseWater(ini.addSection("use water")),
            sHardBounds(ini.addSection("hard bounds")),
            sInitializeReservoirDate(ini.addSection("initialize reservoir date")),
            sUseHeuristic(ini.addSection("use heuristic")),
            sUseLeeway(ini.addSection("use leeway")),
            sPowerToLevel(ini.addSection("power to level")),
            sLeewayLow(ini.addSection("leeway low")),
            sLeewayUp(ini.addSection("leeway up")),
            spumpingEfficiency(ini.addSection("pumping efficiency"))
        {
        }
    };

    // Init
    IniFile ini;
    AllSections allSections(ini);

    // return status
    bool ret = true;

    // Add all alpha values for each area
    areas.each(
      [&allSections, &buffer, &folder, &ret](const Data::Area& area)
      {
          allSections.s->add(area.id, area.hydro.interDailyBreakdown);
          allSections.smod->add(area.id, area.hydro.intraDailyModulation);
          allSections.sIMB->add(area.id, area.hydro.intermonthlyBreakdown);
          allSections.sInitializeReservoirDate->add(area.id,
                                                    area.hydro.initializeReservoirLevelDate);
          allSections.sLeewayLow->add(area.id, area.hydro.leewayLowerBound);
          allSections.sLeewayUp->add(area.id, area.hydro.leewayUpperBound);
          allSections.spumpingEfficiency->add(area.id, area.hydro.pumpingEfficiency);
          if (area.hydro.reservoirCapacity > 1e-6)
          {
              allSections.sreservoirCapacity->add(area.id, area.hydro.reservoirCapacity);
          }
          if (area.hydro.reservoirManagement)
          {
              allSections.sreservoir->add(area.id, true);
          }
          if (!area.hydro.followLoadModulations)
          {
              allSections.sFollowLoad->add(area.id, false);
          }
          if (area.hydro.useWaterValue)
          {
              allSections.sUseWater->add(area.id, true);
          }
          if (area.hydro.hardBoundsOnRuleCurves)
          {
              allSections.sHardBounds->add(area.id, true);
          }
          if (!area.hydro.useHeuristicTarget)
          {
              allSections.sUseHeuristic->add(area.id, false);
          }
          if (area.hydro.useLeeway)
          {
              allSections.sUseLeeway->add(area.id, true);
          }
          if (area.hydro.powerToLevel)
          {
              allSections.sPowerToLevel->add(area.id, true);
          }

          // max hours gen
          buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                         << "maxDailyGenEnergy_" << area.id << ".txt";
          ret = area.hydro.dailyNbHoursAtGenPmax.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
          // max hours pump
          buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                         << "maxDailyPumpEnergy_" << area.id << ".txt";
          ret = area.hydro.dailyNbHoursAtPumpPmax.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
          // credit modulations
          buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                         << "creditmodulations_" << area.id << ".txt";
          ret = area.hydro.creditModulation.saveToCSVFile(buffer, /*decimal*/ 2) && ret;
          // inflow pattern
          buffer.clear() << folder << SEP << "common" << SEP << "capacity" << SEP
                         << "inflowPattern_" << area.id << ".txt";
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

bool PartHydro::forceReload(bool reload) const
{
    bool ret = true;
    ret = creditModulation.forceReload(reload) && ret;
    ret = inflowPattern.forceReload(reload) && ret;
    ret = reservoirLevel.forceReload(reload) && ret;
    ret = waterValues.forceReload(reload) && ret;
    ret = dailyNbHoursAtGenPmax.forceReload(reload) && ret;
    ret = dailyNbHoursAtPumpPmax.forceReload(reload) && ret;

    if (series)
    {
        ret = series->forceReload(reload) && ret;
    }
    if (prepro)
    {
        ret = prepro->forceReload(reload) && ret;
    }

    return ret;
}

void PartHydro::markAsModified() const
{
    inflowPattern.markAsModified();
    reservoirLevel.markAsModified();
    waterValues.markAsModified();
    creditModulation.markAsModified();
    dailyNbHoursAtGenPmax.markAsModified();
    dailyNbHoursAtPumpPmax.markAsModified();

    if (series)
    {
        series->markAsModified();
    }
    if (prepro)
    {
        prepro->markAsModified();
    }
}

void PartHydro::copyFrom(const PartHydro& rhs)
{
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

    // max daily gen
    {
        dailyNbHoursAtGenPmax = rhs.dailyNbHoursAtGenPmax;
        dailyNbHoursAtGenPmax.unloadFromMemory();
        rhs.dailyNbHoursAtGenPmax.unloadFromMemory();
    }

    // max daily pump
    {
        dailyNbHoursAtPumpPmax = rhs.dailyNbHoursAtPumpPmax;
        dailyNbHoursAtPumpPmax.unloadFromMemory();
        rhs.dailyNbHoursAtPumpPmax.unloadFromMemory();
    }
}

bool PartHydro::LoadDailyMaxEnergy(const fs::path& folder, const std::string& areaid)
{
    Matrix<>::BufferType fileContent;
    bool ret = true;

    fs::path genPath = folder / "common" / "capacity" / ("maxDailyGenEnergy_" + areaid + ".txt");
    ret = dailyNbHoursAtGenPmax.loadFromCSVFile(genPath.string(),
                                                1,
                                                DAYS_PER_YEAR,
                                                Matrix<>::optFixedSize,
                                                &fileContent)
          && ret;

    fs::path pumpPath = folder / "common" / "capacity" / ("maxDailyPumpEnergy_" + areaid + ".txt");
    ret = dailyNbHoursAtPumpPmax.loadFromCSVFile(pumpPath.string(),
                                                 1,
                                                 DAYS_PER_YEAR,
                                                 Matrix<>::optFixedSize,
                                                 &fileContent)
          && ret;

    return ret;
}

bool PartHydro::CheckDailyMaxEnergy(const AnyString& areaName)
{
    bool ret = true;
    bool errorEnergy = false;
    auto& colGen = dailyNbHoursAtGenPmax[0];
    auto& colPump = dailyNbHoursAtPumpPmax[0];

    for (unsigned int day = 0; day < DAYS_PER_YEAR; day++)
    {
        if (!errorEnergy && (colGen[day] < 0 || (colGen[day] > 24)))
        {
            logs.error() << areaName << ": invalid maximum generation energy value";
            errorEnergy = true;
            ret = false;
        }

        if (!errorEnergy && (colPump[day] < 0 || (colPump[day] > 24)))
        {
            logs.error() << areaName << ": invalid maximum pumping energy value";
            errorEnergy = true;
            ret = false;
        }
    }

    return ret;
}

double getWaterValue(const double& level /* format : in % of reservoir capacity */,
                     const Matrix<double>& waterValues,
                     const uint day)
{
    assert((level >= 0. && level <= 100.) && "getWaterValue function : invalid level");
    double levelUp = ceil(level);
    double levelDown = floor(level);

    if ((int)(levelUp) == (int)(levelDown))
    {
        return waterValues[(int)(levelUp)][day];
    }
    return waterValues[(int)(levelUp)][day] * (level - levelDown)
           + waterValues[(int)(levelDown)][day] * (levelUp - level);
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
    {
        valueToReturn = creditMod[(int)(levelUp)][modType];
    }
    else
    {
        valueToReturn = creditMod[(int)(levelUp)][modType] * (level - levelDown)
                        + creditMod[(int)(levelDown)][modType] * (levelUp - level);
    }

    return valueToReturn;
}

} // namespace Antares::Data
