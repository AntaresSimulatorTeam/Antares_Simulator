// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

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
    auto* group = bcGroups[bc->group()];
    if (group && bc->RHSTimeSeries().width != 1)
    {
        ts_number = group->timeseriesNumbers[year];
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

std::vector<double> operator*(const std::span<const double>& left, const double& scalar)
{
    std::vector<double> to_return(left.size(), 0.);
    for (unsigned i = 0; i < left.size(); i++)
    {
        to_return[i] = left[i] * scalar;
    }
    return to_return;
}

std::vector<double>& operator+=(std::vector<double>& left, const std::vector<double>& right)
{
    std::ranges::transform(left, right, left.begin(), std::plus<double>());
    return left;
}

std::vector<double> accumulateByDay(const TimeSerie& ts)
{
    if (ts.size() != Constants::nbHoursInAWeek) // ts must be an hourly TS, covering a week
    {
        throw std::invalid_argument("Trying to make a daily TS of a non 168 values TS");
    }

    std::vector<double> to_return(7, 0.);
    for (unsigned day = 0; day < 7; day++)
    {
        to_return[day] = std::accumulate(ts.begin() + day * HOURS_PER_DAY,
                                         ts.begin() + (day + 1) * HOURS_PER_DAY,
                                         0.);
    }
    return to_return;
}

std::vector<double> computeMustrunDailyTerms(const BindingConstraint* bc,
                                             const unsigned year,
                                             const unsigned PasDeTempsDebut)
{
    std::vector<double> to_return(7, 0.);

    auto mustrunClustersWeigths = filterByMustrunCluster(bc->clustersAndWeights());
    for (auto& [cluster, weight]: mustrunClustersWeigths)
    {
        auto hourlyProductionTS = TimeSerie{cluster->series.getColumn(year) + PasDeTempsDebut,
                                            Constants::nbHoursInAWeek};
        std::vector<double> dailyProductionTS = accumulateByDay(hourlyProductionTS);
        to_return += dailyProductionTS * weight;
    }
    return to_return;
}

std::vector<double> computeMustrunHourlyTerms(const BindingConstraint* bc,
                                              const unsigned year,
                                              const unsigned PasDeTempsDebut)
{
    std::vector<double> to_return(Constants::nbHoursInAWeek, 0.);

    auto mustrunClustersWeigths = filterByMustrunCluster(bc->clustersAndWeights());
    for (auto& [cluster, weight]: mustrunClustersWeigths)
    {
        auto hourlyProductionTS = TimeSerie{cluster->series.getColumn(year) + PasDeTempsDebut,
                                            Constants::nbHoursInAWeek};
        to_return += hourlyProductionTS * weight;
    }
    return to_return;
}

static void setRHSforHourlyBC(PROBLEME_HEBDO& problem,
                              const BindingConstraint* bc,
                              const BindingConstraintGroupRepository& bcGroups,
                              const unsigned PasDeTempsDebut,
                              const unsigned bcIndex)
{
    TimeSerie hourlyBCrhs = fetchBindingConstraintRHS(bc, bcGroups, problem.year);
    std::vector<double> mustrunHourlyTerms = computeMustrunHourlyTerms(bc,
                                                                       problem.year,
                                                                       PasDeTempsDebut);
    std::vector<double>& rhs = problem.MatriceDesContraintesCouplantes[bcIndex]
                                 .SecondMembreDeLaContrainteCouplante;

    for (unsigned hour = 0; hour < Constants::nbHoursInAWeek; ++hour)
    {
        rhs[hour] = hourlyBCrhs[PasDeTempsDebut + hour] - mustrunHourlyTerms[hour];
    }
}

static void setRHSforDailyBC(PROBLEME_HEBDO& problem,
                             const BindingConstraint* bc,
                             const BindingConstraintGroupRepository& bcGroups,
                             const unsigned PasDeTempsDebut,
                             const unsigned weekFirstDay,
                             const unsigned bcIndex)
{
    assert(weekFirstDay + 6 < bc->RHSTimeSeries().height && "Invalid constraint data height");

    TimeSerie dailyBCrhs = fetchBindingConstraintRHS(bc, bcGroups, problem.year);
    std::vector<double> mustrunDailyTerms = computeMustrunDailyTerms(bc,
                                                                     problem.year,
                                                                     PasDeTempsDebut);
    std::vector<double>& rhs = problem.MatriceDesContraintesCouplantes[bcIndex]
                                 .SecondMembreDeLaContrainteCouplante;

    for (unsigned day = 0; day != 7; ++day)
    {
        rhs[day] = dailyBCrhs[weekFirstDay + day] - mustrunDailyTerms[day];
    }
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
    std::vector<double> mustrunDailyTerms = computeMustrunDailyTerms(bc,
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
                              const unsigned weekFirstDay)
{
    unsigned bcIndex = 0;
    for (const auto& bc: bindingConstraints.activeConstraints())
    {
        switch (bc->type())
        {
        case Data::BindingConstraint::typeHourly:
        {
            setRHSforHourlyBC(problem, bc.get(), bcGroups, PasDeTempsDebut, bcIndex);
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
