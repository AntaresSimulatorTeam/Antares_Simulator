#include "include/antares/checks/checksOnLPsolver.h"

#include <boost/algorithm/string/join.hpp>

#include <antares/exception/LoadingError.hpp>
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares;
using namespace Antares::Solver::Optimization;

namespace Antares::Check
{
static void checkSolverExists(const std::string solverName, const std::list<std::string> availableSolversList)
{
    // Check if solver is available
    bool found = std::ranges::find(availableSolversList, solverName) != availableSolversList.end();
    if (!found)
    {
        throw Error::InvalidSolver(solverName, boost::algorithm::join(availableSolversList, ","));
    }
}

void checkForSolversExistence(const OptimizationOptions& solverOptions)
{
    checkSolverExists(solverOptions.linearSolver, getAvailableLinearSolverNames());
    checkSolverExists(solverOptions.quadraticSolver, getAvailableQuadraticSolverNames());
}

void checkSolverMILPoptionsConsistency(const OptimizationOptions& solverOptions)
{
    if (solverOptions.linearSolver == "sirius")
    {
        throw Error::IncompatibleMILPOrtoolsSolver();
    }
}

void checkForSolverOptionsConsistency(const OptimizationOptions& solverOptions)
{
    bool UserSuppliedParamsBothOptims = !solverOptions.linearSolverParameters.empty();
    bool UserSuppliedParamsOptim1 = !solverOptions.lpSolverParamOptim1.empty();
    bool UserSuppliedParamsOptim2 = !solverOptions.lpSolverParamOptim2.empty();

    if (UserSuppliedParamsBothOptims && (UserSuppliedParamsOptim1 || UserSuppliedParamsOptim2))
    {
        throw Error::IncompatibleLinearSolverParameters();
    }
}
} // namespace Antares::Check
