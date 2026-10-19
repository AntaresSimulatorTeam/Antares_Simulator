// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/optimisation/linear-problem-mpsolver-impl/convertOrtoolsBasisStatus.h"

namespace Antares::LinearProblem::MpsolverImpl
{
Api::MipBasisStatus convertOrtoolsBasisStatus(
  const operations_research::MPSolver::BasisStatus& status)
{
    switch (status)
    {
    case operations_research::MPSolver::BasisStatus::FREE:
        return Api::MipBasisStatus::FREE;
    case operations_research::MPSolver::BasisStatus::AT_LOWER_BOUND:
        return Api::MipBasisStatus::AT_LOWER_BOUND;
    case operations_research::MPSolver::BasisStatus::AT_UPPER_BOUND:
        return Api::MipBasisStatus::AT_UPPER_BOUND;
    case operations_research::MPSolver::BasisStatus::FIXED_VALUE:
        return Api::MipBasisStatus::FIXED_VALUE;
    case operations_research::MPSolver::BasisStatus::BASIC:
        return Api::MipBasisStatus::BASIC;
    default:
        return Api::MipBasisStatus::NOT_AVAILABLE;
    }
}
} // namespace Antares::LinearProblem::MpsolverImpl
