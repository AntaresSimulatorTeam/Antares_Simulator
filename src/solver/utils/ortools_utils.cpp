#include "ortools_utils.h"

#include <antares/logs.h>
#include <antares/exception/AssertionError.hpp>
#include <antares/Enum.hpp>
#include <antares/emergency.h>
#include <filesystem>

using namespace operations_research;

const char* const XPRESS_PARAMS = "THREADS 1";
enum class MATRIX_ELEMENT_TYPE
{
    VARIABLE,
    CONSTRAINT
};

char FromMatrixElementTypeToChar(MATRIX_ELEMENT_TYPE type)
{
    if (type == MATRIX_ELEMENT_TYPE::VARIABLE)
    {
        return 'x';
    }
    return 'c';
}

void CheckName(std::string& var_name, unsigned idxVar, MATRIX_ELEMENT_TYPE type)
{
    if (var_name.empty())
    {
        var_name = FromMatrixElementTypeToChar(type) + std::to_string(idxVar);
    }
}

static void UpdateCoefficients(MPSolver* solver,
                               MPObjective* const objective,
                               const double* bMin,
                               const double* bMax,
                               const double* costs,
                               unsigned idxVar,
                               std::string& varName,
                               MATRIX_ELEMENT_TYPE type)
{
    double min_l = 0.0;
    if (bMin != NULL)
    {
        min_l = bMin[idxVar];
    }
    double max_l = bMax[idxVar];
    CheckName(varName, idxVar, type);
    const MPVariable* var = solver->MakeNumVar(min_l, max_l, varName);
    objective->SetCoefficient(var, costs[idxVar]);
}
static void transferVariables(MPSolver* solver,
                              const double* bMin,
                              const double* bMax,
                              const double* costs,
                              int nbVar)
{
    MPObjective* const objective = solver->MutableObjective();
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        std::string varName;
        UpdateCoefficients(
          solver, objective, bMin, bMax, costs, idxVar, varName, MATRIX_ELEMENT_TYPE::VARIABLE);
    }
}
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
        std::string varName = NomDesVariables[idxVar];
        UpdateCoefficients(
          solver, objective, bMin, bMax, costs, idxVar, varName, MATRIX_ELEMENT_TYPE::VARIABLE);
    }
}

static void UpdateContraints(MPSolver* solver,
                             const double* rhs,
                             const char* sens,
                             unsigned idxRow,
                             std::string& constraintName)
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

    CheckName(constraintName, idxRow, MATRIX_ELEMENT_TYPE::CONSTRAINT);

    solver->MakeRowConstraint(bMin, bMax, constraintName);
}
static void transferRows(MPSolver* solver,
                         const double* rhs,
                         const char* sens,
                         int nbRow,
                         const std::vector<std::string>& NomDesContraintes)
{
    for (int idxRow = 0; idxRow < nbRow; ++idxRow)
    {
        std::string constraintName = NomDesContraintes[idxRow];
        UpdateContraints(solver, rhs, sens, idxRow, constraintName);
    }
}
static void transferRows(MPSolver* solver, const double* rhs, const char* sens, int nbRow)
{
    for (int idxRow = 0; idxRow < nbRow; ++idxRow)
    {
        std::string constraintName;
        UpdateContraints(solver, rhs, sens, idxRow, constraintName);
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
MPSolver* convert_to_MPSolver(const std::string& solverName,
                              const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    // Create the MPSolver
    MPSolver* solver = MPSolverFactory(problemeSimplexe, solverName);

    tuneSolverSpecificOptions(solver);
    if (problemeSimplexe->UseNamedProblems())
    {
        // Create the variables and set objective cost.
        transferVariables(solver,
                          problemeSimplexe->Xmin,
                          problemeSimplexe->Xmax,
                          problemeSimplexe->CoutLineaire,
                          problemeSimplexe->NombreDeVariables,
                          problemeSimplexe->VariableNames());

        // Create constraints and set coefs
        transferRows(solver,
                     problemeSimplexe->SecondMembre,
                     problemeSimplexe->Sens,
                     problemeSimplexe->NombreDeContraintes,
                     problemeSimplexe->ConstraintNames());
    }
    else
    {
        // Create the variables and set objective cost.
        transferVariables(solver,
                          problemeSimplexe->Xmin,
                          problemeSimplexe->Xmax,
                          problemeSimplexe->CoutLineaire,
                          problemeSimplexe->NombreDeVariables);

        // Create constraints and set coefs
        transferRows(solver,
                     problemeSimplexe->SecondMembre,
                     problemeSimplexe->Sens,
                     problemeSimplexe->NombreDeContraintes);
    }
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
        AntaresSolverEmergencyShutdown();
    }

    return solver;
}
