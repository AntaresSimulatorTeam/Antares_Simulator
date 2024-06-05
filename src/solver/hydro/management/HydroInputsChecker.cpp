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
#include "antares/solver/hydro/management/HydroInputsChecker.h"

#include <yuni/core/logs.h>

#include <antares/utils/utils.h>
#include "antares/antares/fatal-error.h"
#include "antares/solver/hydro/monthly/h2o_m_donnees_annuelles.h"
#include "antares/solver/hydro/monthly/h2o_m_fonctions.h"
#include "antares/solver/simulation/common-eco-adq.h"

namespace Antares
{

HydroInputsChecker::HydroInputsChecker(Data::AreaList& areas,
                                       const Data::Parameters& params,
                                       const Date::Calendar& calendar,
                                       Data::SimulationMode simulationMode,
                                       uint firstYear,
                                       uint endYear/*,
                                       Solver::IResultWriter& resultWriter*/):
    areas_(areas),
    parameters_(params),
    calendar_(calendar),
    simulationMode_(simulationMode),
    firstYear_(firstYear),
    endYear_(endYear),
    prepareInflows_(areas, calendar),
    minGenerationScaling_(areas, calendar)/*,
    resultWriter_(resultWriter)*/
{
}

void HydroInputsChecker::Execute()
{
    Antares::Data::Area::ScratchMap scratchmap;
    unsigned int numSpace = 999999;
    uint nbPerformedYears = 0;
    for (auto year = firstYear_; year < endYear_; ++year)
    {
        // performCalculations
       // if (parameters_.yearsFilter[year])
      //  {
            // // not good missing info about buildnewSet which re-init nbPerformedYears = 0
            // nbPerformedYears++;
            // // Index of the MC year's space (useful if this year is actually run)
            // numSpace = nbPerformedYears - 1;
            // scratchmap = areas_.buildScratchMap(numSpace);

            // PrepareDataFromClustersInMustrunMode(scratchmap, year);

            prepareInflows_.Run(year);
            minGenerationScaling_.Run(year);
            if (!checksOnGenerationPowerBounds(year))
            {
                throw FatalError("hydro management: invalid minimum generation");
            }

            //---------------------//
            //            prepareNetDemand(year, parameters_.mode, scratchmap);
            //            prepareEffectiveDemand();
            //            prepareMonthlyOptimalGenerations(randomReservoirLevel, year);
            //   }
    }
}

bool HydroInputsChecker::checksOnGenerationPowerBounds(uint year) const
{
    return (checkMinGeneration(year) && checkGenerationPowerConsistency(year)) ? true : false;
}

bool HydroInputsChecker::checkMinGeneration(uint year) const
{
    bool ret = true;
    areas_.each(
      [this, &ret, &year](const Data::Area& area)
      {
          bool useHeuristicTarget = area.hydro.useHeuristicTarget;
          bool followLoadModulations = area.hydro.followLoadModulations;
          bool reservoirManagement = area.hydro.reservoirManagement;

          if (!useHeuristicTarget)
          {
              return;
          }

          if (!followLoadModulations)
          {
              ret = checkWeeklyMinGeneration(year, area) && ret;
              return;
          }

          if (reservoirManagement)
          {
              ret = checkYearlyMinGeneration(year, area) && ret;
          }
          else
          {
              ret = checkMonthlyMinGeneration(year, area) && ret;
          }
      });
    return ret;
}

bool HydroInputsChecker::checkWeeklyMinGeneration(uint year, const Data::Area& area) const
{
    const auto& srcinflows = area.hydro.series->storage.getColumn(year);
    const auto& srcmingen = area.hydro.series->mingen.getColumn(year);
    // Weekly minimum generation <= Weekly inflows for each week
    for (uint week = 0; week < calendar_.maxWeeksInYear - 1; ++week)
    {
        double totalWeekMingen = 0.0;
        double totalWeekInflows = 0.0;
        for (uint hour = calendar_.weeks[week].hours.first;
             hour < calendar_.weeks[week].hours.end && hour < HOURS_PER_YEAR;
             ++hour)
        {
            totalWeekMingen += srcmingen[hour];
        }

        for (uint day = calendar_.weeks[week].daysYear.first;
             day < calendar_.weeks[week].daysYear.end;
             ++day)
        {
            totalWeekInflows += srcinflows[day];
        }
        if (totalWeekMingen > totalWeekInflows)
        {
            logs.error() << "In Area " << area.name << " the minimum generation of "
                         << totalWeekMingen << " MW in week " << week + 1 << " of TS-"
                         << area.hydro.series->mingen.getSeriesIndex(year) + 1
                         << " is incompatible with the inflows of " << totalWeekInflows << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroInputsChecker::checkYearlyMinGeneration(uint year, const Data::Area& area) const
{
    //    const auto& data = tmpDataByArea_.at(&area);
    const auto& data = area.hydro.hydro_management_data.at(year);
    if (data.totalYearMingen > data.totalYearInflows)
    {
        // Yearly minimum generation <= Yearly inflows
        logs.error() << "In Area " << area.name << " the minimum generation of "
                     << data.totalYearMingen << " MW of TS-"
                     << area.hydro.series->mingen.getSeriesIndex(year) + 1
                     << " is incompatible with the inflows of " << data.totalYearInflows << " MW.";
        return false;
    }
    return true;
}

bool HydroInputsChecker::checkMonthlyMinGeneration(uint year, const Data::Area& area) const
{
    //    const auto& data = tmpDataByArea_.at(&area);
    const auto& data = area.hydro.hydro_management_data.at(year);
    for (uint month = 0; month != 12; ++month)
    {
        uint realmonth = calendar_.months[month].realmonth;
        // Monthly minimum generation <= Monthly inflows for each month
        if (data.totalMonthMingen[realmonth] > data.totalMonthInflows[realmonth])
        {
            logs.error() << "In Area " << area.name << " the minimum generation of "
                         << data.totalMonthMingen[realmonth] << " MW in month " << month + 1
                         << " of TS-" << area.hydro.series->mingen.getSeriesIndex(year) + 1
                         << " is incompatible with the inflows of "
                         << data.totalMonthInflows[realmonth] << " MW.";
            return false;
        }
    }
    return true;
}

bool HydroInputsChecker::checkGenerationPowerConsistency(uint year) const
{
    bool ret = true;

    areas_.each(
      [&ret, &year](const Data::Area& area)
      {
          const auto& srcmingen = area.hydro.series->mingen.getColumn(year);
          const auto& srcmaxgen = area.hydro.series->maxHourlyGenPower.getColumn(year);

          const uint tsIndexMin = area.hydro.series->mingen.getSeriesIndex(year);
          const uint tsIndexMax = area.hydro.series->maxHourlyGenPower.getSeriesIndex(year);

          for (uint h = 0; h < HOURS_PER_YEAR; ++h)
          {
              const auto& min = srcmingen[h];
              const auto& max = srcmaxgen[h];

              if (max < min)
              {
                  logs.error() << "In area: " << area.name << " [hourly] minimum generation of "
                               << min << " MW in timestep " << h + 1 << " of TS-" << tsIndexMin + 1
                               << " is incompatible with the maximum generation of " << max
                               << " MW in timestep " << h + 1 << " of TS-" << tsIndexMax + 1
                               << " MW.";
                  ret = false;
                  return;
              }
          }
      });

    return ret;
}

// void HydroInputsChecker::prepareNetDemand(uint year,
//                                           Data::SimulationMode mode,
//                                           const Antares::Data::Area::ScratchMap& scratchmap)
// {
//     areas_.each(
//       [this, &year, &scratchmap, &mode](Data::Area& area)
//       {
//           const auto& scratchpad = scratchmap.at(&area);

//           const auto& rormatrix = area.hydro.series->ror;
//           const auto* ror = rormatrix.getColumn(year);

//           auto& data = area.hydro.data;
//           const double* loadSeries = area.load.series.getColumn(year);
//           const double* windSeries = area.wind.series.getColumn(year);
//           const double* solarSeries = area.solar.series.getColumn(year);

//           for (uint hour = 0; hour != HOURS_PER_YEAR; ++hour)
//           {
//               auto dayYear = calendar_.hours[hour].dayYear;

//               double netdemand = 0;

//               // Aggregated renewable production: wind & solar
//               if (parameters_.renewableGeneration.isAggregated())
//               {
//                   netdemand = +loadSeries[hour] - windSeries[hour] - scratchpad.miscGenSum[hour]
//                               - solarSeries[hour] - ror[hour]
//                               - ((mode != Data::SimulationMode::Adequacy)
//                                    ? scratchpad.mustrunSum[hour]
//                                    : scratchpad.originalMustrunSum[hour]);
//               }

//               // Renewable clusters, if enabled
//               else if (parameters_.renewableGeneration.isClusters())
//               {
//                   netdemand = loadSeries[hour] - scratchpad.miscGenSum[hour] - ror[hour]
//                               - ((mode != Data::SimulationMode::Adequacy)
//                                    ? scratchpad.mustrunSum[hour]
//                                    : scratchpad.originalMustrunSum[hour]);

//                   for (auto& c: area.renewable.list.each_enabled())
//                   {
//                       netdemand -= c->valueAtTimeStep(year, hour);
//                   }
//               }

//               assert(!std::isnan(netdemand)
//                      && "hydro management: NaN detected when calculating the net demande");
//               data.DLN[dayYear] += netdemand;
//           }
//       });
// }

// void HydroInputsChecker::PrepareDataFromClustersInMustrunMode(Data::Area::ScratchMap& scratchmap,
//                                                               uint year)
// {
//     bool inAdequacy = (simulationMode_ == Data::SimulationMode::Adequacy);

//     for (uint i = 0; i < areas_.size(); ++i)
//     {
//         auto& area = *(areas_[i]);
//         auto& scratchpad = scratchmap.at(&area);

//         memset(scratchpad.mustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);
//         if (inAdequacy)
//         {
//             memset(scratchpad.originalMustrunSum, 0, sizeof(double) * HOURS_PER_YEAR);
//         }

//         double* mrs = scratchpad.mustrunSum;
//         double* adq = scratchpad.originalMustrunSum;

//         for (const auto& cluster: area.thermal.list.each_mustrun_and_enabled())
//         {
//             const auto& availableProduction = cluster->series.getColumn(year);
//             if (inAdequacy && cluster->mustrunOrigin)
//             {
//                 for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
//                 {
//                     mrs[h] += availableProduction[h];
//                     adq[h] += availableProduction[h];
//                 }
//             }
//             else
//             {
//                 for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
//                 {
//                     mrs[h] += availableProduction[h];
//                 }
//             }
//         }

//         if (inAdequacy)
//         {
//             for (const auto& cluster: area.thermal.list.each_mustrun_and_enabled())
//             {
//                 if (!cluster->mustrunOrigin)
//                 {
//                     continue;
//                 }

//                 const auto& availableProduction = cluster->series.getColumn(year);
//                 for (uint h = 0; h != cluster->series.timeSeries.height; ++h)
//                 {
//                     adq[h] += availableProduction[h];
//                 }
//             }
//         }
//     }
// }

// void HydroInputsChecker::prepareEffectiveDemand()
// {
//     areas_.each(
//       [&](Data::Area& area)
//       {
//           auto& data = area.hydro.data;

//           for (uint day = 0; day != 365; ++day)
//           {
//               auto month = calendar_.days[day].month;
//               assert(month < 12 && "Invalid month index");
//               auto realmonth = calendar_.months[month].realmonth;

//               double effectiveDemand = 0;
//               // area.hydro.allocation is indexed by area index
//               area.hydro.allocation.eachNonNull(
//                 [&](unsigned areaIndex, double value)
//                 {
//                     const auto* area = areas_.byIndex[areaIndex];
//                     // effectiveDemand += tmpDataByArea_[area].DLN[day] * value;
//                     effectiveDemand += area->hydro.data.DLN[day] * value;
//                 });

//               assert(!std::isnan(effectiveDemand) && "nan value detected for effectiveDemand");
//               data.DLE[day] += effectiveDemand;
//               data.MLE[realmonth] += effectiveDemand;

//               assert(not std::isnan(data.DLE[day]) && "nan value detected for DLE");
//               assert(not std::isnan(data.MLE[realmonth]) && "nan value detected for DLE");
//           }

//           auto minimumYear = std::numeric_limits<double>::infinity();
//           auto dayYear = 0u;

//           for (uint month = 0; month != 12; ++month)
//           {
//               auto minimumMonth = +std::numeric_limits<double>::infinity();
//               auto daysPerMonth = calendar_.months[month].days;
//               auto realmonth = calendar_.months[month].realmonth;

//               for (uint d = 0; d != daysPerMonth; ++d)
//               {
//                   auto dYear = d + dayYear;
//                   if (data.DLE[dYear] < minimumMonth)
//                   {
//                       minimumMonth = data.DLE[dYear];
//                   }
//               }

//               if (minimumMonth < 0.)
//               {
//                   for (uint d = 0; d != daysPerMonth; ++d)
//                   {
//                       data.DLE[dayYear + d] -= minimumMonth - 1e-4;
//                   }
//               }

//               if (data.MLE[realmonth] < minimumYear)
//               {
//                   minimumYear = data.MLE[realmonth];
//               }

//               dayYear += daysPerMonth;
//           }

//           if (minimumYear < 0.)
//           {
//               for (uint realmonth = 0; realmonth != 12; ++realmonth)
//               {
//                   data.MLE[realmonth] -= minimumYear - 1e-4;
//               }
//           }
//       });
// }

// template<class ProblemT>
// static void CheckHydroAllocationProblem(Data::Area& area,
//                                         ProblemT& problem,
//                                         int initLevelMonth,
//                                         double lvi)
//{
//     bool error = false;
//
//     double X = problem.Volume[11] - problem.Turbine[11] + problem.Apport[11];
//     if (!Utils::isZero(X - lvi))
//     {
//         logs.fatal() << area.id << ": hydro management: monthly: reservoir error";
//         error = true;
//     }
//
//     if (error)
//     {
//         logs.warning() << area.id << ": lvi = " << lvi;
//         logs.warning() << area.id << ": cost = " << problem.CoutDepassementVolume;
//         for (uint month = 0; month != 12; ++month)
//         {
//             uint realmonth = (initLevelMonth + month) % 12;
//             logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
//                            << ", turb.max: " << problem.TurbineMax[realmonth]
//                            << ", turb.cible: " << problem.TurbineCible[realmonth]
//                            << ", apport: " << problem.Apport[realmonth] << ", volume: ["
//                            << problem.VolumeMin[realmonth] << " .. " <<
//                            problem.VolumeMax[realmonth]
//                            << "]";
//         }
//
//         logs.info();
//         problem.Volume[initLevelMonth] = lvi;
//         for (uint month = 0; month != 12; ++month)
//         {
//             uint realmonth = (initLevelMonth + month) % 12;
//             logs.warning() << "month: " << ((realmonth < 10) ? "0" : "") << realmonth
//                            << ", turbine: " << problem.Turbine[realmonth]
//                            << ", volume: " << problem.Volume[realmonth];
//         }
//     }
// }
//
// void HydroInputsChecker::prepareMonthlyOptimalGenerations(double* random_reservoir_level, uint y)
//{
//     uint indexArea = 0;
//     areas_.each(
//       [&](Data::Area& area)
//       {
//           auto& data = area.hydro.data;
//
//           auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
//           auto& maxLvl = area.hydro.reservoirLevel[Data::PartHydro::maximum];
//
//           int initReservoirLvlMonth = area.hydro.initializeReservoirLevelDate;
//
//           double lvi = -1.;
//           if (area.hydro.reservoirManagement)
//           {
//               lvi = random_reservoir_level[indexArea];
//           }
//
//           indexArea++;
//
//           double solutionCost = 0.;
//           double solutionCostNoised = 0.;
//
//           if (area.hydro.reservoirManagement)
//           {
//               auto problem = H2O_M_Instanciation(1);
//
//               double totalInflowsYear = prepareMonthlyTargetGenerations(area, data);
//               assert(totalInflowsYear >= 0.);
//
//               problem.CoutDepassementVolume = 1e2;
//               problem.CoutViolMaxDuVolumeMin = 1e5;
//               problem.VolumeInitial = lvi;
//
//               for (unsigned month = 0; month != 12; ++month)
//               {
//                   uint realmonth = (initReservoirLvlMonth + month) % 12;
//
//                   uint simulationMonth = calendar_.mapping.months[realmonth];
//                   uint firstDay = calendar_.months[simulationMonth].daysYear.first;
//
//                   problem.TurbineMax[month] = totalInflowsYear;
//                   problem.TurbineMin[month] = data.mingens[realmonth];
//                   problem.TurbineCible[month] = data.MTG[realmonth];
//                   problem.Apport[month] = data.inflows[realmonth];
//                   problem.VolumeMin[month] = minLvl[firstDay];
//                   problem.VolumeMax[month] = maxLvl[firstDay];
//               }
//
//               H2O_M_OptimiserUneAnnee(problem, 0);
//               switch (problem.ResultatsValides)
//               {
//               case OUI:
//               {
//                   if (Logs::Verbosity::Debug::enabled)
//                   {
//                       CheckHydroAllocationProblem(area, problem, initReservoirLvlMonth, lvi);
//                   }
//
//                   for (uint month = 0; month != 12; ++month)
//                   {
//                       uint realmonth = (initReservoirLvlMonth + month) % 12;
//
//                       data.MOG[realmonth] = problem.Turbine[month] *
//                       area.hydro.reservoirCapacity; data.MOL[realmonth] = problem.Volume[month];
//                   }
//                   data.MOL[initReservoirLvlMonth] = lvi;
//                   solutionCost = problem.ProblemeHydraulique.CoutDeLaSolution;
//                   solutionCostNoised = problem.ProblemeHydraulique.CoutDeLaSolutionBruite;
//
//                   break;
//               }
//               case NON:
//               {
//                   std::ostringstream msg;
//                   msg << "Year : " << y + 1 << " - hydro: " << area.name
//                       << " [month] no solution found";
//                   throw FatalError(msg.str());
//               }
//               case EMERGENCY_SHUT_DOWN:
//               {
//                   std::ostringstream msg;
//                   msg << "Year : " << y + 1 << " - hydro: " << area.name << " [month] fatal
//                   error"; throw FatalError(msg.str());
//               }
//               }
//
//               H2O_M_Free(problem);
//           }
//
//           else
//           {
//               auto& reservoirLevel = area.hydro.reservoirLevel[Data::PartHydro::average];
//
//               for (uint realmonth = 0; realmonth != 12; ++realmonth)
//               {
//                   data.MOG[realmonth] = data.inflows[realmonth];
//                   data.MOL[realmonth] = reservoirLevel[realmonth];
//               }
//           }
//
// #ifndef NDEBUG
//           for (uint realmonth = 0; realmonth != 12; ++realmonth)
//           {
//               assert(!std::isnan(data.MOG[realmonth]) && "nan value detected for MOG");
//               assert(!std::isnan(data.MOL[realmonth]) && "nan value detected for MOL");
//               assert(!std::isinf(data.MOG[realmonth]) && "infinite value detected for MOG");
//               assert(!std::isinf(data.MOL[realmonth]) && "infinite value detected for MOL");
//           }
// #endif
//           if (parameters_.hydroDebug)
//           {
//               std::ostringstream buffer, path;
//               path << "debug" << SEP << "solver" << SEP << (1 + y) << SEP << "monthly." <<
//               area.name
//                    << ".txt";
//
//               if (area.hydro.reservoirManagement)
//                   buffer << "Initial Reservoir Level\t" << lvi << "\n";
//               else
//                   buffer << "Initial Reservoir Level : unrelevant (no reservoir mgmt)\n";
//               buffer << "\n";
//
//               auto writeSolutionCost = [&buffer](const std::string& caption, double cost)
//               {
//                   auto precision = buffer.precision();
//                   buffer << caption << std::fixed << std::setprecision(13) << cost;
//                   buffer << std::setprecision(precision) << std::defaultfloat;
//               };
//               writeSolutionCost("Solution cost : ", solutionCost);
//               writeSolutionCost("Solution cost (noised) : ", solutionCostNoised);
//               buffer << "\n\n";
//
//               buffer << '\t' << "\tInflows" << '\t' << "\tTarget Gen."
//                      << "\tTurbined"
//                      << "\tLevels" << '\t' << "\tLvl min" << '\t' << "\tLvl max\n";
//               for (uint month = 0; month != 12; ++month)
//               {
//                   uint realmonth = (initReservoirLvlMonth + month) % 12;
//
//                   uint simulationMonth = calendar_.mapping.months[realmonth];
//
//                   uint firstDay = calendar_.months[simulationMonth].daysYear.first;
//
//                   auto monthName = calendar_.text.months[simulationMonth].name;
//
//                   buffer << monthName[0] << monthName[1] << monthName[2] << '\t';
//                   buffer << '\t';
//                   buffer << data.inflows[realmonth] << '\t';
//                   buffer << data.MTG[realmonth] << '\t';
//                   buffer << data.MOG[realmonth] / area.hydro.reservoirCapacity << '\t';
//                   buffer << data.MOL[realmonth] << '\t';
//                   buffer << minLvl[firstDay] << '\t';
//                   buffer << maxLvl[firstDay] << '\t';
//                   buffer << '\n';
//               }
//               auto content = buffer.str();
//               resultWriter_.addEntryFromBuffer(path.str(), content);
//           }
//       });
// }
//
// double HydroInputsChecker::prepareMonthlyTargetGenerations(Data::Area& area,
//                                                            Antares::Data::TmpDataByArea& data)
//{
//     double total = 0;
//
//     for (uint realmonth = 0; realmonth != 12; ++realmonth)
//     {
//         total += data.inflows[realmonth];
//     }
//
//     if (not area.hydro.followLoadModulations)
//     {
//         for (uint realmonth = 0; realmonth != 12; ++realmonth)
//         {
//             data.MTG[realmonth] = data.inflows[realmonth];
//         }
//
//         return total;
//     }
//
//     double monthlyMaxDemand = -std::numeric_limits<double>::infinity();
//
//     for (uint realmonth = 0; realmonth != 12; ++realmonth)
//     {
//         if (data.MLE[realmonth] > monthlyMaxDemand)
//         {
//             monthlyMaxDemand = data.MLE[realmonth];
//         }
//     }
//
//     if (!Utils::isZero(monthlyMaxDemand))
//     {
//         double coeff = 0.;
//         for (uint realmonth = 0; realmonth != 12; ++realmonth)
//         {
//             assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
//             coeff += std::pow(data.MLE[realmonth] / monthlyMaxDemand,
//                               area.hydro.intermonthlyBreakdown);
//         }
//
//         if (!Utils::isZero(coeff))
//         {
//             coeff = total / coeff;
//         }
//
//         for (uint realmonth = 0; realmonth != 12; ++realmonth)
//         {
//             assert(data.MLE[realmonth] / monthlyMaxDemand >= 0.);
//             data.MTG[realmonth] = coeff
//                                   * std::pow(data.MLE[realmonth] / monthlyMaxDemand,
//                                              area.hydro.intermonthlyBreakdown);
//         }
//     }
//     else
//     {
//         double coeff = total / 12.;
//
//         for (uint realmonth = 0; realmonth != 12; ++realmonth)
//         {
//             data.MTG[realmonth] = coeff;
//         }
//     }
//
//     return total;
// }

void HydroChecks::Run()
{
    for (auto check: checks)
    {
        check->Run();
    }
}
} // namespace Antares
