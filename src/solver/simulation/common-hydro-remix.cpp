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
#include <antares/study/area/scratchpad.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include "antares/solver/simulation/common-eco-adq.h"
#include "antares/study/simulation.h"

#define EPSILON 1e-6

namespace Antares::Solver::Simulation
{
template<uint step>
static bool Remix(const Data::AreaList& areas,
                  PROBLEME_HEBDO& problem,
                  uint numSpace,
                  uint hourInYear)
{
    double HE[168];

    double DE[168];

    bool remix[168];

    double G[168];

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
          for (; offset < 168; offset += step, endHour += step)
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
            result = Remix<168>(areas, problem, numSpace, hourInYear);
            break;
        case Data::sorDay:
            result = Remix<24>(areas, problem, numSpace, hourInYear);
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
}
} // namespace Antares::Solver::Simulation
