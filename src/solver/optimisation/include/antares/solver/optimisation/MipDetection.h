// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/study/system-model/optimConfig.h>

namespace Antares::Solver
{
struct ModelerData;
}

namespace Antares::Optimization
{
bool hasIntegerVariables(const Solver::ModelerData* modelerData,
                         Antares::Solver::Config::Location location);
} // namespace Antares::Optimization
