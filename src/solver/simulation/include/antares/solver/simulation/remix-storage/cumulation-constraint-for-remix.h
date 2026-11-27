// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

namespace Antares::Solver::Simulation
{

enum class CumulationVariable
{
    Withdrawal,
    Injection,
    Netting // injectionEff * injection - withdrawalEff * withdrawal
};

enum class CumulationSense
{
    Less,
    Greater,
    Equal
};

// Sum over hours of variable {<, >, =} rhs
struct CumulationConstraintForRemix
{
    std::vector<unsigned> hours; // 0-based, within the week
    CumulationVariable variable;
    CumulationSense sense;
    double rhs;
};

enum class RemixLever
{
    Withdrawal, // withdrawal moved from hourOfMaxGen to hourOfMinGen
    Injection   // injection moved from hourOfMinGen to hourOfMaxGen
};

// Max amount exchangeable with the given lever so that the constraint stays satisfied
double maxExchangeUnderCumulation(const CumulationConstraintForRemix& constraint,
                                  RemixLever lever,
                                  unsigned hourOfMaxGen,
                                  unsigned hourOfMinGen,
                                  const std::vector<double>& withdrawal,
                                  const std::vector<double>& injection,
                                  double withdrawalEff,
                                  double injectionEff);

// Current value of the constraint's left-hand side
double cumulationSum(const CumulationConstraintForRemix& constraint,
                     const std::vector<double>& withdrawal,
                     const std::vector<double>& injection,
                     double withdrawalEff,
                     double injectionEff);

} // namespace Antares::Solver::Simulation
