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

#include <cassert>
#include <cmath>

#include <antares/exception/AssertionError.hpp>
#include <antares/logs/logs.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/solver/simulation/shave-peaks-by-remix-hydro.h"
#include "antares/study/simulation.h"

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

std::vector<double> computeTotalGenWithoutHydro(const std::vector<double>& load,
                                                const std::vector<double>& unsupE,
                                                const std::vector<double>& hydroGen)
{
    // Can be computed (for any hour) as : load - unsupplied energy - hydro
    std::vector<double> to_return = load;
    for (size_t i = 0; i < to_return.size(); ++i)
    {
        to_return[i] -= unsupE[i] + hydroGen[i];
    }
    return to_return;
}

std::vector<double> extractLoadForCurrentWeek(const Data::Area& area,
                                              const unsigned int year,
                                              const unsigned int firstHourOfWeek)
{
    std::vector<double> load_to_return(HOURS_IN_WEEK, 0.);
    for (int h = 0; h < HOURS_IN_WEEK; h++)
    {
        load_to_return[h] = area.load.series.getColumn(year)[h + firstHourOfWeek];
    }
    return load_to_return;
}

std::vector<double> extractHydroPmin(const Data::Area& area,
                                     const unsigned int year,
                                     const unsigned int firstHourOfWeek)
{
    // area->hydro.series->mingen.timeSeries
    std::vector<double> hydroPmin(HOURS_IN_WEEK, 0.);
    for (int h = 0; h < HOURS_IN_WEEK; h++)
    {
        hydroPmin[h] = area.hydro.series->mingen.getColumn(year)[h + firstHourOfWeek];
    }
    return hydroPmin;
}

static void RunAccurateShavePeaks(const Data::AreaList& areas,
                                  PROBLEME_HEBDO& problem,
                                  uint numSpace,
                                  uint firstHourOfWeek)
{
    areas.each(
      [&](const Data::Area& area)
      {
          auto& weeklyResults = problem.ResultatsHoraires[area.index];

          const auto load = extractLoadForCurrentWeek(area, problem.year, firstHourOfWeek);
          auto& unsupE = weeklyResults.ValeursHorairesDeDefaillancePositive;
          auto& hydroGen = weeklyResults.TurbinageHoraire;
          auto& levels = weeklyResults.niveauxHoraires;
          const auto DispatchGen = computeTotalGenWithoutHydro(load, unsupE, hydroGen);
          const auto& hydroPmax = problem.CaracteristiquesHydrauliques[area.index]
                                    .ContrainteDePmaxHydrauliqueHoraire;
          const auto hydroPmin = extractHydroPmin(area, problem.year, firstHourOfWeek);
          const double initLevel = problem.CaracteristiquesHydrauliques[area.index]
                                     .NiveauInitialReservoir;
          const double capacity = area.hydro.reservoirCapacity;
          const auto& inflows = problem.CaracteristiquesHydrauliques[area.index]
                                  .ApportNaturelHoraire;
          const auto& ovf = weeklyResults.debordementsHoraires;
          const auto& pump = weeklyResults.PompageHoraire;
          const auto& spillage = weeklyResults.ValeursHorairesDeDefaillanceNegative;

          const auto& dtgMrgArray = area.scratchpad[numSpace].dispatchableGenerationMargin;
          const std::vector<double> dtgMrg(dtgMrgArray, dtgMrgArray + HOURS_IN_WEEK);

          auto [H, U, L] = shavePeaksByRemixingHydro(DispatchGen,
                                                     hydroGen,
                                                     unsupE,
                                                     hydroPmax,
                                                     hydroPmin,
                                                     initLevel,
                                                     capacity,
                                                     inflows,
                                                     ovf,
                                                     pump,
                                                     spillage,
                                                     dtgMrg);
          hydroGen = H;
          unsupE = U;
          levels = L;
      });
}

void RemixHydroForAllAreas(const Data::AreaList& areas,
                           PROBLEME_HEBDO& problem,
                           Data::SheddingPolicy sheddingPolicy,
                           Data::SimplexOptimization simplexOptimizationRange,
                           uint numSpace,
                           uint hourInYear)
{
    if (sheddingPolicy == Data::shpShavePeaks)
    {
        bool result = true;

        switch (simplexOptimizationRange)
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
            throw new Data::AssertionError(
              "Error in simplex optimisation. Check logs for more details.");
        }
    }
    else if (sheddingPolicy == Data::shpAccurateShavePeaks)
    {
        try
        {
            RunAccurateShavePeaks(areas, problem, numSpace, hourInYear);
        }
        catch (std::invalid_argument& invalidArgExc)
        {
            Data::AssertionError assertErrException(invalidArgExc.what());
            throw assertErrException;
        }
    }
}
} // namespace Antares::Solver::Simulation
