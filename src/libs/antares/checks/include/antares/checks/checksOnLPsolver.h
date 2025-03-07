
#pragma once

#include <list>
#include <string>

#include <antares/optimization-options/options.h>
#include "antares/study/fwd.h"

namespace Antares::Check
{
void checkForSolversExistence(const Solver::Optimization::OptimizationOptions& solverOptions);
void checkSolverMILPoptionsConsistency(const Solver::Optimization::OptimizationOptions& solverOptions);
void checkForSolverOptionsConsistency(const Solver::Optimization::OptimizationOptions& solverOptions);
}
