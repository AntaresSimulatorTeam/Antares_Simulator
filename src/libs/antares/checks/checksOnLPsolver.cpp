#include "include/antares/checks/checksOnLPsolver.h"

#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include "antares/solver/utils/ortools_utils.h"

using namespace Antares;
using namespace Antares::Solver::Optimization;

namespace Antares::Check
{
static void checkSolverExists(const std::string solverName,
                              const std::list<std::string> availableSolversList)
{
    bool found = std::ranges::find(availableSolversList, solverName) != availableSolversList.end();
    if (!found)
    {
        throw Error::InvalidSolver(solverName, toString(availableSolversList));
    }
}

static void checkForSolversExistence(const CmdLineOptimOptions& solverOptions)
{
    checkSolverExists(solverOptions.linearSolver, availableLinearSolversList());
    checkSolverExists(solverOptions.quadraticSolver, availableQuadraticSolversList());
}

static void checkSolverMILPoptionsConsistency(const CmdLineOptimOptions& solverOptions)
{
    if (solverOptions.linearSolver == "sirius")
    {
        throw Error::IncompatibleMILPOrtoolsSolver();
    }

    bool UserSuppliedParamsOptim1 = !solverOptions.lpSolverParamOptim1.empty();
    bool UserSuppliedParamsOptim2 = !solverOptions.lpSolverParamOptim2.empty();
    if (UserSuppliedParamsOptim1 || UserSuppliedParamsOptim2)
    {
        throw Error::UseMILPsolverWithWrongOptions();
    }
}

static void checkForSolverOptionsConsistency(const CmdLineOptimOptions& solverOptions)
{
    bool UserSuppliedParamsBothOptims = !solverOptions.linearSolverParameters.empty();
    bool UserSuppliedParamsOptim1 = !solverOptions.lpSolverParamOptim1.empty();
    bool UserSuppliedParamsOptim2 = !solverOptions.lpSolverParamOptim2.empty();

    if (UserSuppliedParamsBothOptims && (UserSuppliedParamsOptim1 || UserSuppliedParamsOptim2))
    {
        throw Error::IncompatibleLinearSolverParameters();
    }
}

void checkSolverOptions(const CmdLineOptimOptions& solverOptions, bool milpRequired)
{
    checkForSolversExistence(solverOptions);
    checkForSolverOptionsConsistency(solverOptions);

    if (milpRequired)
    {
        checkSolverMILPoptionsConsistency(solverOptions);
    }

    logs.info() << "  :: solver " << solverOptions.linearSolver
                << " is used for linear problem resolution";

    logs.info() << "  :: solver " << solverOptions.quadraticSolver
                << " is used for quadratic problem resolution";

    logs.info() << "  :: Printing solver logs : " << (solverOptions.solverLogs ? "True" : "False");
}
} // namespace Antares::Check
