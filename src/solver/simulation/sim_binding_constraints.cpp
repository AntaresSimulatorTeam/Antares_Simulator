// Copyright 2007-2025, RTE (https://www.rte-france.com)
// See AUTHORS.txt
// SPDX-License-Identifier: MPL-2.0
// This file is part of Antares-Simulator,
// Adequacy and Performance assessment for interconnected energy networks.
//
// Antares_Simulator is free software: you can redistribute it and/or modify
// it under the terms of the Mozilla Public Licence 2.0 as published by
// the Mozilla Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Antares_Simulator is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// Mozilla Public Licence 2.0 for more details.
//
// You should have received a copy of the Mozilla Public Licence 2.0
// along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.

#include "antares/solver/simulation/sim_binding_constraints.h"

#include <numeric>
#include <span>
#include <vector>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/binding_constraint/BindingConstraint.h"
#include "antares/study/binding_constraint/BindingConstraintGroup.h"
#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

namespace
{
using TimeSerie = std::span<const double>;
using Weight = double;
using TimeSeriesAndWeight = std::vector<std::pair<TimeSerie, Weight>>;

TimeSerie timeSeriesColumn(Data::BindingConstraint* bc,
                            const Data::BindingConstraintGroupRepository& bcgroups,
                            int year)
{
    assert(bc->RHSTimeSeries().width && "Invalid constraint data width");

    uint tsIndexForBc = 0;
    if (auto* group = bcgroups[bc->group()])
    {
        tsIndexForBc = group->timeseriesNumbers[year];
    }

    // If there is only one TS, always select it.
    const auto ts_number = bc->RHSTimeSeries().width == 1 ? 0 : tsIndexForBc;
    auto& timeSeries = bc->RHSTimeSeries();
    const double* column = timeSeries[ts_number];
    return {column, timeSeries.height};
}

TimeSeriesAndWeight getMustRunClusterTimeSeriesAndWeight(Data::BindingConstraint* bc, int year)
{
    auto mustrun_clusters = bc->clusters()
                            | std::ranges::views::filter(
                              [](auto pair)
                              {
                                  const auto& [cluster, weight] = pair;
                                  return cluster->isEnabled() && cluster->isMustRun();
                              });
    TimeSeriesAndWeight time_seriesAndWeight;
    time_seriesAndWeight.reserve(std::distance(mustrun_clusters.begin(), mustrun_clusters.end()));
    std::ranges::transform(mustrun_clusters,
                           std::back_inserter(time_seriesAndWeight),
                           [year](auto pair)
                           {
                               auto& [cluster, weight] = pair;
                               auto ts = std::span<const double>{cluster->series.getColumn(year),
                                                                 cluster->series.timeSeries.height};
                               return std::pair{ts, weight};
                           });
    return time_seriesAndWeight;
}

double getClustersHourlyProduction(int PasDeTempsDebut,
                                   int pasDeTemps,
                                   const TimeSeriesAndWeight& clusterMustRunTimeSeriesAndWeight)
{
    return std::accumulate(clusterMustRunTimeSeriesAndWeight.begin(),
                           clusterMustRunTimeSeriesAndWeight.end(),
                           0.,
                           [pasDeTemps, PasDeTempsDebut](double acc, const auto pair)
                           {
                               const auto& [ts, weight] = pair;
                               return acc + ts[PasDeTempsDebut + pasDeTemps] * weight;
                           });
}

double getClusterDailyProduction(int PasDeTempsDebut,
                                 const TimeSeriesAndWeight& clusterMustRunTimeSeriesAndWeight,
                                 unsigned day)
{
    double sum = 0;
    for (unsigned int hour = 0; hour < HOURS_PER_DAY; ++hour)
    {
        sum += getClustersHourlyProduction(PasDeTempsDebut,
                                           day * HOURS_PER_DAY + hour,
                                           clusterMustRunTimeSeriesAndWeight);
    }
    return sum;
}
} // namespace

namespace Simulation
{
void prepareBindingConstraint(PROBLEME_HEBDO& problem,
                              int PasDeTempsDebut,
                              const Data::BindingConstraintsRepository& bindingConstraints,
                              const Data::BindingConstraintGroupRepository& bcgroups,
                              const uint weekFirstDay,
                              int pasDeTemps)
{
    const auto activeConstraints = bindingConstraints.activeConstraints();
    const auto constraintCount = activeConstraints.size();

    for (unsigned constraintIndex = 0; constraintIndex != constraintCount; ++constraintIndex)
    {
        auto bc = activeConstraints[constraintIndex];
        auto column = timeSeriesColumn(bc.get(), bcgroups, problem.year);
        auto clusterMustRunTimeSeriesAndWeight = getMustRunClusterTimeSeriesAndWeight(bc.get(),
                                                                                      problem.year);

        switch (bc->type())
        {
        case Data::BindingConstraint::typeHourly:
        {
            auto hourly_mustrun_production = getClustersHourlyProduction(
              PasDeTempsDebut,
              pasDeTemps,
              clusterMustRunTimeSeriesAndWeight);
            problem.MatriceDesContraintesCouplantes[constraintIndex]
              .SecondMembreDeLaContrainteCouplante[pasDeTemps]
              = column[PasDeTempsDebut + pasDeTemps] - hourly_mustrun_production;
            break;
        }
        case Data::BindingConstraint::typeDaily:
        {
            assert(weekFirstDay + 6 < bc->RHSTimeSeries().height
                   && "Invalid constraint data height");

            std::vector<double>& sndMember = problem
                                               .MatriceDesContraintesCouplantes[constraintIndex]
                                               .SecondMembreDeLaContrainteCouplante;

            for (unsigned day = 0; day != 7; ++day)
            {
                auto dailyClusterMustRunProduction = getClusterDailyProduction(

                  PasDeTempsDebut,
                  clusterMustRunTimeSeriesAndWeight,
                  day);
                sndMember[day] = column[weekFirstDay + day] - dailyClusterMustRunProduction;
            }

            break;
        }
        case Data::BindingConstraint::typeWeekly:
        {
            assert(weekFirstDay + 6 < bc->RHSTimeSeries().height
                   && "Invalid constraint data height");

            double sum = 0;
            for (unsigned day = 0; day != 7; ++day)
            {
                auto mustrun_production = getClusterDailyProduction(
                  PasDeTempsDebut,
                  clusterMustRunTimeSeriesAndWeight,
                  day);
                sum += column[weekFirstDay + day] - mustrun_production;
            }

            problem.MatriceDesContraintesCouplantes[constraintIndex]
              .SecondMembreDeLaContrainteCouplante[0]
              = sum;
            break;
        }
        case Data::BindingConstraint::typeUnknown:
        case Data::BindingConstraint::typeMax:
        default:
        {
            logs.error() << "internal error. Please submit a full bug report";
            assert(false && "invalid constraint type");
            break;
        }
        }
    }
}
} // namespace Simulation
