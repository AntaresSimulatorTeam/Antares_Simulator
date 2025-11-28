/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
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
#include <filesystem>
#include <optional>
#include <ortools/math_opt/cpp/parameters.h>
#include <spx_constantes_externes.h>

#include <boost/algorithm/string/join.hpp>

#include <antares/enums/Enum.hpp>
#include <antares/exception/LoadingError.hpp>
#include <antares/logs/logs.h>
#include <antares/solver/simulation/sim_structure_probleme_economique.h>
#include <antares/solver/utils/ortools_utils.h>

using namespace operations_research;

const std::string XPRESS_PARAMS = "THREADS 1";
const std::string SCIP_PARAMS = "parallel/maxnthreads 1";
const std::string GUROBI_PARAMS
  = "Threads 1"; // See
                 // https://docs.gurobi.com/projects/optimizer/en/current/reference/parameters.html#threads
                 // for Gurobi threads (and other) parameter reference

using Antares::Solver::Optimization::SingleOptimOptions;

// TODO use Objective().Value() instead
// This is a temporary workaround for Windows
double getObjectiveValue(const MPSolver* solver)
{
    double ret = 0;
    const auto& objective = solver->Objective();
    for (const auto* variable: solver->variables())
    {
        ret += variable->solution_value() * objective.GetCoefficient(variable);
    }
    ret += objective.offset();
    return ret;
}

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
        throw Error::InvalidSolverSpecificParameters(solverName, specificParameters);
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

    const bool isMIP = problemeAResoudre->isMIP();

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
        logs.error() << e.what();
    }
    if (!ret)
    {
        logs.warning() << "Could not remove temporary file " << tmpPath;
    }
}

void ORTOOLS_EcrireJeuDeDonneesLineaireAuFormatMPS(MPSolver* solver,
                                                   Antares::Solver::IResultWriter& writer,
                                                   const std::string& filename)
{
    // 0. Logging file name
    logs.info() << "Solver OR-Tools MPS File: `" << filename << "'";

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

const std::map<std::string, struct OrtoolsUtils::SolverNames> OrtoolsUtils::mpSolverMap = {
  {"xpress", {"xpress_lp", "xpress"}},
  {"sirius", {"sirius_lp", std::nullopt}}, // only allowed in LP (MIP only supports binaries)
  {"coin", {"clp", "cbc"}},
  {"glpk", {"glpk_lp", "glpk"}},
  {"scip", {std::nullopt, "scip"}}, // SCIP only supports MIPs
  {"highs", {"highs_lp", "highs"}},
  {"pdlp", {"pdlp", std::nullopt}}, // PDLP only supports LPs
  {"gurobi", {"gurobi_lp", "gurobi"}}};

const std::map<std::string, math_opt::SolverType> OrtoolsUtils::mathoptSolverMap = {
  {"pdlp", math_opt::SolverType::kPdlp},
  {"scip", math_opt::SolverType::kGscip},
  {"xpress", math_opt::SolverType::kXpress},
  {"gurobi", math_opt::SolverType::kGurobi}};

std::list<std::string> availableLinearSolversList()
{
    std::list<std::string> result;

    for (const auto& solverName: OrtoolsUtils::mpSolverMap)
    {
        MPSolver::OptimizationProblemType solverType;
        if (solverName.second.LPSolverName.has_value())
        {
            MPSolver::ParseSolverType(solverName.second.LPSolverName.value(), &solverType);
        }
        else
        {
            MPSolver::ParseSolverType(solverName.second.MIPSolverName.value(), &solverType);
        }

        if (MPSolver::SupportsProblemType(solverType))
        {
            result.push_back(solverName.first);
        }
    }
    return result;
}

bool isLinearSolverAvailable(const std::string& solverName)
{
    auto it = OrtoolsUtils::mpSolverMap.find(solverName);
    if (it == OrtoolsUtils::mpSolverMap.end())
    {
        return false;
    }

    MPSolver::OptimizationProblemType solverType;
    if (it->second.LPSolverName.has_value())
    {
        MPSolver::ParseSolverType(it->second.LPSolverName.value(), &solverType);
    }
    else
    {
        MPSolver::ParseSolverType(it->second.MIPSolverName.value(), &solverType);
    }

    return MPSolver::SupportsProblemType(solverType);
}

std::list<std::string> availableQuadraticSolversList()
{
    std::list<std::string> result;
    // Sirius is supported, but not through mathopt
    result.push_back("sirius");
    for (const auto& solverName: OrtoolsUtils::mathoptSolverMap)
    {
        result.push_back(solverName.first);
    }
    return result;
}

std::string toString(const std::list<std::string>& solverList)
{
    return boost::algorithm::join(solverList, ",") + ".";
}

static std::optional<std::string> translateSolverName(const std::string& solverName, bool isMip)
{
    if (!OrtoolsUtils::mpSolverMap.contains(solverName))
    {
        return {};
    }
    auto names = OrtoolsUtils::mpSolverMap.at(solverName);
    if (isMip)
    {
        return names.MIPSolverName;
    }
    else
    {
        return names.LPSolverName;
    }
}

MPSolver* MPSolverFactory(const bool isMip, const std::string& solverName)
{
    const std::string notFound = "Solver " + solverName
                                 + " not supported for linear problems optimization.";
    const std::invalid_argument except(notFound);

    auto internalSolverName = translateSolverName(solverName, isMip);
    if (!internalSolverName.has_value())
    {
        throw std::invalid_argument("Solver " + solverName
                                    + " is not supported by Antares or does not support "
                                    + (isMip ? "MIP" : "LP") + " problems.");
    }

    MPSolver* solver = MPSolver::CreateSolver(*internalSolverName);
    if (!solver)
    {
        throw std::invalid_argument("Solver " + solverName + " (" + *internalSolverName
                                    + ") could not be loaded by OR-Tools MPSolver.");
    }

    return solver;
}
