/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/solver/utils/ortools_utils.h"

#include <filesystem>
#include <optional>

#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include "antares/antares/Enum.hpp"
#include "antares/solver/utils/basis_status.h"

using namespace operations_research;

const std::string XPRESS_PARAMS = "THREADS 1";
const std::string SCIP_PARAMS = "parallel/maxnthreads 1";

using Antares::Solver::Optimization::OptimizationOptions;

// MPSolverParameters's copy constructor is private
static void setGenericParameters(MPSolverParameters& params)
{
    params.SetIntegerParam(MPSolverParameters::SCALING, 0);
    params.SetIntegerParam(MPSolverParameters::PRESOLVE, 0);
}

static void checkSetSolverSpecificParameters(bool status,
                                             const std::string& solverName,
                                             const std::string& specificParameters)
{
    if (!status)
    {
        throw Antares::Error::InvalidSolverSpecificParameters(solverName, specificParameters);
    }
    else
    {
        Antares::logs.info() << "  Successfully set " + solverName + " solver specific parameters";
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
    default:
        break;
    }
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

static void extractSolutionValues(const std::vector<MPVariable*>& variables,
                                  Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    int nbVar = problemeSimplexe->NombreDeVariables;
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        const MPVariable* var = variables[idxVar];
        problemeSimplexe->X[idxVar] = var->solution_value();
    }
}

static void extractReducedCosts(const std::vector<MPVariable*>& variables,
                                Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    int nbVar = problemeSimplexe->NombreDeVariables;
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        const MPVariable* var = variables[idxVar];
        problemeSimplexe->CoutsReduits[idxVar] = var->reduced_cost();
    }
}

static void extractDualValues(const std::vector<MPConstraint*>& constraints,
                              Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    int nbRows = problemeSimplexe->NombreDeContraintes;
    for (int idxRow = 0; idxRow < nbRows; ++idxRow)
    {
        const MPConstraint* row = constraints[idxRow];
        problemeSimplexe->CoutsMarginauxDesContraintes[idxRow] = row->dual_value();
    }
}

static void extract_from_MPSolver(const MPSolver* solver,
                                  Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    assert(solver);
    assert(problemeSimplexe);

    const bool isMIP = problemeSimplexe->isMIP();

    extractSolutionValues(solver->variables(), problemeSimplexe);

    if (isMIP)
    {
        // TODO extract dual values & marginal costs from LP with fixed integer variables
        const int nbVar = problemeSimplexe->NombreDeVariables;
        std::fill(problemeSimplexe->CoutsReduits, problemeSimplexe->CoutsReduits + nbVar, 0.);
        const int nbRows = problemeSimplexe->NombreDeContraintes;
        std::fill(problemeSimplexe->CoutsMarginauxDesContraintes,
                  problemeSimplexe->CoutsMarginauxDesContraintes + nbRows,
                  0.);
    }
    else
    {
        extractReducedCosts(solver->variables(), problemeSimplexe);
        extractDualValues(solver->constraints(), problemeSimplexe);
    }
}

std::string generateTempPath(const std::string& filename)
{
    namespace fs = std::filesystem;
    std::ostringstream tmpPath;
    tmpPath << fs::temp_directory_path().string() << Yuni::IO::SeparatorAsString << filename;
    return tmpPath.str();
}

void removeTemporaryFile(const std::string& tmpPath)
{
    namespace fs = std::filesystem;
    bool ret = false;
    try
    {
        ret = fs::remove(tmpPath);
    }
    catch (fs::filesystem_error& e)
    {
        Antares::logs.error() << e.what();
    }
    if (!ret)
    {
        Antares::logs.warning() << "Could not remove temporary file " << tmpPath;
    }
}

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver,
                                                   Antares::Solver::IResultWriter& writer,
                                                   const std::string& filename)
{
    // 0. Logging file name
    Antares::logs.info() << "Solver OR-Tools MPS File: `" << filename << "'";

    // 1. Determine filename
    const auto tmpPath = generateTempPath(filename);

    // 2. Write MPS to temporary file
    solver->Write(tmpPath);

    // 3. Copy to real output using generic writer
    writer.addEntryFromFile(filename, tmpPath);

    // 4. Remove tmp file
    removeTemporaryFile(tmpPath);
}

bool solveAndManageStatus(MPSolver* solver, int& resultStatus, const MPSolverParameters& params)
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

MPSolver* ORTOOLS_Simplexe(Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                           MPSolver* solver,
                           bool keepBasis,
                           const OptimizationOptions& options)
{
    MPSolverParameters params;
    setGenericParameters(
      params);              // Keep generic params for default settings working for all solvers
    if (options.solverLogs) // May be overriden by log level if set as specific parameters
    {
        solver->EnableOutput();
    }
    TuneSolverSpecificOptions(solver, options.ortoolsSolver, options.solverParameters);
    const bool warmStart = solverSupportsWarmStart(solver->ProblemType());
    // Provide an initial simplex basis, if any
    if (warmStart && Probleme->basisExists())
    {
        Probleme->basisStatus.setStartingBasis(solver);
    }

    if (solveAndManageStatus(solver, Probleme->ExistenceDUneSolution, params))
    {
        extract_from_MPSolver(solver, Probleme);
        // Save the final simplex basis for next resolutions
        if (warmStart && keepBasis)
        {
            Probleme->basisStatus.extractBasis(solver);
        }
    }

    return solver;
}

void ORTOOLS_ModifierLeVecteurCouts(MPSolver* solver, const double* costs, int nbVar)
{
    auto& variables = solver->variables();
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        auto& var = variables[idxVar];
        solver->MutableObjective()->SetCoefficient(var, costs[idxVar]);
    }
}

void ORTOOLS_ModifierLeVecteurSecondMembre(MPSolver* solver,
                                           const double* rhs,
                                           const char* sens,
                                           int nbRow)
{
    auto& constraints = solver->constraints();
    for (int idxRow = 0; idxRow < nbRow; ++idxRow)
    {
        if (sens[idxRow] == '=')
        {
            constraints[idxRow]->SetBounds(rhs[idxRow], rhs[idxRow]);
        }
        else if (sens[idxRow] == '<')
        {
            constraints[idxRow]->SetBounds(-MPSolver::infinity(), rhs[idxRow]);
        }
        else if (sens[idxRow] == '>')
        {
            constraints[idxRow]->SetBounds(rhs[idxRow], MPSolver::infinity());
        }
    }
}

void ORTOOLS_CorrigerLesBornes(MPSolver* solver,
                               const double* bMin,
                               const double* bMax,
                               const int* typeVar,
                               int nbVar)
{
    auto& variables = solver->variables();
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        double min_l = ((typeVar[idxVar] == VARIABLE_NON_BORNEE)
                            || (typeVar[idxVar] == VARIABLE_BORNEE_SUPERIEUREMENT)
                          ? -MPSolver::infinity()
                          : bMin[idxVar]);
        double max_l = ((typeVar[idxVar] == VARIABLE_NON_BORNEE)
                            || (typeVar[idxVar] == VARIABLE_BORNEE_INFERIEUREMENT)
                          ? MPSolver::infinity()
                          : bMax[idxVar]);
        auto& var = variables[idxVar];
        var->SetBounds(min_l, max_l);
    }
}

void ORTOOLS_LibererProbleme(MPSolver* solver)
{
    delete solver;
}

const std::map<std::string, struct OrtoolsUtils::SolverNames> OrtoolsUtils::solverMap = {
  {"xpress", {"xpress_lp", "xpress"}},
  {"sirius", {"sirius_lp", "sirius"}},
  {"coin", {"clp", "cbc"}},
  {"glpk", {"glpk_lp", "glpk"}},
  {"scip", {"scip", "scip"}}};

std::list<std::string> getAvailableOrtoolsSolverName()
{
    std::list<std::string> result;

    for (const auto& solverName: OrtoolsUtils::solverMap)
    {
        MPSolver::OptimizationProblemType solverType;
        MPSolver::ParseSolverType(solverName.second.LPSolverName, &solverType);

        if (MPSolver::SupportsProblemType(solverType))
        {
            result.push_back(solverName.first);
        }
    }
    return result;
}

std::string availableOrToolsSolversString()
{
    const std::list<std::string> availableSolverList = getAvailableOrtoolsSolverName();
    std::ostringstream solvers;
    for (const std::string& avail: availableSolverList)
    {
        bool last = &avail == &availableSolverList.back();
        std::string sep = last ? "." : ", ";
        solvers << avail << sep;
    }
    return solvers.str();
}

static std::optional<std::string> translateSolverName(const std::string& solverName, bool isMip)
{
    try
    {
        if (isMip)
        {
            return OrtoolsUtils::solverMap.at(solverName).MIPSolverName;
        }
        else
        {
            return OrtoolsUtils::solverMap.at(solverName).LPSolverName;
        }
    }
    catch (const std::out_of_range&)
    {
        return {};
    }
}

MPSolver* MPSolverFactory(const bool isMip, const std::string& solverName)
{
    const std::string notFound = "Solver " + solverName + " not found";
    const std::invalid_argument except(notFound);

    auto internalSolverName = translateSolverName(solverName, isMip);
    if (!internalSolverName)
    {
        throw except;
    }

    MPSolver* solver = MPSolver::CreateSolver(*internalSolverName);
    if (!solver)
    {
        throw except;
    }

    return solver;
}
