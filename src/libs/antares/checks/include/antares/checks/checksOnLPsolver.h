
#pragma once

#include <list>
#include <string>

#include <antares/optimization-options/options.h>

namespace Antares::Check
{
void checkSolverOptions(const Solver::Optimization::CmdLineOptimOptions& solverOptions,
                        bool milpRequired = false);
} // namespace Antares::Check
