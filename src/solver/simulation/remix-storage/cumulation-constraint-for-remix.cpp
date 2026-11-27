// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/remix-storage/cumulation-constraint-for-remix.h"

#include <algorithm>
#include <limits>

namespace Antares::Solver::Simulation
{

double cumulationSum(const CumulationConstraintForRemix& constraint,
                     const std::vector<double>& withdrawal,
                     const std::vector<double>& injection,
                     double withdrawalEff,
                     double injectionEff)
{
    double sum = 0.;
    for (unsigned h: constraint.hours)
    {
        switch (constraint.variable)
        {
        case CumulationVariable::Withdrawal:
            sum += withdrawal[h];
            break;
        case CumulationVariable::Injection:
            sum += injection[h];
            break;
        case CumulationVariable::Netting:
            sum += injectionEff * injection[h] - withdrawalEff * withdrawal[h];
            break;
        }
    }
    return sum;
}

// Variation of the constraint's left-hand side per unit of exchanged amount
static double sumVariationPerUnit(const CumulationConstraintForRemix& constraint,
                                  RemixLever lever,
                                  unsigned hourOfMaxGen,
                                  unsigned hourOfMinGen,
                                  double withdrawalEff,
                                  double injectionEff)
{
    const auto contains = [&constraint](unsigned h)
    { return std::ranges::find(constraint.hours, h) != constraint.hours.end(); };

    // +1 : hourOfMaxGen only in the group, -1 : hourOfMinGen only, 0 : both or none
    const double s = (contains(hourOfMaxGen) ? 1. : 0.) - (contains(hourOfMinGen) ? 1. : 0.);

    switch (constraint.variable)
    {
    case CumulationVariable::Withdrawal:
        return lever == RemixLever::Withdrawal ? -s : 0.;
    case CumulationVariable::Injection:
        return lever == RemixLever::Injection ? s : 0.;
    case CumulationVariable::Netting:
        return (lever == RemixLever::Withdrawal ? withdrawalEff : injectionEff) * s;
    }
    return 0.;
}

double maxExchangeUnderCumulation(const CumulationConstraintForRemix& constraint,
                                  RemixLever lever,
                                  unsigned hourOfMaxGen,
                                  unsigned hourOfMinGen,
                                  const std::vector<double>& withdrawal,
                                  const std::vector<double>& injection,
                                  double withdrawalEff,
                                  double injectionEff)
{
    constexpr double unbounded = std::numeric_limits<double>::max();

    const double coef = sumVariationPerUnit(constraint,
                                            lever,
                                            hourOfMaxGen,
                                            hourOfMinGen,
                                            withdrawalEff,
                                            injectionEff);
    if (coef == 0.)
    {
        return unbounded;
    }

    const double sum = cumulationSum(constraint, withdrawal, injection, withdrawalEff, injectionEff);

    switch (constraint.sense)
    {
    case CumulationSense::Less:
        return coef > 0. ? std::max(0., constraint.rhs - sum) / coef : unbounded;
    case CumulationSense::Greater:
        return coef < 0. ? std::max(0., sum - constraint.rhs) / -coef : unbounded;
    case CumulationSense::Equal:
        return 0.;
    }
    return unbounded;
}

} // namespace Antares::Solver::Simulation
