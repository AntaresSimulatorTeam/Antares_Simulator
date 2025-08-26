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

#pragma once

#include <spx_constantes_externes.h>

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "opt_structure_probleme_a_resoudre.h"

namespace Antares::Optimization
{
template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
class LegacyFiller: public Optimisation::LinearProblemApi::LinearProblemFiller<SolverTagType>
{
public:
    LegacyFiller(const PROBLEME_HEBDO* problemeHebdo, bool namedProblems):
        problemeAResoudre_(problemeHebdo->ProblemeAResoudre.get()),
        useNamedProblems_(namedProblems)
    {
    }

    void addVariables(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData&,
                      Optimisation::LinearProblemApi::FillContext&)
    {
        // Create the variables and set objective cost.
        CopyVariables(pb);
    }

    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                        Optimisation::LinearProblemApi::ILinearProblemData&,
                        Optimisation::LinearProblemApi::FillContext&)
    {
        // Create constraints and set coefs
        CopyRows(pb);
        CopyMatrix(pb);
    }

    void addObjective(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData&,
                      Optimisation::LinearProblemApi::FillContext&)
    {
        // nothing to do: objective coefficients are set along with variables definition
    }

private:
    const PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre_;
    const bool useNamedProblems_;

    void CreateVariable(unsigned idxVar,
                        Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const
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

    void CopyVariables(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const
    {
        for (int idxVar = 0; idxVar < problemeAResoudre_->NombreDeVariables; ++idxVar)
        {
            CreateVariable(idxVar, pb);
        }
    }

    void UpdateContraints(unsigned idxRow,
                          Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const
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

    void CopyRows(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const
    {
        for (int idxRow = 0; idxRow < problemeAResoudre_->NombreDeContraintes; ++idxRow)
        {
            UpdateContraints(idxRow, pb);
        }
    }

    void CopyMatrix(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const
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
                ct->setCoefficient(var,
                                   problemeAResoudre_->CoefficientsDeLaMatriceDesContraintes[pos]);
            }
        }
    }

    std::string GetVariableName(unsigned int index) const
    {
        if (!useNamedProblems_ || problemeAResoudre_->NomDesVariables[index].empty())
        {
            return 'x' + std::to_string(index);
        }
        return problemeAResoudre_->NomDesVariables[index];
    }

    std::string GetConstraintName(unsigned int index) const
    {
        if (!useNamedProblems_ || problemeAResoudre_->NomDesContraintes[index].empty())
        {
            return 'c' + std::to_string(index);
        }
        return problemeAResoudre_->NomDesContraintes[index];
    }
};
} // namespace Antares::Optimization
