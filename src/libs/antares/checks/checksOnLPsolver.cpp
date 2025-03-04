#include "include/antares/checks/checksOnLPsolver.h"

#include <boost/algorithm/string/join.hpp>

#include <antares/exception/LoadingError.hpp>
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares;

namespace Antares::Check
{
void checkSolverExists(std::string solverName, const std::list<std::string> availableSolversList)
{
    // Check if solver is available
    bool found = std::ranges::find(availableSolversList, solverName) != availableSolversList.end();
    if (!found)
    {
        throw Error::InvalidSolver(solverName, boost::algorithm::join(availableSolversList, ","));
    }
}

void checkForSolversExistence(Solver::Optimization::OptimizationOptions& solverOptions)
{
    checkSolverExists(solverOptions.linearSolver, getAvailableLinearSolverNames());
    checkSolverExists(solverOptions.quadraticSolver, getAvailableQuadraticSolverNames());
}

void checkSolverMILPincompatibility(Antares::Data::UnitCommitmentMode ucMode,
                                    const std::string& solverName)
{
    using namespace Antares::Data;
    if (ucMode == UnitCommitmentMode::ucMILP && solverName == "sirius")
    {
        throw Error::IncompatibleMILPOrtoolsSolver();
    }
}
} // namespace Antares::Check
