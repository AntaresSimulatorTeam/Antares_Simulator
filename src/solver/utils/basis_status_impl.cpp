// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "basis_status_impl.h"

#include <vector>

#include "ortools/linear_solver/linear_solver.h"

template<class SourceT>
static void transferBasis(std::vector<operations_research::MPSolver::BasisStatus>& destination,
                          const SourceT& source)
{
    destination.resize(source.size());
    for (size_t idx = 0; idx < source.size(); idx++)
    {
        destination[idx] = source[idx]->basis_status();
    }
}

namespace Antares::Optimization
{
void BasisStatusImpl::setStartingBasis(operations_research::MPSolver* solver) const
{
    solver->SetStartingLpBasis(StatutDesVariables, StatutDesContraintes);
}

void BasisStatusImpl::extractBasis(const operations_research::MPSolver* solver)
{
    transferBasis(StatutDesVariables, solver->variables());
    transferBasis(StatutDesContraintes, solver->constraints());
}

bool BasisStatusImpl::exists() const
{
    return !StatutDesVariables.empty() && !StatutDesContraintes.empty();
}

void BasisStatusImpl::clear()
{
    StatutDesVariables.clear();
    StatutDesContraintes.clear();
}
} // namespace Antares::Optimization
