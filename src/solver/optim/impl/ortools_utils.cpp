#include "antares/solver/utils/ortools_utils.h"

#include <antares/logs/logs.h>
#include <antares/exception/AssertionError.hpp>
#include <antares/Enum.hpp>
#include <filesystem>

using namespace operations_research;

const char* const XPRESS_PARAMS = "THREADS 1";

// MPSolverParameters's copy constructor is private
static void setGenericParameters(MPSolverParameters& params)
{
    params.SetIntegerParam(MPSolverParameters::SCALING, 0);
    params.SetIntegerParam(MPSolverParameters::PRESOLVE, 0);
}

static bool solverSupportsWarmStart(const MPSolver::OptimizationProblemType solverType)
{
    switch (solverType)
    {
    case MPSolver::XPRESS_LINEAR_PROGRAMMING:
        return true;
    default:
        return false;
    }
}

namespace Antares::optim::impl
{

    void TuneSolverSpecificOptions(MPSolver *solver)
    {
        if (!solver)
            return;

        switch (solver->ProblemType()) {
            case MPSolver::XPRESS_LINEAR_PROGRAMMING:
            case MPSolver::XPRESS_MIXED_INTEGER_PROGRAMMING:
                solver->SetSolverSpecificParametersAsString(XPRESS_PARAMS);
                break;
                // Add solver-specific options here
            default:
                break;
        }
        /* TODO if (problemeSimplexe_->SolverLogs())
        {
            solver->EnableOutput();
        }*/
    }
}

const std::map<std::string, struct OrtoolsUtils::SolverNames> OrtoolsUtils::solverMap
  = {{"xpress", {"xpress_lp", "xpress"}},
     {"sirius", {"sirius_lp", "sirius"}},
     {"coin", {"clp", "cbc"}},
     {"glpk", {"glpk_lp", "glpk"}},
     {"scip", {"scip", "scip"}}};

std::list<std::string> getAvailableOrtoolsSolverName()
{
    std::list<std::string> result;

    for (const auto& solverName : OrtoolsUtils::solverMap)
    {
        MPSolver::OptimizationProblemType solverType;
        MPSolver::ParseSolverType(solverName.second.LPSolverName, &solverType);

        if (MPSolver::SupportsProblemType(solverType))
            result.push_back(solverName.first);
    }
    return result;
}

std::string availableOrToolsSolversString()
{
  const std::list<std::string> availableSolverList = getAvailableOrtoolsSolverName();
  std::ostringstream solvers;
  for (const std::string& avail : availableSolverList)
  {
    bool last = &avail == &availableSolverList.back();
    std::string sep = last ? "." : ", ";
    solvers << avail << sep;
  }
  return solvers.str();
}

MPSolver* MPSolverFactory(bool isMip,
                          const std::string& solverName)
{
    MPSolver* solver;
    try
    {
        if (isMip)
            solver = MPSolver::CreateSolver((OrtoolsUtils::solverMap.at(solverName)).MIPSolverName);
        else
            solver = MPSolver::CreateSolver((OrtoolsUtils::solverMap.at(solverName)).LPSolverName);

        if (!solver)
        {
            std::string msg_to_throw = "Solver " + solverName + " not found. \n";
            msg_to_throw += "Please make sure that your OR-Tools install supports solver " + solverName + ".";

            throw Antares::Data::AssertionError(msg_to_throw);
        }
    }
    catch (...)
    {
        Antares::logs.error() << "Solver creation failed.";
        throw;
    }

    return solver;
}
