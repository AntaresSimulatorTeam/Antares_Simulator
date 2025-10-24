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

#include "antares/solver/optimisation/LegacyFiller.h"

#include <spx_constantes_externes.h>

using namespace Antares::Optimisation::LinearProblemApi;

namespace Antares::Optimization
{

LegacyFiller::LegacyFiller(Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                           const PROBLEME_HEBDO* problemeHebdo,
                           bool namedProblems):

    problemeAResoudre_(problemeHebdo->ProblemeAResoudre.get()),
    useNamedProblems_(namedProblems),
    linearProblem_(linearProblem)
{
}

void LegacyFiller::addVariables(const FillContext&)
{
    // Create the variables and set objective cost.
    CopyVariables();
}

void LegacyFiller::addConstraints(const FillContext&)
{
    // Create constraints and set coefs
    CopyRows();
    CopyMatrix();
}

void LegacyFiller::addObjectives(const FillContext&)
{
    // nothing to do: objective coefficients are set along with variables definition
}

void LegacyFiller::CopyMatrix() const
{
    for (int idxRow = 0; idxRow < problemeAResoudre_->NombreDeContraintes; ++idxRow)
    {
        auto* ct = linearProblem_.getConstraint(idxRow);
        int debutLigne = problemeAResoudre_->IndicesDebutDeLigne[idxRow];
        for (int idxCoef = 0; idxCoef < problemeAResoudre_->NombreDeTermesDesLignes[idxRow];
             ++idxCoef)
        {
            int pos = debutLigne + idxCoef;
            auto* var = linearProblem_.getVariable(problemeAResoudre_->IndicesColonnes[pos]);
            ct->setCoefficient(var, problemeAResoudre_->CoefficientsDeLaMatriceDesContraintes[pos]);
        }
    }
}

void LegacyFiller::CreateVariable(unsigned idxVar) const
{
    const double bMin = problemeAResoudre_->Xmin[idxVar];
    const double bMax = problemeAResoudre_->Xmax[idxVar];
    const int typeVar = problemeAResoudre_->TypeDeVariable[idxVar];

    double min_l = (typeVar == VARIABLE_NON_BORNEE || typeVar == VARIABLE_BORNEE_SUPERIEUREMENT)
                     ? -linearProblem_.infinity()
                     : bMin;
    double max_l = (typeVar == VARIABLE_NON_BORNEE || typeVar == VARIABLE_BORNEE_INFERIEUREMENT)
                     ? linearProblem_.infinity()
                     : bMax;
    const bool isIntegerVariable = problemeAResoudre_->VariablesEntieres[idxVar];

    auto* var = linearProblem_.addVariable(min_l,
                                           max_l,
                                           isIntegerVariable,
                                           GetVariableName(idxVar));
    linearProblem_.setObjectiveCoefficient(var, problemeAResoudre_->CoutLineaire[idxVar]);
}

void LegacyFiller::CopyVariables() const
{
    for (int idxVar = 0; idxVar < problemeAResoudre_->NombreDeVariables; ++idxVar)
    {
        CreateVariable(idxVar);
    }
}

void LegacyFiller::UpdateContraints(unsigned idxRow) const
{
    double bMin = -linearProblem_.infinity(), bMax = linearProblem_.infinity();
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

    linearProblem_.addConstraint(bMin, bMax, GetConstraintName(idxRow));
}

void LegacyFiller::CopyRows() const
{
    for (int idxRow = 0; idxRow < problemeAResoudre_->NombreDeContraintes; ++idxRow)
    {
        UpdateContraints(idxRow);
    }
}

// TODO: in the following code, we hide variable & constraint names from MPSolver only to
// workaround MPSolver.Write when we want lighter MPS. In the future (maybe through MathOpt), this
// shouldn't be done here

std::string LegacyFiller::GetVariableName(unsigned int index) const
{
    if (!useNamedProblems_ || problemeAResoudre_->NomDesVariables[index].empty())
    {
        return 'x' + std::to_string(index);
    }
    return problemeAResoudre_->NomDesVariables[index];
}

std::string LegacyFiller::GetConstraintName(unsigned int index) const
{
    if (!useNamedProblems_ || problemeAResoudre_->NomDesContraintes[index].empty())
    {
        return 'c' + std::to_string(index);
    }
    return problemeAResoudre_->NomDesContraintes[index];
}
} // namespace Antares::Optimization
