// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

namespace Antares::Solver
{
struct ModelerData;
}

namespace Antares::Optimization
{

bool hasModelerIntegerVariables(const Solver::ModelerData* modelerData);

} // namespace Antares::Optimization
