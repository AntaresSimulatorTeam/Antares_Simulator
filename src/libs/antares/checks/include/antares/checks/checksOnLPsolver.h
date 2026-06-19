
// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <list>
#include <string>

#include <antares/optimization-options/options.h>

namespace Antares::Check
{
void checkSolverOptions(const Solver::Optimization::CmdLineOptimOptions& solverOptions,
                        bool milpRequired = false);
} // namespace Antares::Check
