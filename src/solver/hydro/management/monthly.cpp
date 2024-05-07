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
#include <iomanip>
#include <limits>
#include <sstream>

#include <yuni/yuni.h>
#include <yuni/io/directory.h>
#include <yuni/io/file.h>

#include <antares/antares/fatal-error.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include "antares/solver/hydro/management/management.h"
#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"

using namespace Yuni;

#define SEP IO::Separator

namespace Antares
{
template<class ProblemT>
static void CheckHydroAllocationProblem(Data::Area& area,
                                        ProblemT& problem,
                                        int initLevelMonth,
                                        double lvi)
{
    bool error = false;

    double X = problem.Volume[11] - problem.Turbine[11] + problem.Apport[11];
    if (!Utils::isZero(X - lvi))
    {
        logs.fatal() << area.id << ": hydro management: monthly: reservoir error";
        error = true;
    }

    if (error)
    {
        logs.warning() << area.id << ": lvi = " << lvi;
        logs.warning() << area.id << ": cost = " << problem.CoutDepassementVolume;
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initLevelMonth + month) % 12;
            logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
                           << ", turb.max: " << problem.TurbineMax[realmonth]
                           << ", turb.cible: " << problem.TurbineCible[realmonth]
                           << ", apport: " << problem.Apport[realmonth] << ", volume: ["
                           << problem.VolumeMin[realmonth] << " .. " << problem.VolumeMax[realmonth]
                           << "]";
        }

        logs.info();
        problem.Volume[initLevelMonth] = lvi;
        for (uint month = 0; month != 12; ++month)
        {
            uint realmonth = (initLevelMonth + month) % 12;
            logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
                           << ", turbine: " << problem.Turbine[realmonth]
                           << ", volume: " << problem.Volume[realmonth];
        }
    }
}

double HydroManagement::prepareMonthlyTargetGenerations(Data::Area& area, TmpDataByArea& data)
{
    double total = 0;

    for (uint realmonth = 0; realmonth != 12; ++realmonth)
    {
        total += data.inflows[realmonth];
    }

    if (not area.hydro.followLoadModulations)
    {
        for (uint realmonth = 0; realmonth != 12; ++realmonth)
        {
            data.MTG[realmonth] = data.inflows[realmonth];
        }

        return total;
    }

    double monthlyMaxDemand = -std::numeric_limits<double>::infinity();

    for (uint realmonth = 0; realmonth != 12; ++realmonth)
    {
        if (data.MLE[realmonth] > monthlyMaxDemand)
        {
            monthlyMaxDemand = data.MLE[realmonth];
        }
    }

    if (!Utils::isZero(monthlyMaxDemand))
    {
        double coeff = 0.;
        for (uint realmonth = 0; realmonth != 12; ++realmonth)
        {
            assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
            coeff += std::pow(data.MLE[realmonth] / monthlyMaxDemand,
                              area.hydro.intermonthlyBreakdown);
        }

        if (!Utils::isZero(coeff))
        {
            coeff = total / coeff;
        }

        for (uint realmonth = 0; realmonth != 12; ++realmonth)
        {
            assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
            data.MTG[realmonth] = coeff
                                  * std::pow(data.MLE[realmonth] / monthlyMaxDemand,
                                             area.hydro.intermonthlyBreakdown);
        }
    }
    else
    {
        double coeff = total / 12.;

        for (uint realmonth = 0; realmonth != 12; ++realmonth)
        {
            data.MTG[realmonth] = coeff;
        }
    }

    return total;
}

void HydroManagement::prepareMonthlyOptimalGenerations(double* random_reservoir_level, uint y)
{
    uint indexArea = 0;
    areas_.each(
      [&](Data::Area& area)
      {
          auto& data = tmpDataByArea_[&area];

          auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
          auto& maxLvl = area.hydro.reservoirLevel[Data::PartHydro::maximum];

          int initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate;

          double lvi = -1.;
          if (area.hydro.reservoirManagement)
          {
              lvi = random_reservoir_level[indexArea];
          }

          indexArea++;

          double solutionCost = 0.;
          double solutionCostNoised = 0.;

          if (area.hydro.reservoirManagement)
          {
              auto problem = H2O_M_Instanciation(1);

              double totalInflowsYear = prepareMonthlyTargetGenerations(area, data);
              assert(totalInflowsYear >= 0.);

              problem.CoutDepassementVolume = 1e2;
              problem.CoutViolMaxDuVolumeMin = 1e5;
              problem.VolumeInitial = lvi;

              for (unsigned month = 0; month != 12; ++month)
              {
                  uint realmonth = (initReservoirLvlMonth + month) % 12;

                  uint simulationMonth = calendar_.mapping.months[realmonth];
                  uint firstDay = calendar_.months[simulationMonth].daysYear.first;

                  problem.TurbineMax[month] = totalInflowsYear;
                  problem.TurbineMin[month] = data.mingens[realmonth];
                  problem.TurbineCible[month] = data.MTG[realmonth];
                  problem.Apport[month] = data.inflows[realmonth];
                  problem.VolumeMin[month] = minLvl[firstDay];
                  problem.VolumeMax[month] = maxLvl[firstDay];
              }

              H2O_M_OptimiserUneAnnee(problem, 0);
              switch (problem.ResultatsValides)
              {
              case OUI:
              {
                  if (Logs::Verbosity::Debug::enabled)
                  {
                      CheckHydroAllocationProblem(area, problem, initReservoirLvlMonth, lvi);
                  }

                  for (uint month = 0; month != 12; ++month)
                  {
                      uint realmonth = (initReservoirLvlMonth + month) % 12;

                      data.MOG[realmonth] = problem.Turbine[month] * area.hydro.reservoirCapacity;
                      data.MOL[realmonth] = problem.Volume[month];
                  }
                  data.MOL[initReservoirLvlMonth] = lvi;
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

              H2O_M_Free(problem);
          }

          else
          {
              auto& reservoirLevel = area.hydro.reservoirLevel[Data::PartHydro::average];

              for (uint realmonth = 0; realmonth != 12; ++realmonth)
              {
                  data.MOG[realmonth] = data.inflows[realmonth];
                  data.MOL[realmonth] = reservoirLevel[realmonth];
              }
          }

#ifndef NDEBUG
          for (uint realmonth = 0; realmonth != 12; ++realmonth)
          {
              assert(!std::isnan(data.MOG[realmonth]) && "nan value detected for MOG");
              assert(!std::isnan(data.MOL[realmonth]) && "nan value detected for MOL");
              assert(!std::isinf(data.MOG[realmonth]) && "infinite value detected for MOG");
              assert(!std::isinf(data.MOL[realmonth]) && "infinite value detected for MOL");
          }
#endif
          if (parameters_.hydroDebug)
          {
              std::ostringstream buffer, path;
              path << "debug" << SEP << "solver" << SEP << (1 + y) << SEP << "monthly." << area.name
                   << ".txt";

              if (area.hydro.reservoirManagement)
                  buffer << "Initial Reservoir Level\t" << lvi << "\n";
              else
                  buffer << "Initial Reservoir Level : unrelevant (no reservoir mgmt)\n";
              buffer << "\n";

              auto writeSolutionCost = [&buffer](const std::string& caption, double cost)
              {
                  auto precision = buffer.precision();
                  buffer << caption << std::fixed << std::setprecision(13) << cost;
                  buffer << std::setprecision(precision) << std::defaultfloat;
              };
              writeSolutionCost("Solution cost : ", solutionCost);
              writeSolutionCost("Solution cost (noised) : ", solutionCostNoised);
              buffer << "\n\n";

              buffer << '\t' << "\tInflows" << '\t' << "\tTarget Gen." << "\tTurbined" << "\tLevels"
                     << '\t' << "\tLvl min" << '\t' << "\tLvl max\n";
              for (uint month = 0; month != 12; ++month)
              {
                  uint realmonth = (initReservoirLvlMonth + month) % 12;

                  uint simulationMonth = calendar_.mapping.months[realmonth];

                  uint firstDay = calendar_.months[simulationMonth].daysYear.first;

                  auto monthName = calendar_.text.months[simulationMonth].name;

                  buffer << monthName[0] << monthName[1] << monthName[2] << '\t';
                  buffer << '\t';
                  buffer << data.inflows[realmonth] << '\t';
                  buffer << data.MTG[realmonth] << '\t';
                  buffer << data.MOG[realmonth] / area.hydro.reservoirCapacity << '\t';
                  buffer << data.MOL[realmonth] << '\t';
                  buffer << minLvl[firstDay] << '\t';
                  buffer << maxLvl[firstDay] << '\t';
                  buffer << '\n';
              }
              auto content = buffer.str();
              resultWriter_.addEntryFromBuffer(path.str(), content);
          }
      });
}

} // namespace Antares
