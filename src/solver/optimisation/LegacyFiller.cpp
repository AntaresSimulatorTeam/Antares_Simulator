#include "antares/solver/optimisation/LegacyFiller.h"

using namespace Antares::Solver::Modeler::Api;

namespace Antares::Optimization
{

LegacyFiller::LegacyFiller(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe):
    problemeSimplexe_(problemeSimplexe)
{
}

void LegacyFiller::addVariables(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx)
{
    // Create the variables and set objective cost.
    CopyVariables(pb);
}

void LegacyFiller::addConstraints(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx)
{
    // Create constraints and set coefs
    CopyRows(pb);
    CopyMatrix(pb);
}

void LegacyFiller::addObjective(ILinearProblem& pb, LinearProblemData& data, FillContext& ctx)
{
    // nothing to do: objective coefficients are set along with variables definition
}

void LegacyFiller::CopyMatrix(ILinearProblem& pb) const
{
    for (int idxRow = 0; idxRow < problemeSimplexe_->NombreDeContraintes; ++idxRow)
    {
        auto* ct = pb.getConstraint(GetConstraintName(idxRow));
        int debutLigne = problemeSimplexe_->IndicesDebutDeLigne[idxRow];
        for (int idxCoef = 0; idxCoef < problemeSimplexe_->NombreDeTermesDesLignes[idxRow];
             ++idxCoef)
        {
            int pos = debutLigne + idxCoef;
            auto* var = pb.getVariable(GetVariableName(problemeSimplexe_->IndicesColonnes[pos]));
            ct->setCoefficient(var, problemeSimplexe_->CoefficientsDeLaMatriceDesContraintes[pos]);
        }
    }
}

void LegacyFiller::CreateVariable(unsigned idxVar, ILinearProblem& pb) const
{
    const double bMin = problemeSimplexe_->Xmin[idxVar];
    const double bMax = problemeSimplexe_->Xmax[idxVar];
    const int typeVar = problemeSimplexe_->TypeDeVariable[idxVar];

    double min_l = (typeVar == VARIABLE_NON_BORNEE || typeVar == VARIABLE_BORNEE_SUPERIEUREMENT)
                     ? -pb.infinity()
                     : bMin;
    double max_l = (typeVar == VARIABLE_NON_BORNEE || typeVar == VARIABLE_BORNEE_INFERIEUREMENT)
                     ? pb.infinity()
                     : bMax;
    const bool isIntegerVariable = problemeSimplexe_->IntegerVariable(idxVar);

    auto* var = pb.addVariable(min_l, max_l, isIntegerVariable, GetVariableName(idxVar));
    pb.setObjectiveCoefficient(var, problemeSimplexe_->CoutLineaire[idxVar]);
}

void LegacyFiller::CopyVariables(ILinearProblem& pb) const
{
    for (int idxVar = 0; idxVar < problemeSimplexe_->NombreDeVariables; ++idxVar)
    {
        CreateVariable(idxVar, pb);
    }
}

void LegacyFiller::UpdateContraints(unsigned idxRow, ILinearProblem& pb) const
{
    double bMin = -pb.infinity(), bMax = pb.infinity();
    switch (problemeSimplexe_->Sens[idxRow])
    {
    case '=':
        bMin = bMax = problemeSimplexe_->SecondMembre[idxRow];
        break;
    case '<':
        bMax = problemeSimplexe_->SecondMembre[idxRow];
        break;
    case '>':
        bMin = problemeSimplexe_->SecondMembre[idxRow];
        break;
    }

    pb.addConstraint(bMin, bMax, GetConstraintName(idxRow));
}

void LegacyFiller::CopyRows(ILinearProblem& pb) const
{
    for (int idxRow = 0; idxRow < problemeSimplexe_->NombreDeContraintes; ++idxRow)
    {
        UpdateContraints(idxRow, pb);
    }
}

std::string LegacyFiller::GetVariableName(unsigned int index) const
{
    if (!problemeSimplexe_->UseNamedProblems()
        || problemeSimplexe_->VariableNames().at(index).empty())
    {
        return 'x' + std::to_string(index);
    }
    return problemeSimplexe_->VariableNames().at(index);
}

std::string LegacyFiller::GetConstraintName(unsigned int index) const
{
    if (!problemeSimplexe_->UseNamedProblems()
        || problemeSimplexe_->ConstraintNames().at(index).empty())
    {
        return 'c' + std::to_string(index);
    }
    return problemeSimplexe_->ConstraintNames().at(index);
}
} // namespace Antares::Optimization
