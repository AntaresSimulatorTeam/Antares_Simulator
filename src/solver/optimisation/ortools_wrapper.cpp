// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/ortools_wrapper.h"

// ignore unused parameters warnings from ortools
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

#include <spx_constantes_externes.h>

#include <antares/exception/LoadingError.hpp>

using namespace operations_research;
using Antares::Optimization::SingleOptimOptions;

const std::string XPRESS_PARAMS = "THREADS 1";
const std::string SCIP_PARAMS = "parallel/maxnthreads 1";
const std::string GUROBI_PARAMS
  = "Threads 1"; // See
                 // https://docs.gurobi.com/projects/optimizer/en/current/reference/parameters.html#threads
                 // for Gurobi threads (and other) parameter reference

// MPSolverParameters's copy constructor is private
static void setGenericParameters(MPSolverParameters& params)
{
    params.SetIntegerParam(MPSolverParameters::SCALING, 0);
    params.SetIntegerParam(MPSolverParameters::PRESOLVE, 0);
    // ortools default is 1e-7 for primal tolerance, but this may be too high as we manipulate large
    // values in the problem. Then 1e-7 may be too hard to achieve and has lead to declare some
    // problems infeasible whereas they were not (contraints were active but not violated). Sirius
    // uses 1e-6 (and this cannot be changed with ortools), this has effect for all solvers except
    // sirius
    params.SetDoubleParam(MPSolverParameters::PRIMAL_TOLERANCE, 1e-6);
}

static void checkSetSolverSpecificParameters(bool status,
                                             const std::string& solverName,
                                             const std::string& specificParameters)
{
    if (!status)
    {
        throw Antares::Error::InvalidSolverSpecificParameters(solverName, specificParameters);
    }
}

static void TuneSolverSpecificOptions(MPSolver* solver,
                                      const std::string& solverName,
                                      const std::string& solverParameters)
{
    if (!solver)
    {
        return;
    }

    bool status;
    std::string specificParams;

    switch (solver->ProblemType())
    {
    // Allow solver to use only one thread
    case MPSolver::XPRESS_LINEAR_PROGRAMMING:
    case MPSolver::XPRESS_MIXED_INTEGER_PROGRAMMING:
    {
        specificParams = XPRESS_PARAMS + " " + solverParameters;
        status = solver->SetSolverSpecificParametersAsString(specificParams);
        checkSetSolverSpecificParameters(status, solverName, specificParams);
        break;
    }
    case MPSolver::SCIP_MIXED_INTEGER_PROGRAMMING:
    {
        specificParams = SCIP_PARAMS + ", " + solverParameters;
        status = solver->SetSolverSpecificParametersAsString(specificParams);
        checkSetSolverSpecificParameters(status, solverName, specificParams);
        break;
    }
    case MPSolver::GUROBI_LINEAR_PROGRAMMING:
    case MPSolver::GUROBI_MIXED_INTEGER_PROGRAMMING:
    {
        specificParams = GUROBI_PARAMS + "\n" + solverParameters;
        status = solver->SetSolverSpecificParametersAsString(specificParams);
        checkSetSolverSpecificParameters(status, solverName, specificParams);
        break;
    }
    default:
        break;
    }
}

static bool solverSupportsWarmStart(const MPSolver::OptimizationProblemType solverType)
{
    switch (solverType)
    {
    case MPSolver::SIRIUS_LINEAR_PROGRAMMING:
    case MPSolver::XPRESS_LINEAR_PROGRAMMING:
        return true;
    default:
        return false;
    }
}

static void extractSolutionValues(const std::vector<MPVariable*>& variables,
                                  PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre)
{
    int nbVar = problemeAResoudre->NombreDeVariables;
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        const MPVariable* var = variables[idxVar];
        problemeAResoudre->X[idxVar] = var->solution_value();
    }
}

static void extractReducedCosts(const std::vector<MPVariable*>& variables,
                                PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre)
{
    int nbVar = problemeAResoudre->NombreDeVariables;
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        const MPVariable* var = variables[idxVar];
        problemeAResoudre->CoutsReduits[idxVar] = var->reduced_cost();
    }
}

static void extractDualValues(const std::vector<MPConstraint*>& constraints,
                              PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre)
{
    int nbRows = ProblemeAResoudre->NombreDeContraintes;
    for (int idxRow = 0; idxRow < nbRows; ++idxRow)
    {
        const MPConstraint* row = constraints[idxRow];
        ProblemeAResoudre->CoutsMarginauxDesContraintes[idxRow] = row->dual_value();
    }
}

static void extract_from_MPSolver(const MPSolver* solver,
                                  PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre)
{
    assert(solver);
    assert(problemeAResoudre);

    const bool isMIP = solver->IsMIP();

    extractSolutionValues(solver->variables(), problemeAResoudre);

    if (isMIP)
    {
        // TODO extract dual values & marginal costs from LP with fixed integer variables
        const int nbVar = problemeAResoudre->NombreDeVariables;
        std::fill(problemeAResoudre->CoutsReduits.data(),
                  problemeAResoudre->CoutsReduits.data() + nbVar,
                  0.);
        const int nbRows = problemeAResoudre->NombreDeContraintes;
        std::fill(problemeAResoudre->CoutsMarginauxDesContraintes.data(),
                  problemeAResoudre->CoutsMarginauxDesContraintes.data() + nbRows,
                  0.);
    }
    else
    {
        extractReducedCosts(solver->variables(), problemeAResoudre);
        extractDualValues(solver->constraints(), problemeAResoudre);
    }
}

static bool solveAndManageStatus(MPSolver* solver,
                                 int& resultStatus,
                                 const MPSolverParameters& params)
{
    auto status = solver->Solve(params);
    if (status == MPSolver::OPTIMAL || status == MPSolver::FEASIBLE)
    {
        resultStatus = OUI_SPX;
    }
    else
    {
        resultStatus = NON_SPX;
    }

    return resultStatus == OUI_SPX;
}

static bool doWeGiveBasisToSolver(const SingleOptimOptions& options,
                                  const MPSolver* solver,
                                  const PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre)
{
    return solverSupportsWarmStart(solver->ProblemType()) && problemeAResoudre->basisStatus.exists()
           && options.solverUsesBasis;
}

static bool doWeStoreSolverBasis(const SingleOptimOptions& options, const MPSolver* solver)
{
    return solverSupportsWarmStart(solver->ProblemType()) && options.solverExportsBasis;
}

void ORTOOLS_Simplexe(PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre,
                      MPSolver* solver,
                      const SingleOptimOptions& options)
{
    MPSolverParameters params;
    // Keep generic params for default settings working for all solvers
    setGenericParameters(params);
    if (options.solverLogs) // May be overriden by log level if set as specific parameters
    {
        solver->EnableOutput();
    }
    TuneSolverSpecificOptions(solver, options.solverName, options.solverParameters);

    if (doWeGiveBasisToSolver(options, solver, problemeAResoudre))
    {
        problemeAResoudre->basisStatus.setStartingBasis(solver);
    }

    if (solveAndManageStatus(solver, problemeAResoudre->ExistenceDUneSolution, params))
    {
        extract_from_MPSolver(solver, problemeAResoudre);
        if (doWeStoreSolverBasis(options, solver))
        {
            problemeAResoudre->basisStatus.extractBasis(solver);
        }
    }
}
