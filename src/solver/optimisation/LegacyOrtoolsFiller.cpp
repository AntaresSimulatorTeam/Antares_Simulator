#include "antares/solver/optimisation/LegacyOrtoolsFiller.h"

namespace Antares::Optimization
{

LegacyOrtoolsFiller::LegacyOrtoolsFiller(
  const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe):
    problemeSimplexe_(problemeSimplexe)
{
    if (problemeSimplexe_->UseNamedProblems())
    {
        variableNameManager_.SetTarget(problemeSimplexe_->VariableNames());
        constraintNameManager_.SetTarget(problemeSimplexe_->ConstraintNames());
    }
}

void LegacyOrtoolsFiller::addVariables(ILinearProblem& pb, LinearProblemData& data)
{
    auto* mpSolver = dynamic_cast<operations_research::MPSolver*>(&pb);
    if (mpSolver)
    {
        // Create the variables and set objective cost.
        CopyVariables(mpSolver);
    }
    else
    {
        logs.error() << "Invalid cast, ortools MPSolver expected.";
        throw std::bad_cast();
    }
}

void LegacyOrtoolsFiller::addConstraints(ILinearProblem& pb, LinearProblemData& data)
{
    auto* mpSolver = dynamic_cast<operations_research::MPSolver*>(&pb);
    if (mpSolver)
    {
        // Create constraints and set coefs
        CopyRows(mpSolver);
        CopyMatrix(mpSolver);
    }
    else
    {
        logs.error() << "Invalid cast, ortools MPSolver expected.";
        throw std::bad_cast();
    }
}

void LegacyOrtoolsFiller::addObjective(ILinearProblem& pb, LinearProblemData& data)
{
}

void LegacyOrtoolsFiller::CopyMatrix(const MPSolver* solver) const
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

void LegacyOrtoolsFiller::CreateVariable(unsigned idxVar,
                                         MPSolver* solver,
                                         MPObjective* const objective) const
{
    double min_l = problemeSimplexe_->Xmin[idxVar];
    double max_l = problemeSimplexe_->Xmax[idxVar];
    bool isIntegerVariable = problemeSimplexe_->IntegerVariable(idxVar);
    const MPVariable* var = solver->MakeVar(min_l,
                                            max_l,
                                            isIntegerVariable,
                                            variableNameManager_.GetName(idxVar));
    objective->SetCoefficient(var, problemeSimplexe_->CoutLineaire[idxVar]);
}

void LegacyOrtoolsFiller::CopyVariables(MPSolver* solver) const

{
    MPObjective* const objective = solver->MutableObjective();
    for (int idxVar = 0; idxVar < problemeSimplexe_->NombreDeVariables; ++idxVar)
    {
        CreateVariable(idxVar, solver, objective);
    }
}

void LegacyOrtoolsFiller::UpdateContraints(unsigned idxRow, MPSolver* solver) const
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

void LegacyOrtoolsFiller::CopyRows(MPSolver* solver) const
{
    for (int idxRow = 0; idxRow < problemeSimplexe_->NombreDeContraintes; ++idxRow)
    {
        UpdateContraints(idxRow, solver);
    }
}
} // namespace Antares::Optimization
