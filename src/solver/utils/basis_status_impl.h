// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <vector>

#include "ortools/linear_solver/linear_solver.h"

namespace Test
{
class BasisStatus;
}

namespace Antares::Optimization
{
class BasisStatusImpl final
{
private:
    friend class BasisStatus;
    friend class Test::BasisStatus; // For tests

    using Status = operations_research::MPSolver::BasisStatus;
    std::vector<Status> StatutDesVariables;
    std::vector<Status> StatutDesContraintes;

    void setStartingBasis(operations_research::MPSolver* solver) const;
    void extractBasis(const operations_research::MPSolver* solver);
    bool exists() const;
    void clear();
};
} // namespace Antares::Optimization
