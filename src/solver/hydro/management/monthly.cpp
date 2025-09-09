/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <iomanip>
#include <limits>
#include <sstream>

#include <antares/antares/fatal-error.h>
#include <antares/study/area/scratchpad.h>
#include "antares/solver/hydro/management/management.h"
#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"

namespace fs = std::filesystem;
using namespace Antares::Data;

namespace Antares
{

#ifndef NDEBUG
static void CheckHydroAllocationProblem(Data::Area& area,
                                        DONNEES_ANNUELLES& problem,
                                        int initLevelMonth,
                                        double lvi)
{
    bool error = false;

    double X = problem.Volume[11] - problem.Turbine[11] + problem.Apport[11] - problem.overflow[11];
    if (!Utils::isZero(X - lvi))
    {
        logs.fatal() << area.id << ": hydro management: monthly: reservoir error";
        error = true;
    }

    if (error)
    {
        logs.warning() << area.id << ": lvi = " << lvi;
        logs.warning() << area.id << ": cost = " << problem.CoutDepassementVolume;
        for (uint month = 0; month != MONTHS_PER_YEAR; ++month)
        {
            uint realmonth = (initLevelMonth + month) % MONTHS_PER_YEAR;
            logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
                           << ", turb.max: " << problem.TurbineMax[realmonth]
                           << ", turb.cible: " << problem.TurbineCible[realmonth]
                           << ", apport: " << problem.Apport[realmonth] << ", volume: ["
                           << problem.VolumeMin[realmonth] << " .. " << problem.VolumeMax[realmonth]
                           << "]";
        }

        logs.info();
        problem.Volume[initLevelMonth] = lvi;
        for (uint month = 0; month != MONTHS_PER_YEAR; ++month)
        {
            uint realmonth = (initLevelMonth + month) % MONTHS_PER_YEAR;
            logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
                           << ", turbine: " << problem.Turbine[realmonth]
                           << ", volume: " << problem.Volume[realmonth];
        }
    }
}
#endif

double computeYearInflows(AreaDependantHydroManagementData& data)
{
    double yearInflows = 0;
    for (uint realmonth = 0; realmonth != 12; ++realmonth)
    {
        yearInflows += data.inflows[realmonth];
    }
    return yearInflows;
}

static std::array<double, 12> computeMonthlyTargetGenerations(
  Area& area,
  AreaDependantHydroManagementData& data,
  std::array<MonthlyGenerationTargetData, 12>& areaMonthlyData)
{
    std::array<double, 12> MTG{0};
    double yearInflows = computeYearInflows(data);

    if (not area.hydro.followLoadModulations)
    {
        for (uint realmonth = 0; realmonth != MONTHS_PER_YEAR; ++realmonth)
        {
            MTG[realmonth] = data.inflows[realmonth];
        }
        return MTG;
    }

    double monthlyMaxDemand = -std::numeric_limits<double>::infinity();

    for (uint realmonth = 0; realmonth != MONTHS_PER_YEAR; ++realmonth)
    {
        if (areaMonthlyData[realmonth].MLE > monthlyMaxDemand)
        {
            monthlyMaxDemand = areaMonthlyData[realmonth].MLE;
        }
    }

    if (!Utils::isZero(monthlyMaxDemand))
    {
        double coeff = 0.;
        for (uint realmonth = 0; realmonth != MONTHS_PER_YEAR; ++realmonth)
        {
            assert(areaMonthlyData[realmonth].MLE / monthlyMaxDemand >= 0.);
            coeff += std::pow(areaMonthlyData[realmonth].MLE / monthlyMaxDemand,
                              area.hydro.intermonthlyBreakdown);
        }

        if (!Utils::isZero(coeff))
        {
            coeff = yearInflows / coeff;
        }

        for (uint realmonth = 0; realmonth != MONTHS_PER_YEAR; ++realmonth)
        {
            assert(areaMonthlyData[realmonth].MLE / monthlyMaxDemand >= 0.);
            MTG[realmonth] = coeff
                             * std::pow(areaMonthlyData[realmonth].MLE / monthlyMaxDemand,
                                        area.hydro.intermonthlyBreakdown);
        }
    }
    else
    {
        double coeff = yearInflows / 12.;
        for (uint realmonth = 0; realmonth != MONTHS_PER_YEAR; ++realmonth)
        {
            MTG[realmonth] = coeff;
        }
    }
    return MTG;
}

static double calcTurbineMaxForMonth(unsigned simulationMonth,
                                     unsigned y,
                                     Data::Area& area,
                                     const Date::Calendar& calendar,
                                     Antares::Data::Area::ScratchMap& scratchmap)
{
    double turbine = 0;

    auto daysPerMonth = calendar.months[simulationMonth].days;
    unsigned firstDay = calendar.months[simulationMonth].daysYear.first;
    unsigned endDay = firstDay + daysPerMonth;
    auto& scratchpad = scratchmap.at(&area);
    const unsigned tsIndex = scratchpad.meanMaxDailyGenPower.getSeriesIndex(y);

    for (unsigned day = firstDay; day != endDay; ++day)
    {
        turbine += scratchpad.meanMaxDailyGenPower[tsIndex][day]
                   * area.hydro.dailyNbHoursAtGenPmax[0][day] / area.hydro.reservoirCapacity;
    }

    return turbine;
}

void HydroManagement::prepareMonthlyOptimalGenerations(
  const std::vector<double>& random_reservoir_level,
  uint y,
  Antares::Data::Area::ScratchMap& scratchmap,
  HydroSpecificMap& hydro_specific_map)
{
    uint indexArea = 0;
    areas_.each(
      [this, &random_reservoir_level, &y, &indexArea, &hydro_specific_map, &scratchmap](
        Data::Area& area)
      {
          auto& data = area.hydro.managementData[y];
          auto& areaMonthlyData = hydro_specific_map[&area].monthly;

          auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
          auto& maxLvl = area.hydro.reservoirLevel[Data::PartHydro::maximum];

          int initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate;

          double lvi = -1.;
          if (area.hydro.reservoirManagement)
          {
              lvi = random_reservoir_level[indexArea];
          }

          double solutionCost = 0.;
          double solutionCostNoised = 0.;
          std::array<double, 12> MTG{0};
          std::array<double, 12> OVF{0};

          if (area.hydro.reservoirManagement)
          {
              auto problem = DonneesOptimisationMensuelle::H2O_M_Instanciation(1);

              MTG = computeMonthlyTargetGenerations(area, data, areaMonthlyData);

              problem.CoutDepassementVolume = 1e2;
              problem.CoutViolMaxDuVolumeMin = 1e5;
              problem.overflowfCost = 1e6;
              problem.VolumeInitial = lvi;

              for (unsigned month = 0; month != MONTHS_PER_YEAR; ++month)
              {
                  uint realmonth = (initReservoirLvlMonth + month) % MONTHS_PER_YEAR;

                  unsigned simulationMonth = calendar_.mapping.months[realmonth];
                  unsigned firstDay = calendar_.months[simulationMonth].daysYear.first;

                  problem.TurbineMax[month] = calcTurbineMaxForMonth(simulationMonth,
                                                                     y,
                                                                     area,
                                                                     calendar_,
                                                                     scratchmap);

                  problem.TurbineMin[month] = data.mingens[realmonth];
                  problem.TurbineCible[month] = MTG[realmonth];
                  problem.Apport[month] = data.inflows[realmonth];
                  problem.VolumeMin[month] = minLvl[firstDay];
                  problem.VolumeMax[month] = maxLvl[firstDay];
              }

              DonneesOptimisationMensuelle::H2O_M_OptimiserUneAnnee(problem, 0);
              switch (problem.ResultatsValides)
              {
              case OUI:
              {
#ifndef NDEBUG
                  CheckHydroAllocationProblem(area, problem, initReservoirLvlMonth, lvi);
#endif

                  for (uint month = 0; month != MONTHS_PER_YEAR; ++month)
                  {
                      uint realmonth = (initReservoirLvlMonth + month) % MONTHS_PER_YEAR;
                      areaMonthlyData[realmonth].MOG = problem.Turbine[month]
                                                       * area.hydro.reservoirCapacity;
                      areaMonthlyData[realmonth].MOL = problem.Volume[month];
                      OVF[realmonth] = problem.overflow[month];
                  }
                  areaMonthlyData[initReservoirLvlMonth].MOL = lvi;
                  solutionCost = problem.ProblemeHydraulique.CoutDeLaSolution;
                  solutionCostNoised = problem.ProblemeHydraulique.CoutDeLaSolutionBruite;

                  break;
              }
              case NON:
              {
                  std::ostringstream msg;
                  msg << "Year : " << y + 1 << " - hydro: " << area.name
                      << " [month] no solution found";
                  throw FatalError(msg.str());
              }
              case EMERGENCY_SHUT_DOWN:
              {
                  std::ostringstream msg;
                  msg << "Year : " << y + 1 << " - hydro: " << area.name << " [month] fatal error";
                  throw FatalError(msg.str());
              }
              }
          }

          else
          {
              auto& reservoirLevel = area.hydro.reservoirLevel[Data::PartHydro::average];

              for (uint realmonth = 0; realmonth != MONTHS_PER_YEAR; ++realmonth)
              {
                  areaMonthlyData[realmonth].MOG = data.inflows[realmonth];
                  areaMonthlyData[realmonth].MOL = reservoirLevel[realmonth];
              }
          }

#ifndef NDEBUG
          for (uint realmonth = 0; realmonth != MONTHS_PER_YEAR; ++realmonth)
          {
              assert(!std::isnan(areaMonthlyData[realmonth].MOG) && "nan value detected for MOG");
              assert(!std::isnan(areaMonthlyData[realmonth].MOL) && "nan value detected for MOL");
              assert(!std::isinf(areaMonthlyData[realmonth].MOG)
                     && "infinite value detected for MOG");
              assert(!std::isinf(areaMonthlyData[realmonth].MOL)
                     && "infinite value detected for MOL");
          }
#endif
          if (parameters_.hydroDebug)
          {
              auto path = fs::path("debug") / "solver" / std::to_string(1 + y)
                          / ("monthly." + area.name + ".txt").c_str();

              std::ostringstream fileContent;
              if (area.hydro.reservoirManagement)
                  fileContent << "Initial Reservoir Level\t" << lvi << "\n";
              else
                  fileContent << "Initial Reservoir Level : unrelevant (no reservoir mgmt)\n";
              fileContent << "\n";

              auto writeSolutionCost = [&fileContent](const std::string& caption, double cost)
              {
                  auto precision = fileContent.precision();
                  fileContent << caption << std::fixed << std::setprecision(13) << cost;
                  fileContent << std::setprecision(precision) << std::defaultfloat;
              };
              writeSolutionCost("Solution cost : ", solutionCost);
              writeSolutionCost("Solution cost (noised) : ", solutionCostNoised);
              fileContent << "\n\n";

              fileContent << '\t' << "\tInflows" << '\t' << "\tTarget Gen." << "\tTurbined"
                          << "\tOVF" << "\tLevels" << '\t' << "\tLvl min" << '\t' << "\tLvl max\n";
              for (uint month = 0; month != MONTHS_PER_YEAR; ++month)
              {
                  uint realmonth = (initReservoirLvlMonth + month) % MONTHS_PER_YEAR;

                  uint simulationMonth = calendar_.mapping.months[realmonth];

                  uint firstDay = calendar_.months[simulationMonth].daysYear.first;

                  auto monthName = calendar_.text.months[simulationMonth].name;

                  fileContent << monthName[0] << monthName[1] << monthName[2] << '\t';
                  fileContent << '\t';
                  fileContent << data.inflows[realmonth] << '\t';
                  fileContent << MTG[realmonth] << '\t';
                  fileContent << areaMonthlyData[realmonth].MOG / area.hydro.reservoirCapacity
                              << '\t';
                  fileContent << OVF[realmonth] << '\t';
                  fileContent << areaMonthlyData[realmonth].MOL << '\t';
                  fileContent << minLvl[firstDay] << '\t';
                  fileContent << maxLvl[firstDay] << '\t';
                  fileContent << '\n';
              }
              auto content = fileContent.str();
              resultWriter_.addEntryFromBuffer(path, content);
          }
          indexArea++;
      });
}

} // namespace Antares
