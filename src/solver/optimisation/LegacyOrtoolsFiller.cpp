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
    // Create the variables and set objective cost.
    CopyVariables(pb);
}

void LegacyOrtoolsFiller::addConstraints(ILinearProblem& pb, LinearProblemData& data)
{
    // Create constraints and set coefs
    CopyRows(pb);
    CopyMatrix(pb);
}

void LegacyOrtoolsFiller::addObjective(ILinearProblem& pb, LinearProblemData& data)
{
}

void LegacyOrtoolsFiller::CopyMatrix(ILinearProblem& pb) const
{
    for (int idxRow = 0; idxRow < problemeSimplexe_->NombreDeContraintes; ++idxRow)
    {
        auto* ct = pb.getConstraint(constraintNameManager_.GetName(idxRow));
        int debutLigne = problemeSimplexe_->IndicesDebutDeLigne[idxRow];
        for (int idxCoef = 0; idxCoef < problemeSimplexe_->NombreDeTermesDesLignes[idxRow];
             ++idxCoef)
        {
            int pos = debutLigne + idxCoef;
            auto* var = pb.getVariable(
              variableNameManager_.GetName(problemeSimplexe_->IndicesColonnes[pos]));
            ct->setCoefficient(var, problemeSimplexe_->CoefficientsDeLaMatriceDesContraintes[pos]);
        }
    }
}

void LegacyOrtoolsFiller::CreateVariable(unsigned idxVar, ILinearProblem& pb) const
{
    double min_l = problemeSimplexe_->Xmin[idxVar];
    double max_l = problemeSimplexe_->Xmax[idxVar];
    bool isIntegerVariable = problemeSimplexe_->IntegerVariable(idxVar);
    auto* var = pb.addVariable(min_l,
                               max_l,
                               isIntegerVariable,
                               variableNameManager_.GetName(idxVar));
    pb.setObjectiveCoefficient(var, problemeSimplexe_->CoutLineaire[idxVar]);
}

void LegacyOrtoolsFiller::CopyVariables(ILinearProblem& pb) const
{
    for (int idxVar = 0; idxVar < problemeSimplexe_->NombreDeVariables; ++idxVar)
    {
        CreateVariable(idxVar, pb);
    }
}

void LegacyOrtoolsFiller::UpdateContraints(unsigned idxRow, ILinearProblem& pb) const
{
    double bMin = -pb.infinity(), bMax = pb.infinity();
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

    pb.addConstraint(bMin, bMax, constraintNameManager_.GetName(idxRow));
}

void LegacyOrtoolsFiller::CopyRows(ILinearProblem& pb) const
{
    for (int idxRow = 0; idxRow < problemeSimplexe_->NombreDeContraintes; ++idxRow)
    {
        UpdateContraints(idxRow, pb);
    }
}
} // namespace Antares::Optimization
