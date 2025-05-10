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

#include "antares/solver/simulation/sim_binding_constraints_rhs.h"

#include <numeric>
#include <span>

#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/study/binding_constraint/BindingConstraint.h"
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

using namespace Antares::Data;
using clusterWeightMap = Antares::Data::BindingConstraint::clusterWeightMap;
using TimeSerie = std::span<const double>;

TimeSerie fetchBindingConstraintRHS(const BindingConstraint* bc,
                                    const BindingConstraintGroupRepository& bcGroups,
                                    int year)
{
    assert(bc->RHSTimeSeries().width && "Invalid constraint data width");

    unsigned ts_number = 0;
    if (auto* group = bcGroups[bc->group()])
    {
        // If there is only one TS, always select it.
        ts_number = bc->RHSTimeSeries().width == 1 ? 0 : group->timeseriesNumbers[year];
    }

    const double* TS = bc->RHSTimeSeries()[ts_number];
    return {TS, bc->RHSTimeSeries().height};
}

auto filterByMustrunCluster(const clusterWeightMap& map)
{
    return map
           | std::ranges::views::filter(
             [](auto pair) { return pair.first->isEnabled() && pair.first->isMustRun(); });
}

std::array<double, 7>& operator*(std::array<double, 7>& left, const double& scalar)
{
    for (unsigned i = 0; i < 7; i++)
    {
        left[i] *= scalar;
    }
    return left;
}

std::array<double, 7>& operator+=(std::array<double, 7>& left, const std::array<double, 7>& right)
{
    for (unsigned i = 0; i < 7; i++)
    {
        left[i] += right[i];
    }
    return left;
}

std::array<double, 7> accumulateByDay(const TimeSerie& ts)
{
    if (ts.size() != HOURS_PER_WEEK) // ts has to be an hourly TS, covering a week
    {
        throw std::invalid_argument("Trying to make a daily TS of a non 168 values TS");
    }
    std::array<double, 7> to_return;
    std::ranges::fill(to_return, 0.); // std::array initialization
    for (unsigned i = 0; i < 7; i++)
    {
        to_return[i] = std::accumulate(ts.begin() + i * HOURS_PER_DAY,
                                       ts.begin() + (i + 1) * HOURS_PER_DAY,
                                       0.);
    }
    return to_return;
}

std::array<double, 7> computeMustrunDailyTerms(const BindingConstraint* bc,
                                               const unsigned year,
                                               const unsigned PasDeTempsDebut)
{
    std::array<double, 7> to_return;
    std::ranges::fill(to_return, 0.); // std::array initialization
    auto mustrunClustersWeigths = filterByMustrunCluster(bc->clustersAndWeights());
    for (auto& [cluster, weight]: mustrunClustersWeigths)
    {
        auto hourlyProductionTS = TimeSerie{cluster->series.getColumn(year) + PasDeTempsDebut,
                                            HOURS_PER_WEEK};
        std::array<double, 7> dailyProductionTS = accumulateByDay(hourlyProductionTS);
        to_return += dailyProductionTS * weight;
    }
    return to_return;
}

static void setRHSforHourlyBC()
{
}

static void setRHSforDailyBC(PROBLEME_HEBDO& problem,
                             const BindingConstraint* bc,
                             const BindingConstraintGroupRepository& bcGroups,
                             const unsigned PasDeTempsDebut,
                             const unsigned weekFirstDay,
                             const unsigned bcIndex)
{
    std::vector<double>& rhs = problem.MatriceDesContraintesCouplantes[bcIndex]
                                 .SecondMembreDeLaContrainteCouplante;
}

static void setRHSforWeeklyBC(PROBLEME_HEBDO& problem,
                              const BindingConstraint* bc,
                              const BindingConstraintGroupRepository& bcGroups,
                              const unsigned PasDeTempsDebut,
                              const unsigned weekFirstDay,
                              const unsigned bcIndex)
{
    assert(weekFirstDay + 6 < bc->RHSTimeSeries().height && "Invalid constraint data height");

    TimeSerie dailyBCrhs = fetchBindingConstraintRHS(bc, bcGroups, problem.year);
    std::array<double, 7> mustrunDailyTerms = computeMustrunDailyTerms(bc,
                                                                       problem.year,
                                                                       PasDeTempsDebut);

    double sum = 0.;
    for (unsigned day = 0; day != 7; ++day)
    {
        sum += dailyBCrhs[weekFirstDay + day] - mustrunDailyTerms[day];
    }
    problem.MatriceDesContraintesCouplantes[bcIndex].SecondMembreDeLaContrainteCouplante[0] = sum;
}

namespace Simulation
{
void setBindingConstraintsRHS(PROBLEME_HEBDO& problem,
                              const BindingConstraintsRepository& bindingConstraints,
                              const BindingConstraintGroupRepository& bcGroups,
                              const unsigned PasDeTempsDebut,
                              const unsigned NombreDePasDeTemps,
                              const unsigned weekFirstDay)
{
    unsigned bcIndex = 0;
    for (const auto& bc: bindingConstraints.activeConstraints())
    {
        switch (bc->type())
        {
        case Data::BindingConstraint::typeHourly:
        {
            setRHSforHourlyBC();
            break;
        }
        case Data::BindingConstraint::typeDaily:
        {
            setRHSforDailyBC(problem, bc.get(), bcGroups, PasDeTempsDebut, weekFirstDay, bcIndex);
            break;
        }
        case Data::BindingConstraint::typeWeekly:
        {
            setRHSforWeeklyBC(problem, bc.get(), bcGroups, PasDeTempsDebut, weekFirstDay, bcIndex);
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
        bcIndex++;
    }
}
} // namespace Simulation
