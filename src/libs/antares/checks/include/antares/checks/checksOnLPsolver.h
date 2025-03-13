
#pragma once

#include <list>
#include <string>

#include <antares/optimization-options/options.h>
#include "antares/study/fwd.h"

namespace Antares::Check
{
void checkSolverOptions(const Antares::Solver::Optimization::CmdLineOptimOptions& solverOptions,
                        bool milpRequired);
void checkForSolversExistence(const Solver::Optimization::CmdLineOptimOptions& solverOptions);
void checkSolverMILPoptionsConsistency(const Solver::Optimization::CmdLineOptimOptions& solverOptions);
void checkForSolverOptionsConsistency(const Solver::Optimization::CmdLineOptimOptions& solverOptions);
}
