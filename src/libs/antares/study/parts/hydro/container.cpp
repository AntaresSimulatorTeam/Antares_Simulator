// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/study/parts/hydro/container.h"

#include <boost/algorithm/string/case_conv.hpp>

#include <antares/inifile/inifile.h>
#include <antares/study/area/capacityReservation.h>
#include <antares/study/parts/reserves/makeGroupsOfSymmetriesFromString.h>
#include "antares/study/parts/reserves/reservesParticipationsLoader.h"
#include "antares/study/study.h"

namespace fs = std::filesystem;

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
    overflowSpilledCostDifference = 1.;

    inflowPattern.reset(1, DAYS_PER_YEAR);
    inflowPattern.fillColumn(0, 1.0);
    waterValues.reset(101, DAYS_PER_YEAR);
    dailyNbHoursAtGenPmax.reset(1, DAYS_PER_YEAR);
    dailyNbHoursAtGenPmax.fillColumn(0, 24.);
    dailyNbHoursAtPumpPmax.reset(1, DAYS_PER_YEAR);
    dailyNbHoursAtPumpPmax.fillColumn(0, 24.);
    creditModulation.reset(101, 2);
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
        boost::to_lower(id);

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

bool PartHydro::LoadIniFile(Study& study, const std::filesystem::path& folder)
{
    bool ret = true;
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

    if (IniFile::Section* section = ini.find("inter-monthly-breakdown"))
    {
        ret = loadProperties(study, section->firstProperty, path, &PartHydro::intermonthlyBreakdown)
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

    if (IniFile::Section* section = ini.find("overflow spilled cost difference"))
    {
        ret = loadProperties(study,
                             section->firstProperty,
                             path,
                             &PartHydro::overflowSpilledCostDifference)
              && ret;
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
              ret = area.hydro.LoadDailyMaxEnergy(folder.string(), area.id) && ret;

              // Check is moved here, because in case of old study
              // dailyNbHoursAtGenPmax and dailyNbHoursAtPumpPmax are not yet initialized.

              ret = area.hydro.CheckDailyMaxEnergy(area.name) && ret;
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
    ret = PartHydro::LoadIniFile(study, folder) && ret;
    return ret;
}

bool PartHydro::checkInflowPatternAndCredModul(const Study& study)
{
    bool ret = true;

    for (const auto& [areaName, area]: study.areas)
    {
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
    bool ret = checkInflowPatternAndCredModul(study);
    return checkProperties(study) && ret;
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

bool PartHydro::CheckDailyMaxEnergy(const std::string& areaName)
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

bool PartHydro::loadReserveParticipations(Area& area, const std::filesystem::path& file)
{
    HydroReserveLoader loader;
    return loader.load(area, file);
}

unsigned int PartHydro::reserveParticipationsCount() const
{
    return reserveParticipationContainer
             ? reserveParticipationContainer->reserveParticipationsCount()
             : 0;
}

std::optional<ReserveID> PartHydro::reserveParticipationAt(const Area* area,
                                                           unsigned int index) const
{
    int globalReserveParticipationIdx = 0;

    for (const auto& reserveID:
         area->allCapacityReservations->areaCapacityReservations | std::views::keys)
    {
        if (reserveParticipationContainer->isParticipatingInReserve(reserveID))
        {
            if (static_cast<unsigned int>(globalReserveParticipationIdx) == index)
            {
                return reserveID;
            }
            globalReserveParticipationIdx++;
        }
    }
    return std::nullopt;
}

unsigned int PartHydro::count() const
{
    // Retournez 1 si le stockage long terme est activé, 0 sinon
    return series->TScount() ? 1 : 0;
}

double getWaterValue(const double& level /* format : in % of reservoir capacity */,
                     const Matrix<double>& waterValues,
                     const unsigned int day)
{
    if (level < 0. - 1e-6 || level > 100. + 1e-6)
    {
        logs.error() << "Invalid level for water values: " << level;
    }
    int levelDown = floor(level);

    // if level has value like -0.000001 because of numerical precision problems and we ceil it
    if (levelDown < 0)
    {
        levelDown = 0;
    }

    // special case, we don't want to use the last layer
    if (levelDown >= 100)
    {
        levelDown = 99;
    }
    return waterValues[levelDown][day];
}

double getWeeklyModulation(const double& level /* format : in % of reservoir capacity */,
                           Matrix<double, double>& creditMod,
                           int modType)
{
    if (level < 0. - 1e-6 || level > 100. + 1e-6)
    {
        logs.error() << "Invalid level for weekly modulation: " << level;
    }
    double valueToReturn = 1.;
    double levelUp = ceil(level);
    double levelDown = floor(level);

    // if level has value like 100.0000001 because of numerical precision problems and we ceil it
    if (levelDown < 0)
    {
        levelDown = 0;
    }
    if (levelUp > 100)
    {
        levelUp = 100;
    }

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
