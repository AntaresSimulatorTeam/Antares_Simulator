#include "ortools_utils.h"

#include <antares/logs/logs.h>
#include <antares/exception/AssertionError.hpp>
#include <antares/Enum.hpp>
#include <memory>
#include <filesystem>

using namespace operations_research;

const char* const XPRESS_PARAMS = "THREADS 1";

namespace Antares
{
namespace Optimization
{
ProblemSimplexeNommeConverter::ProblemSimplexeNommeConverter(
  const std::string& solverName,
  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe) :
 solverName_(solverName), problemeSimplexe_(problemeSimplexe)
{
    if (problemeSimplexe_->UseNamedProblems())
    {
        variableNameManager_.SetTarget(problemeSimplexe_->VariableNames());
        constraintNameManager_.SetTarget(problemeSimplexe_->ConstraintNames());
    }
}

bool Relaxation::visit(int idxVar) const
{
  return true;
}

bool Relaxation::isMIP() const
{
  return false;
}

bool MILP::visit(int idxVar) const
{
    return integerVariable[idxVar];
}

bool MILP::isMIP() const
{
    return true;
}

MILP::MILP(const std::vector<bool>& integerVariable) : integerVariable(integerVariable)
{
}

std::unique_ptr<IMixedIntegerProblemManager> IMixedIntegerProblemManager::factory(bool integer,
                                                                                  const std::vector<bool>& integerVariable)
{
  if (integer)
    return std::make_unique<MILP>(integerVariable);
  else
    return std::make_unique<Relaxation>();
}

MPSolver* ProblemSimplexeNommeConverter::Convert(const IMixedIntegerProblemManager& visitor)
{
    MPSolver* solver = MPSolverFactory(problemeSimplexe_, solverName_, visitor.isMIP());
    TuneSolverSpecificOptions(solver);

    // Create the variables and set objective cost.
    CopyVariables(solver, visitor);

    // Create constraints and set coefs
    CopyConstraints(solver);

    CopyMatrix(solver);

    return solver;
}

void ProblemSimplexeNommeConverter::TuneSolverSpecificOptions(MPSolver* solver) const
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

void ProblemSimplexeNommeConverter::CopyMatrix(const MPSolver* solver)
{
    auto variables = solver->variables();
    auto constraints = solver->constraints();

    for (int idxRow = 0; idxRow < problemeSimplexe_->NombreDeContraintes; ++idxRow)
    {
        MPConstraint* const ct = constraints[idxRow];
        int debutLigne = problemeSimplexe_->IndicesDebutDeLigne[idxRow];
        for (int idxCoef = 0; idxCoef < problemeSimplexe_->NombreDeTermesDesLignes[idxRow];
             ++idxCoef)
        {
            int pos = debutLigne + idxCoef;
            ct->SetCoefficient(variables[problemeSimplexe_->IndicesColonnes[pos]],
                               problemeSimplexe_->CoefficientsDeLaMatriceDesContraintes[pos]);
        }
    }
}

void ProblemSimplexeNommeConverter::CreateVariable(unsigned idxVar,
                                                   MPSolver* solver,
                                                   MPObjective* const objective,
                                                   bool integerVar)
{
    double min_l = 0.0;
    if (problemeSimplexe_->Xmin != NULL) // TODO[FOM] Remove enclosing if ?
    {
        min_l = problemeSimplexe_->Xmin[idxVar];
    }
    double max_l = problemeSimplexe_->Xmax[idxVar];
    const MPVariable* var = solver->MakeVar(min_l, max_l, integerVar, variableNameManager_.GetName(idxVar));
    objective->SetCoefficient(var, problemeSimplexe_->CoutLineaire[idxVar]);
}

void ProblemSimplexeNommeConverter::CopyVariables(MPSolver* solver,
                                                  const IMixedIntegerProblemManager& visitor)

{
    MPObjective* const objective = solver->MutableObjective();
    for (int idxVar = 0; idxVar < problemeSimplexe_->NombreDeVariables; ++idxVar)
    {
        CreateVariable(idxVar, solver, objective, visitor.visit(idxVar));
    }
}

void ProblemSimplexeNommeConverter::UpdateContraints(unsigned idxRow, MPSolver* solver)
{
    double bMin = -MPSolver::infinity(), bMax = MPSolver::infinity();
    if (problemeSimplexe_->Sens[idxRow] == '=')
    {
        bMin = bMax = problemeSimplexe_->SecondMembre[idxRow];
    }
    else if (problemeSimplexe_->Sens[idxRow] == '<')
    {
        bMax = problemeSimplexe_->SecondMembre[idxRow];
    }
    else if (problemeSimplexe_->Sens[idxRow] == '>')
    {
        bMin = problemeSimplexe_->SecondMembre[idxRow];
    }

    solver->MakeRowConstraint(bMin, bMax, constraintNameManager_.GetName(idxRow));
}

void ProblemSimplexeNommeConverter::CopyConstraints(MPSolver* solver)
{
    for (int idxRow = 0; idxRow < problemeSimplexe_->NombreDeContraintes; ++idxRow)
    {
        UpdateContraints(idxRow, solver);
    }
}

} // namespace Optimization
} // namespace Antares

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
                          const std::string& solverName,
                          bool MIP)
{
    MPSolver* solver;
    try
    {
        if (MIP)
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
