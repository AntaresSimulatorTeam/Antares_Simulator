#include "ortools_utils.h"
#include "filename.h" // getFilenameWithExtension

#include <antares/logs.h>
#include <antares/study.h>
#include <antares/exception/AssertionError.hpp>
#include <antares/Enum.hpp>
#include <filesystem>

using namespace operations_research;

const char* const XPRESS_PARAMS = "THREADS 1 SCALING 0";

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

namespace Antares
{
namespace Optimization
{
MPSolver* convert_to_MPSolver(
  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    auto& study = *Data::Study::Current::Get();

    // Define solver used depending on study option
    MPSolver::OptimizationProblemType solverType
      = OrtoolsUtils().getLinearOptimProblemType(study.parameters.ortoolsEnumUsed);

    // Create the linear solver instance
    MPSolver* solver = new MPSolver("simple_lp_program", solverType);

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

static void change_MPSolver_objective(MPSolver* solver, const double* costs, int nbVar)
{
    auto& variables = solver->variables();
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        auto& var = variables[idxVar];
        solver->MutableObjective()->SetCoefficient(var, costs[idxVar]);
    }
}

static void change_MPSolver_rhs(const MPSolver* solver,
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

static std::string generateTempPath(const std::string& filename)
{
    namespace fs = std::filesystem;
    std::ostringstream tmpPath;
    tmpPath << fs::temp_directory_path().string() << Yuni::IO::SeparatorAsString << filename;
    return tmpPath.str();
}

static void removeTemporaryFile(const std::string& tmpPath)
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
                                                   size_t numSpace,
                                                   int const numOptim,
                                                   Antares::Solver::IResultWriter::Ptr writer)
{
    // 1. Determine filename
    const auto filename = getFilenameWithExtension("problem", "mps", numSpace, numOptim);
    const auto tmpPath = generateTempPath(filename);

    // 2. Write MPS to temporary file
    solver->Write(tmpPath);

    // 3. Copy to real output using generic writer
    writer->addEntryFromFile(filename, tmpPath);

    // 4. Remove tmp file
    removeTemporaryFile(tmpPath);
}

bool solveAndManageStatus(MPSolver* solver, int& resultStatus, MPSolverParameters& params)
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

MPSolver* ORTOOLS_ConvertIfNeeded(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                                  MPSolver* solver)
{
    if (solver == nullptr)
    {
        return Antares::Optimization::convert_to_MPSolver(Probleme);
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
    // Provide an initial simplex basis, if any
    if (Probleme->basisExists() && !Probleme->isMIP())
    {
        solver->SetStartingLpBasisInt(Probleme->StatutDesVariables, Probleme->StatutDesContraintes);
    }

    if (solveAndManageStatus(solver, Probleme->ExistenceDUneSolution, params))
    {
        extract_from_MPSolver(solver, Probleme);
        // Save the final simplex basis for next resolutions
        if (keepBasis && !Probleme->isMIP())
        {
            solver->GetFinalLpBasisInt(Probleme->StatutDesVariables, Probleme->StatutDesContraintes);
        }
    }

    return solver;
}

void ORTOOLS_ModifierLeVecteurCouts(MPSolver* solver, const double* costs, int nbVar)
{
    change_MPSolver_objective(solver, costs, nbVar);
}

void ORTOOLS_ModifierLeVecteurSecondMembre(MPSolver* solver,
                                           const double* rhs,
                                           const char* sens,
                                           int nbRow)
{
    change_MPSolver_rhs(solver, rhs, sens, nbRow);
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

using namespace Antares::Data;

OrtoolsUtils::OrtoolsUtils()
{
    // TODO JMK : Values must be adequacy with kOptimizationProblemTypeNames for ortools
    // linear_solver/linear_solver.cc file

    _solverLinearProblemOptimStringMap[OrtoolsSolver::sirius]
      = "sirius_lp"; // TODO JMK : not defined in current ortools RTE branch.
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::sirius]
      = "sirius_mip"; // TODO JMK : not defined in current ortools RTE branch.

    _solverLinearProblemOptimStringMap[OrtoolsSolver::coin] = "clp";
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::coin] = "cbc";

    _solverLinearProblemOptimStringMap[OrtoolsSolver::xpress] = "xpress_lp";
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::xpress] = "xpress";

    _solverLinearProblemOptimStringMap[OrtoolsSolver::glop_scip] = "glop";
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::glop_scip] = "scip";

    _solverLinearProblemOptimStringMap[OrtoolsSolver::cplex]
      = "cplex_lp"; // TODO JMK : not defined in current ortools RTE branch.
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::cplex]
      = "cplex_mip"; // TODO JMK : not defined in current ortools RTE branch.

    _solverLinearProblemOptimStringMap[OrtoolsSolver::gurobi] = "gurobi_lp";
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::gurobi] = "gurobi_mip";

    _solverLinearProblemOptimStringMap[OrtoolsSolver::glpk] = "glpk_lp";
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::glpk] = "glpk_mip";

    _solverLinearProblemOptimStringMap[OrtoolsSolver::glop_cbc] = "glop";
    _solverMixedIntegerProblemOptimStringMap[OrtoolsSolver::glop_cbc] = "cbc";

    /* TODO JMK : see how we can get optimization problem type with current ortools RTE branch
    (can't use enum because of compile switch)

    _solverLinearProblemOptimMap[OrtoolsSolver::sirius]       =
    MPSolver::OptimizationProblemType::SIRIUS_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::sirius] =
    MPSolver::OptimizationProblemType::SIRIUS_MIXED_INTEGER_PROGRAMMING;

    _solverLinearProblemOptimMap[OrtoolsSolver::coin]       =
    MPSolver::OptimizationProblemType::CLP_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::coin] =
    MPSolver::OptimizationProblemType::CBC_MIXED_INTEGER_PROGRAMMING;

    _solverLinearProblemOptimMap[OrtoolsSolver::xpress]       =
    MPSolver::OptimizationProblemType::XPRESS_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::xpress] =
    MPSolver::OptimizationProblemType::XPRESS_MIXED_INTEGER_PROGRAMMING;

    _solverLinearProblemOptimMap[OrtoolsSolver::glop_scip]       =
    MPSolver::OptimizationProblemType::GLOP_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::glop_scip] =
    MPSolver::OptimizationProblemType::SCIP_MIXED_INTEGER_PROGRAMMING;

    _solverLinearProblemOptimMap[OrtoolsSolver::cplex]       =
    MPSolver::OptimizationProblemType::CPLEX_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::cplex] =
    MPSolver::OptimizationProblemType::CPLEX_MIXED_INTEGER_PROGRAMMING;

    _solverLinearProblemOptimMap[OrtoolsSolver::gurobi]       =
    MPSolver::OptimizationProblemType::GUROBI_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::gurobi] =
    MPSolver::OptimizationProblemType::GUROBI_MIXED_INTEGER_PROGRAMMING;

    _solverLinearProblemOptimMap[OrtoolsSolver::glpk]       =
    MPSolver::OptimizationProblemType::GLPK_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::glpk] =
    MPSolver::OptimizationProblemType::GLPK_MIXED_INTEGER_PROGRAMMING;

    _solverLinearProblemOptimMap[OrtoolsSolver::glop_cbc]       =
    MPSolver::OptimizationProblemType::GLOP_LINEAR_PROGRAMMING;
    _solverMixedIntegerProblemOptimMap[OrtoolsSolver::glop_cbc] =
    MPSolver::OptimizationProblemType::CBC_MIXED_INTEGER_PROGRAMMING;

            */
}

bool OrtoolsUtils::isOrtoolsSolverAvailable(OrtoolsSolver ortoolsSolver)
{
    bool result = false;

    // GLOP solver fail for too many examples. For now support is disabled
    if (ortoolsSolver == OrtoolsSolver::glop_scip || ortoolsSolver == OrtoolsSolver::glop_cbc)
    {
        return false;
    }

    try
    {
        result = MPSolver::SupportsProblemType(getLinearOptimProblemType(ortoolsSolver));
        result &= MPSolver::SupportsProblemType(getMixedIntegerOptimProblemType(ortoolsSolver));
    }
    catch (AssertionError& ex)
    {
        result = false;
    }

    return result;
}

std::list<OrtoolsSolver> OrtoolsUtils::getAvailableOrtoolsSolver()
{
    std::list<OrtoolsSolver> result;

    std::list<OrtoolsSolver> solverList = Enum::enumList<Antares::Data::OrtoolsSolver>();

    for (OrtoolsSolver solver : solverList)
    {
        if (isOrtoolsSolverAvailable(solver))
        {
            result.push_back(solver);
        }
    }

    return result;
}

std::list<std::string> OrtoolsUtils::getAvailableOrtoolsSolverName()
{
    std::list<std::string> result;

    std::list<OrtoolsSolver> solverList = getAvailableOrtoolsSolver();

    for (OrtoolsSolver solver : solverList)
    {
        result.push_back(Enum::toString(solver));
    }

    return result;
}

MPSolver::OptimizationProblemType OrtoolsUtils::getLinearOptimProblemType(
  const OrtoolsSolver& ortoolsSolver)
{
    // TODO JMK : see how we can get optimization problem type with current ortools RTE branch
    // (can't use enum because of compile switch) MPSolver::OptimizationProblemType result =
    // _solverLinearProblemOptimMap.at(ortoolsSolver)

    // For now we are using string to get optimization problem type
    MPSolver::OptimizationProblemType result
      = MPSolver::OptimizationProblemType::GLOP_LINEAR_PROGRAMMING;

    if (!MPSolver::ParseSolverType(_solverLinearProblemOptimStringMap.at(ortoolsSolver), &result))
    {
        throw AssertionError("Unsupported Linear OrtoolsSolver for solver "
                             + Enum::toString(ortoolsSolver));
    }

    return result;
}

MPSolver::OptimizationProblemType OrtoolsUtils::getMixedIntegerOptimProblemType(
  const OrtoolsSolver& ortoolsSolver)
{
    // TODO JMK : see how we can get optimization problem type with current ortools RTE branch
    // (can't use enum because of compile switch) MPSolver::OptimizationProblemType result =
    // _solverMixedIntegerProblemOptimMap.at(ortoolsSolver)

    // For now we are using string to get optimization problem type
    MPSolver::OptimizationProblemType result
      = MPSolver::OptimizationProblemType::GLOP_LINEAR_PROGRAMMING;

    if (!MPSolver::ParseSolverType(_solverMixedIntegerProblemOptimStringMap[ortoolsSolver],
                                   &result))
    {
        throw AssertionError("Unsupported Mixed Integer OrtoolsSolver for solver "
                             + Enum::toString(ortoolsSolver));
    }

    return result;
}
