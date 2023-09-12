#include "ortools_utils.h"

#include <antares/logs/logs.h>
#include <antares/exception/AssertionError.hpp>
#include <antares/Enum.hpp>
#include <filesystem>

using namespace operations_research;

const char* const XPRESS_PARAMS = "THREADS 1";

static void transferVariables(MPSolver* solver,
                              const double* bMin,
                              const double* bMax,
                              const double* costs,
                              int nbVar,
                              const std::vector<std::string>& NomDesVariables)
{
    MPObjective* const objective = solver->MutableObjective();
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        double min_l = 0.0;
        if (bMin != NULL)
        {
            min_l = bMin[idxVar];
        }
        double max_l = bMax[idxVar];
        std::string varName;
        if (NomDesVariables[idxVar].empty())
        {
            varName = "x" + std::to_string(idxVar);
        }
        else
        {
            varName = NomDesVariables[idxVar];
        }
        const MPVariable* var = solver->MakeNumVar(min_l, max_l, varName);
        objective->SetCoefficient(var, costs[idxVar]);
    }
}

static void transferRows(MPSolver* solver,
                         const double* rhs,
                         const char* sens,
                         int nbRow,
                         const std::vector<std::string>& NomDesContraintes)
{
    for (int idxRow = 0; idxRow < nbRow; ++idxRow)
    {
        double bMin = -MPSolver::infinity(), bMax = MPSolver::infinity();
        if (sens[idxRow] == '=')
        {
            bMin = bMax = rhs[idxRow];
        }
        else if (sens[idxRow] == '<')
        {
            bMax = rhs[idxRow];
        }
        else if (sens[idxRow] == '>')
        {
            bMin = rhs[idxRow];
        }

        std::string constraintName;
        if (NomDesContraintes[idxRow].empty())
        {
            constraintName = "c" + std::to_string(idxRow);
        }
        else
        {
            constraintName = NomDesContraintes[idxRow];
        }

        solver->MakeRowConstraint(bMin, bMax, constraintName);
    }
}

static void transferMatrix(const MPSolver* solver,
                           const int* indexRows,
                           const int* terms,
                           const int* indexCols,
                           const double* coeffs,
                           int nbRow)
{
    auto variables = solver->variables();
    auto constraints = solver->constraints();

    for (int idxRow = 0; idxRow < nbRow; ++idxRow)
    {
        MPConstraint* const ct = constraints[idxRow];
        int debutLigne = indexRows[idxRow];
        for (int idxCoef = 0; idxCoef < terms[idxRow]; ++idxCoef)
        {
            int pos = debutLigne + idxCoef;
            ct->SetCoefficient(variables[indexCols[pos]], coeffs[pos]);
        }
    }
}

// MPSolverParameters's copy constructor is private
static void setGenericParameters(MPSolverParameters& params)
{
    params.SetIntegerParam(MPSolverParameters::SCALING, 0);
    params.SetIntegerParam(MPSolverParameters::PRESOLVE, 0);
}

static void tuneSolverSpecificOptions(MPSolver* solver)
{
    if (!solver)
        return;

    switch (solver->ProblemType())
    {
    case MPSolver::XPRESS_LINEAR_PROGRAMMING:
    case MPSolver::XPRESS_MIXED_INTEGER_PROGRAMMING:
        solver->SetSolverSpecificParametersAsString(XPRESS_PARAMS);
        break;
    // Add solver-specific options here
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

namespace Antares
{
namespace Optimization
{
MPSolver* convert_to_MPSolver(
    const std::string& solverName,
    const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    // Create the MPSolver
    MPSolver* solver = MPSolverFactory(problemeSimplexe, solverName);

    tuneSolverSpecificOptions(solver);

    // Create the variables and set objective cost.
    transferVariables(solver,
                      problemeSimplexe->Xmin,
                      problemeSimplexe->Xmax,
                      problemeSimplexe->CoutLineaire,
                      problemeSimplexe->NombreDeVariables,
                      problemeSimplexe->NomDesVariables);

    // Create constraints and set coefs
    transferRows(solver,
                 problemeSimplexe->SecondMembre,
                 problemeSimplexe->Sens,
                 problemeSimplexe->NombreDeContraintes,
                 problemeSimplexe->NomDesContraintes);
    transferMatrix(solver,
                   problemeSimplexe->IndicesDebutDeLigne,
                   problemeSimplexe->NombreDeTermesDesLignes,
                   problemeSimplexe->IndicesColonnes,
                   problemeSimplexe->CoefficientsDeLaMatriceDesContraintes,
                   problemeSimplexe->NombreDeContraintes);

    return solver;
}
} // namespace Optimization
} // namespace Antares

static void extract_from_MPSolver(const MPSolver* solver,
                                  Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    auto& variables = solver->variables();
    int nbVar = problemeSimplexe->NombreDeVariables;

    // Extracting variable values and reduced costs
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        auto& var = variables[idxVar];
        problemeSimplexe->X[idxVar] = var->solution_value();
        problemeSimplexe->CoutsReduits[idxVar] = var->reduced_cost();
    }

    auto& constraints = solver->constraints();
    int nbRow = problemeSimplexe->NombreDeContraintes;
    for (int idxRow = 0; idxRow < nbRow; ++idxRow)
    {
        auto& row = constraints[idxRow];
        problemeSimplexe->CoutsMarginauxDesContraintes[idxRow] = row->dual_value();
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

MPSolver* ORTOOLS_ConvertIfNeeded(const std::string& solverName,
                                  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                                  MPSolver* solver)
{
    if (solver == nullptr)
    {
        return Antares::Optimization::convert_to_MPSolver(solverName, Probleme);
    }
    else
    {
        return solver;
    }
}

MPSolver* ORTOOLS_Simplexe(Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                           MPSolver* solver,
                           bool keepBasis)
{
    MPSolverParameters params;
    setGenericParameters(params);
    const bool warmStart = solverSupportsWarmStart(solver->ProblemType());
    // Provide an initial simplex basis, if any
    if (warmStart && Probleme->basisExists())
    {
        solver->SetStartingLpBasisInt(Probleme->StatutDesVariables, Probleme->StatutDesContraintes);
    }

    if (solveAndManageStatus(solver, Probleme->ExistenceDUneSolution, params))
    {
        extract_from_MPSolver(solver, Probleme);
        // Save the final simplex basis for next resolutions
        if (warmStart && keepBasis)
        {
            solver->GetFinalLpBasisInt(Probleme->StatutDesVariables,
                                       Probleme->StatutDesContraintes);
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
            constraints[idxRow]->SetBounds(rhs[idxRow], rhs[idxRow]);
        else if (sens[idxRow] == '<')
            constraints[idxRow]->SetBounds(-MPSolver::infinity(), rhs[idxRow]);
        else if (sens[idxRow] == '>')
            constraints[idxRow]->SetBounds(rhs[idxRow], MPSolver::infinity());
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

const std::map<std::string, struct OrtoolsUtils::SolverNames> OrtoolsUtils::solverMap
  = {{"xpress", {"xpress_lp", "xpress"}},
     {"sirius", {"sirius_lp", "sirius"}},
     {"coin", {"clp", "cbc"}},
     {"glpk", {"glpk_lp", "glpk"}}};

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

MPSolver* MPSolverFactory(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* probleme,
                          const std::string& solverName)
{
    MPSolver* solver;
    try
    {
        if (probleme->isMIP())
            solver = MPSolver::CreateSolver((OrtoolsUtils::solverMap.at(solverName)).MIPSolverName);
        else
            solver = MPSolver::CreateSolver((OrtoolsUtils::solverMap.at(solverName)).LPSolverName);

        if (!solver)
            throw Antares::Data::AssertionError("Solver not found: " + solverName);
    }
    catch (...)
    {
        Antares::logs.error() << "Solver creation failed";
        throw;
    }

    return solver;
}
