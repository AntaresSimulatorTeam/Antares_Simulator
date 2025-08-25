/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#include <antares/solver/optimisation/LegacyFiller.h>

using namespace Antares::Optimisation::LinearProblemApi;

// Définitions des constantes manquantes pour les types de variables
namespace
{
constexpr int VARIABLE_BORNEE_DES_DEUX_COTES = 0;
constexpr int VARIABLE_BORNEE_INFERIEUREMENT = 1;
constexpr int VARIABLE_BORNEE_SUPERIEUREMENT = 2;
constexpr int VARIABLE_NON_BORNEE = 3;
} // namespace

namespace Antares::Optimization
{

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
LegacyFiller<SolverTagType>::LegacyFiller(const PROBLEME_HEBDO* problemeHebdo, bool namedProblems):
    problemeAResoudre_(problemeHebdo->ProblemeAResoudre.get()),
    useNamedProblems_(namedProblems)
{
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::addVariables(ILinearProblem<SolverTagType>& pb,
                                               ILinearProblemData&,
                                               FillContext&)
{
    // Create the variables and set objective cost.
    CopyVariables(pb);
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::addConstraints(ILinearProblem<SolverTagType>& pb,
                                                 ILinearProblemData&,
                                                 FillContext&)
{
    // Create constraints and set coefs
    CopyRows(pb);
    CopyMatrix(pb);
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::addObjective(ILinearProblem<SolverTagType>& pb,
                                               ILinearProblemData&,
                                               FillContext&)
{
    // nothing to do: objective coefficients are set along with variables definition
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::CopyMatrix(ILinearProblem<SolverTagType>& pb) const
{
    for (int idxRow = 0; idxRow < problemeAResoudre_->NombreDeContraintes; ++idxRow)
    {
        auto* ct = pb.getConstraint(idxRow);
        int debutLigne = problemeAResoudre_->IndicesDebutDeLigne[idxRow];
        for (int idxCoef = 0; idxCoef < problemeAResoudre_->NombreDeTermesDesLignes[idxRow];
             ++idxCoef)
        {
            int pos = debutLigne + idxCoef;
            auto* var = pb.getVariable(problemeAResoudre_->IndicesColonnes[pos]);
            ct->setCoefficient(var, problemeAResoudre_->CoefficientsDeLaMatriceDesContraintes[pos]);
        }
    }
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::CreateVariable(unsigned idxVar,
                                                 ILinearProblem<SolverTagType>& pb) const
{
    const double bMin = problemeAResoudre_->Xmin[idxVar];
    const double bMax = problemeAResoudre_->Xmax[idxVar];
    const int typeVar = problemeAResoudre_->TypeDeVariable[idxVar];

    double min_l = (typeVar == VARIABLE_NON_BORNEE || typeVar == VARIABLE_BORNEE_SUPERIEUREMENT)
                     ? -pb.infinity()
                     : bMin;
    double max_l = (typeVar == VARIABLE_NON_BORNEE || typeVar == VARIABLE_BORNEE_INFERIEUREMENT)
                     ? pb.infinity()
                     : bMax;
    const bool isIntegerVariable = problemeAResoudre_->VariablesEntieres[idxVar];

    auto* var = pb.addVariable(min_l, max_l, isIntegerVariable, GetVariableName(idxVar));
    pb.setObjectiveCoefficient(var, problemeAResoudre_->CoutLineaire[idxVar]);
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::CopyVariables(ILinearProblem<SolverTagType>& pb) const
{
    for (int idxVar = 0; idxVar < problemeAResoudre_->NombreDeVariables; ++idxVar)
    {
        CreateVariable(idxVar, pb);
    }
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::UpdateContraints(unsigned idxRow,
                                                   ILinearProblem<SolverTagType>& pb) const
{
    double bMin = -pb.infinity(), bMax = pb.infinity();
    switch (problemeAResoudre_->Sens[idxRow])
    {
    case '=':
        bMin = bMax = problemeAResoudre_->SecondMembre[idxRow];
        break;
    case '<':
        bMax = problemeAResoudre_->SecondMembre[idxRow];
        break;
    case '>':
        bMin = problemeAResoudre_->SecondMembre[idxRow];
        break;
    }

    pb.addConstraint(bMin, bMax, GetConstraintName(idxRow));
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
void LegacyFiller<SolverTagType>::CopyRows(ILinearProblem<SolverTagType>& pb) const
{
    for (int idxRow = 0; idxRow < problemeAResoudre_->NombreDeContraintes; ++idxRow)
    {
        UpdateContraints(idxRow, pb);
    }
}

// TODO: in the following code, we hide variable & constraint names from MPSolver only to
// workaround MPSolver.Write when we want lighter MPS. In the future (maybe through MathOpt), this
// shouldn't be done here

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
std::string LegacyFiller<SolverTagType>::GetVariableName(unsigned int index) const
{
    if (!useNamedProblems_ || problemeAResoudre_->NomDesVariables[index].empty())
    {
        return 'x' + std::to_string(index);
    }
    return problemeAResoudre_->NomDesVariables[index];
}

template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
std::string LegacyFiller<SolverTagType>::GetConstraintName(unsigned int index) const
{
    if (!useNamedProblems_ || problemeAResoudre_->NomDesContraintes[index].empty())
    {
        return 'c' + std::to_string(index);
    }
    return problemeAResoudre_->NomDesContraintes[index];
}
} // namespace Antares::Optimization
