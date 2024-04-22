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
#pragma once

// NOTE
// Try not to include this header too often, if possible, forward-declare BasisStatus
// since linear_solver.h takes time to compile

#include <vector>
#include "ortools/linear_solver/linear_solver.h"

namespace Antares::Optimization
{
struct BasisStatus
{
    bool exists() const;
    void setStarting(operations_research::MPSolver* solver) const;
    void extract(const operations_research::MPSolver* solver);
    using Status = operations_research::MPSolver::BasisStatus;
    std::vector<Status> StatutDesVariables;
    std::vector<Status> StatutDesContraintes;
};
} // namespace Antares::Optimization
