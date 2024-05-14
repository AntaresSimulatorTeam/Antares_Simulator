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
} // namespace Antares::Optimization
