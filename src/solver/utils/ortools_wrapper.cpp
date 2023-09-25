#include <filesystem>
#include <yuni/io/file.h>
#include <antares/logs/logs.h>

#include "ortools_wrapper.h"
#include "ortools_utils.h"

using operations_research::MPSolver;

MPSolver* ORTOOLS_ConvertIfNeeded(const std::string& solverName,
                                  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* Probleme,
                                  const Antares::Optimization::IMixedIntegerProblemManager& mixedProblemManager,
                                  MPSolver* solver)
{
    if (solver == nullptr)
    {
        Antares::Optimization::ProblemSimplexeNommeConverter converter(solverName, Probleme);
        return converter.Convert(mixedProblemManager);
    }
    else
    {
        return solver;
    }
}

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

static bool solveAndManageStatus(MPSolver* solver, int& resultStatus, const MPSolverParameters& params)
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

static void extractSolutionValues(const std::vector<MPVariable*>& variables,
                                  Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    int nbVar = problemeSimplexe->NombreDeVariables;
    assert(nbVar == variables.size());
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        auto& var = variables[idxVar];
        problemeSimplexe->X[idxVar] = var->solution_value();
    }
}

static void extractReducedCosts(const std::vector<MPVariable*>& variables,
                                Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    int nbVar = problemeSimplexe->NombreDeVariables;
    assert(nbVar == variables.size());
    for (int idxVar = 0; idxVar < nbVar; ++idxVar)
    {
        auto& var = variables[idxVar];
        problemeSimplexe->CoutsReduits[idxVar] = var->reduced_cost();
    }
}

static void extractDualValues(const std::vector<MPConstraint*>& constraints,
                              Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
  int nbRows = problemeSimplexe->NombreDeContraintes;
  assert(nbRows == constraints.size());
  for (int idxRow = 0; idxRow < nbRows; ++idxRow)
  {
      auto& row = constraints[idxRow];
      problemeSimplexe->CoutsMarginauxDesContraintes[idxRow] = row->dual_value();
  }
}

// MIP status for MPSolver is immutable
// We need to re-build an instance of it
static MPSolver* solveRelaxation(const std::string& solverName,
                                 Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
  Antares::Optimization::Relaxation relaxation;
  MPSolver* solver = ORTOOLS_ConvertIfNeeded(solverName,
                                             problemeSimplexe,
                                             relaxation,
                                             nullptr);
    // TODO[FOM] Handle status
    auto status = solver->Solve();
    return solver;
}

static void extract_from_MPSolver(MPSolver* solver,
                                  Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe)
{
    assert(solver);
    assert(problemeSimplexe);

    extractSolutionValues(solver->variables(),
                          problemeSimplexe);

    MPSolver* solverMaybeMIP;
    if (problemeSimplexe->isMIP())
    {
        // TODO solver
        solverMaybeMIP = solveRelaxation("xpress",
                                         problemeSimplexe);
    }
    else
    {
        solverMaybeMIP = solver;
    }

    extractReducedCosts(solverMaybeMIP->variables(),
                        problemeSimplexe);

    extractDualValues(solverMaybeMIP->constraints(),
                      problemeSimplexe);
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

std::string generateTempPath(const std::string& filename)
{
    namespace fs = std::filesystem;
    std::ostringstream tmpPath;
    tmpPath << fs::temp_directory_path().string() << Yuni::IO::SeparatorAsString << filename;
    return tmpPath.str();
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
