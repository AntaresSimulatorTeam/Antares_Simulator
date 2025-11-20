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

#include <cassert>
#include <cmath>
#include <sstream>
#include <string>

#include <antares/exception/AssertionError.hpp>
#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/remix-storage/create-storage-for-remix.h"
#include "antares/solver/simulation/remix-storage/remix-utils.h"
#include "antares/solver/simulation/remix-storage/shave-peaks-by-remix-storage-gen.h"

#define EPSILON 1e-6

namespace Antares::Solver::Simulation
{

const unsigned int HOURS_IN_WEEK = 168;
const unsigned int HOURS_IN_DAY = 24;

template<uint step>
static bool Remix(const Data::AreaList& areas,
                  PROBLEME_HEBDO& problem,
                  uint numSpace,
                  uint hourInYear)
{
    double HE[HOURS_IN_WEEK];

    double DE[HOURS_IN_WEEK];

    bool remix[HOURS_IN_WEEK];

    double G[HOURS_IN_WEEK];

    bool status = true;

    areas.each(
      [&HE, &DE, &remix, &G, &status, &problem, &numSpace, &hourInYear](const Data::Area& area)
      {
          auto index = area.index;

          auto& weeklyResults = problem.ResultatsHoraires[index];

          auto& D = weeklyResults.ValeursHorairesDeDefaillancePositive;

          const auto& S = weeklyResults.ValeursHorairesDeDefaillanceNegative;

          auto& H = weeklyResults.TurbinageHoraire;

          memset(remix, 0, sizeof(remix));
          memset(G, 0, sizeof(G));

          uint endHour = step;
          uint offset = 0;
          for (; offset < HOURS_IN_WEEK; offset += step, endHour += step)
          {
              {
                  double WD = 0.;
                  for (uint i = offset; i != endHour; ++i)
                  {
                      WD += D[i];
                  }
                  if (WD < EPSILON)
                  {
                      continue;
                  }
              }

              double WH = 0.;

              for (uint i = offset; i != endHour; ++i)
              {
                  if (S[i] < EPSILON)
                  {
                      WH += H[i];
                  }
              }

              if (WH < EPSILON)
              {
                  continue;
              }

              WH = 0.;

              double bottom = std::numeric_limits<double>::max();
              double top = 0;

              auto* L = area.load.series.getColumn(problem.year);

              const double* M = area.scratchpad[numSpace].dispatchableGenerationMargin;

              for (uint i = offset; i < endHour; ++i)
              {
                  double h_d = H[i] + D[i];
                  if (h_d > 0. && Utils::isZero(S[i] + M[i]))
                  {
                      double Li = L[i + hourInYear];

                      remix[i] = true;
                      G[i] = Li - h_d;

                      if (G[i] < bottom)
                      {
                          bottom = G[i];
                      }
                      if (Li > top)
                      {
                          top = Li;
                      }

                      WH += H[i];
                  }
              }

              const auto& P = problem.CaracteristiquesHydrauliques[index]
                                .ContrainteDePmaxHydrauliqueHoraire;

              double ecart = 1.;
              uint loop = 100;
              do
              {
                  double niveau = (top + bottom) * 0.5;
                  double stock = 0.;

                  for (uint i = offset; i != endHour; ++i)
                  {
                      if (remix[i])
                      {
                          double HEi;
                          uint iYear = i + hourInYear;
                          if (niveau > L[iYear])
                          {
                              HEi = H[i] + D[i];
                              if (HEi > P[i])
                              {
                                  HEi = P[i];
                                  DE[i] = H[i] + D[i] - HEi;
                              }
                              else
                              {
                                  DE[i] = 0;
                              }
                          }
                          else
                          {
                              if (G[i] > niveau)
                              {
                                  HEi = 0;
                                  DE[i] = H[i] + D[i];
                              }
                              else
                              {
                                  HEi = niveau - G[i];
                                  if (HEi > P[i])
                                  {
                                      HEi = P[i];
                                  }
                                  DE[i] = H[i] + D[i] - HEi;
                              }
                          }
                          stock += HEi;
                          HE[i] = HEi;
                      }
                      else
                      {
                          HE[i] = H[i];
                          DE[i] = D[i];
                      }
                  }

                  ecart = WH - stock;
                  if (ecart > 0.)
                  {
                      bottom = niveau;
                  }
                  else
                  {
                      top = niveau;
                  }

                  if (!--loop)
                  {
                      status = false;
                      logs.error() << "hydro remix: " << area.name
                                   << ": infinite loop detected. please check input data";
                      break;
                  }
              } while (std::abs(ecart) > 0.01);

              for (uint i = offset; i != endHour; ++i)
              {
                  H[i] = HE[i];
                  assert(not std::isnan(HE[i]) && "hydro remix: nan detected");
              }
              for (uint i = offset; i != endHour; ++i)
              {
                  D[i] = DE[i];
                  assert(not std::isnan(DE[i]) && "hydro remix: nan detected");
              }
          }
      });

    return status;
}

std::vector<double> extractLoadForCurrentWeek(const Data::Area& area,
                                              const unsigned year,
                                              const unsigned firstHourOfWeek)
{
    std::vector<double> load_to_return(HOURS_IN_WEEK, 0.);
    for (unsigned h = 0; h < HOURS_IN_WEEK; ++h)
    {
        load_to_return[h] = area.load.series.getColumn(year)[h + firstHourOfWeek];
    }
    return load_to_return;
}

std::vector<double> extractHydroPmin(const Data::Area& area,
                                     const unsigned year,
                                     const unsigned firstHourOfWeek)
{
    const double* hydroPmin = area.hydro.series->mingen.getColumn(year);
    return {hydroPmin + firstHourOfWeek, hydroPmin + firstHourOfWeek + HOURS_IN_WEEK};
}

std::vector<double> extractDTG_MRG(const Data::Area& area, uint numSpace)
{
    const double* dtgMrg = area.scratchpad[numSpace].dispatchableGenerationMargin;
    return {dtgMrg, dtgMrg + HOURS_IN_WEEK};
}

std::shared_ptr<IStorageForRemix> extractHydroForRemix(const Data::Area& area,
                                                       PROBLEME_HEBDO& problem,
                                                       unsigned firstHourOfWeek)
{
    auto& weeklyResults = problem.ResultatsHoraires[area.index];

    auto& unsupE = weeklyResults.ValeursHorairesDeDefaillancePositive;
    auto& hydroGen = weeklyResults.TurbinageHoraire;
    auto& levels = weeklyResults.niveauxHoraires;
    const auto& hydroPmax = problem.CaracteristiquesHydrauliques[area.index]
                              .ContrainteDePmaxHydrauliqueHoraire;
    const auto hydroPmin = extractHydroPmin(area, problem.year, firstHourOfWeek);
    const double initLevel = problem.CaracteristiquesHydrauliques[area.index]
                               .NiveauInitialReservoir;
    const double capacity = area.hydro.reservoirCapacity;
    const double efficiency = area.hydro.pumpingEfficiency;
    const bool reservoirManagement = area.hydro.reservoirManagement;
    const auto& inflows = problem.CaracteristiquesHydrauliques[area.index].ApportNaturelHoraire;
    const auto& ovf = weeklyResults.debordementsHoraires;
    const auto& pump = weeklyResults.PompageHoraire;

    return makeHydroForRemix(hydroGen,
                             unsupE,
                             levels,
                             hydroPmax,
                             hydroPmin,
                             inflows,
                             ovf,
                             pump,
                             initLevel,
                             capacity,
                             efficiency,
                             reservoirManagement,
                             std::string(area.name) + " hydro");
}

std::span<const double> weekSubRange(const std::vector<double>& v, unsigned firstHourOfWeek)
{
    return {v.begin() + firstHourOfWeek, v.begin() + firstHourOfWeek + HOURS_IN_WEEK};
}

std::vector<double> extractSTSpmax(const PROPERTIES& sts_properties, const unsigned firstHourOfWeek)
{
    auto subrange = weekSubRange(sts_properties.series->maxWithdrawalModulation, firstHourOfWeek);
    return subrange * sts_properties.withdrawalEfficiency;
}

std::vector<double> extractSTSlowRuleCurve(const PROPERTIES& sts_properties,
                                           const unsigned firstHourOfWeek)
{
    auto subrange = weekSubRange(sts_properties.series->lowerRuleCurve, firstHourOfWeek);
    return subrange * sts_properties.reservoirCapacity;
}

std::vector<double> extractSTSupRuleCurve(const PROPERTIES& sts_properties,
                                          const unsigned firstHourOfWeek)
{
    auto subrange = weekSubRange(sts_properties.series->upperRuleCurve, firstHourOfWeek);
    return subrange * sts_properties.reservoirCapacity;
}

std::vector<double> extractSTSinflows(const PROPERTIES& sts_properties,
                                      const unsigned firstHourOfWeek,
                                      const unsigned year)
{
    std::vector<double> to_return(HOURS_IN_WEEK, 0.);
    for (unsigned h = 0; h < HOURS_IN_WEEK; ++h)
    {
        to_return[h] = sts_properties.series->inflows.getCoefficient(year, firstHourOfWeek + h);
    }
    return to_return;
}

std::shared_ptr<IStorageForRemix> extractSTSforRemix(const Data::Area& area,
                                                     PROBLEME_HEBDO& problem,
                                                     unsigned stsIndex,
                                                     unsigned firstHourOfWeek)
{
    const auto& stsProperties = problem.ShortTermStorage[area.index][stsIndex];
    auto& weeklyResults = problem.ResultatsHoraires[area.index];
    auto& stsResults = weeklyResults.ShortTermStorage;

    auto& withdrawal = stsResults[stsIndex].withdrawal;
    const auto& injection = stsResults[stsIndex].injection;
    auto& unsupE = weeklyResults.ValeursHorairesDeDefaillancePositive;
    auto& levels = stsResults[stsIndex].level;

    const auto& pmax = extractSTSpmax(stsProperties, firstHourOfWeek);
    const auto& inflows = extractSTSinflows(stsProperties, firstHourOfWeek, problem.year);
    const auto lowRuleCurve = extractSTSlowRuleCurve(stsProperties, firstHourOfWeek);
    const auto upRuleCurve = extractSTSupRuleCurve(stsProperties, firstHourOfWeek);
    const double initLevel = levels[0];
    const double withdrawalcapacity = stsProperties.withdrawalNominalCapacity;
    const double withdrawalEff = stsProperties.withdrawalEfficiency;
    const double injectionEff = stsProperties.injectionEfficiency;

    return makeSTSforRemix(withdrawal,
                           unsupE,
                           levels,
                           pmax,
                           inflows,
                           injection,
                           lowRuleCurve,
                           upRuleCurve,
                           initLevel,
                           withdrawalEff,
                           injectionEff,
                           std::string(area.name) + " " + stsProperties.name);
}

ListStorageForRemix extractListSTSforRemix(const Data::Area& area,
                                           PROBLEME_HEBDO& problem,
                                           const unsigned firstHourOfWeek)
{
    StorageListSort stsListSort;

    for (unsigned stsIndex{0}; stsIndex < area.shortTermStorage.count(); ++stsIndex)
    {
        const auto& stsProperties = problem.ShortTermStorage[area.index][stsIndex];
        const double withdrawalCapacity = stsProperties.withdrawalNominalCapacity;

        stsListSort.add(withdrawalCapacity,
                        extractSTSforRemix(area, problem, stsIndex, firstHourOfWeek));
    }

    return stsListSort.makeSortedList();
}

static void RunAccurateShavePeaks(const Data::AreaList& areas,
                                  PROBLEME_HEBDO& problem,
                                  uint numSpace,
                                  uint firstHourOfWeek,
                                  bool includeSTS,
                                  IResultWriter* writer)
{
    std::stringstream debugStream;
    areas.each(
      [&](const Data::Area& area)
      {
          auto& weeklyResults = problem.ResultatsHoraires[area.index];

          // Arguments of remix algorithm that are invariant whatever the storage
          const auto load = extractLoadForCurrentWeek(area, problem.year, firstHourOfWeek);
          auto& unsupE = weeklyResults.ValeursHorairesDeDefaillancePositive;
          const auto& spillage = weeklyResults.ValeursHorairesDeDefaillanceNegative;
          const auto dtgMrg = extractDTG_MRG(area, numSpace);

          ListStorageForRemix listStorage;

          auto hydroStorage = extractHydroForRemix(area, problem, firstHourOfWeek);
          listStorage.push_back(hydroStorage);

          if (includeSTS)
          {
              auto stsForRemix = extractListSTSforRemix(area, problem, firstHourOfWeek);
              listStorage.insert(listStorage.end(), stsForRemix.begin(), stsForRemix.end());
          }

          try
          {
              checkInput(load, unsupE, spillage, dtgMrg, listStorage);
              shavePeaksByRemixingStorageGen(load, unsupE, spillage, dtgMrg, listStorage);
              if (writer)
              {
                  collectRemixDebugInfo(listStorage, debugStream);
              }
          }
          catch (std::exception& e)
          {
              std::string msg = "(year, area, week) = (" + std::to_string(problem.year) + ", "
                                + area.id.to<std::string>() + ", "
                                + std::to_string((firstHourOfWeek + 1) / HOURS_IN_WEEK)
                                + ") : " + e.what();
              logs.warning(msg);
          }
      });

    if (writer)
    {
        std::string filename("remix-" + std::to_string(problem.year) + "-"
                             + std::to_string(problem.weekInTheYear) + ".csv");
        std::string s = debugStream.str();
        writer->addEntryFromBuffer(filename, s);
    }
}

void RemixHydroForAllAreas(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           const Data::Parameters& params,
                           uint numSpace,
                           uint hourInYear,
                           IResultWriter& resultWriter)
{
    if (params.shedding.policy == Data::shpShavePeaks)
    {
        bool result = true;

        switch (params.simplexOptimizationRange)
        {
        case Data::sorWeek:
            result = Remix<HOURS_IN_WEEK>(areas, problem, numSpace, hourInYear);
            break;
        case Data::sorDay:
            result = Remix<HOURS_IN_DAY>(areas, problem, numSpace, hourInYear);
            break;
        case Data::sorUnknown:
            logs.fatal() << "invalid simplex optimization range";
            break;
        }

        if (!result)
        {
            throw Data::AssertionError(
              "Error in simplex optimisation. Check logs for more details.");
        }
    }
    else if (params.shedding.policy == Data::shpAccurateShavePeaks)
    {
        bool includeSTS = params.accurateShavePeaksIncludeShortTermStorage;
        bool debugInfos = params.remixStorageDebug;
        try
        {
            RunAccurateShavePeaks(areas,
                                  problem,
                                  numSpace,
                                  hourInYear,
                                  includeSTS,
                                  debugInfos ? &resultWriter : nullptr);
        }
        catch (std::invalid_argument& invalidArgExc)
        {
            Data::AssertionError assertErrException(invalidArgExc.what());
            throw assertErrException;
        }
    }
}
} // namespace Antares::Solver::Simulation
