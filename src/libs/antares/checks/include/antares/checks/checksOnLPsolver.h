
#pragma once

#include <list>
#include <string>

#include <antares/optimization-options/options.h>
#include "antares/study/fwd.h"

namespace Antares::Check
{
void checkForSolversExistence(Solver::Optimization::OptimizationOptions& solverOptions);
void checkSolverMILPincompatibility(Data::UnitCommitmentMode ucMode,
                                    const std::string& solverName);
}
