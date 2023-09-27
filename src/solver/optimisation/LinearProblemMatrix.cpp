/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/

#include "LinearProblemMatrix.h"
#include "opt_export_structure.h"
#include "../utils/filename.h"
#include "opt_fonctions.h"
#include "opt_rename_problem.h"
#include "constraints/FinalStockEquivalent.h"
#include "constraints/FinalStockExpression.h"

using namespace Antares::Data;
void LinearProblemMatrix::Run()
{
    int var;

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo_->ProblemeAResoudre.get();

    int nombreDePasDeTempsDUneJournee = problemeHebdo_->NombreDePasDeTempsDUneJournee;
    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo_->NombreDePasDeTempsPourUneOptimisation;

    std::vector<double>& Pi = ProblemeAResoudre->Pi;
    std::vector<int>& Colonne = ProblemeAResoudre->Colonne;

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    ConstraintNamer constraintNamer(ProblemeAResoudre->NomDesContraintes,
                                    problemeHebdo_->NamedProblems);

    FinalStockEquivalent finalStockEquivalent(problemeHebdo_);
    FinalStockExpression finalStockExpression(problemeHebdo_);

    for (auto& group : constraintgroups_)
    {
        group->Build();
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        finalStockEquivalent.add(pays);

        finalStockExpression.add(pays);
    }

    if (problemeHebdo_->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo_,
                                                                                false);
    }

    // Export structure
    if (problemeHebdo_->ExportStructure && problemeHebdo_->firstWeekOfSimulation)
    {
        OPT_ExportInterco(writer_, problemeHebdo_);
        OPT_ExportAreaName(writer_, problemeHebdo_->NomsDesPays);
    }

    return;
}
