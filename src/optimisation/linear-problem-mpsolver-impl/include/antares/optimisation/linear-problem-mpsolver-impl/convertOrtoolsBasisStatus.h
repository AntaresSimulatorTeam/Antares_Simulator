// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once
#include <ortools/linear_solver/linear_solver.h>

#include "antares/optimisation/linear-problem-api/hasStatus.h"

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{
LinearProblemApi::MipBasisStatus convertOrtoolsBasisStatus(
  const operations_research::MPSolver::BasisStatus& status);

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
