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

#include "LinearProblemMatrixBuilder.h"
#include "opt_export_structure.h"
#include "../utils/filename.h"
#include "opt_fonctions.h"
#include "opt_rename_problem.h"
#include "AreaBalance.h"
#include "FictitiousLoad.h"
#include "ShortTermStorageLevel.h"
#include "FlowDissociation.h"
#include "BindingConstraintHour.h"
#include "BindingConstraintDay.h"
#include "BindingConstraintWeek.h"
#include "HydroPower.h"
#include "HydroPowerSmoothingUsingVariationSum.h"
#include "HydroPowerSmoothingUsingVariationMaxDown.h"
#include "HydroPowerSmoothingUsingVariationMaxUp.h"
#include "MinHydroPower.h"
#include "MaxHydroPower.h"
#include "MaxPumping.h"
#include "AreaHydroLevel.h"
#include "FinalStockEquivalent.h"
#include "FinalStockExpression.h"
#include "new_constraint_builder_utils.h"
#include "Group1.h"

using namespace Antares::Data;
void LinearProblemMatrixBuilder::Run()
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

    BindingConstraintDay bindingConstraintDay(problemeHebdo_);
    BindingConstraintWeek bindingConstraintWeek(problemeHebdo_);
    HydroPower hydroPower(problemeHebdo_);
    HydroPowerSmoothingUsingVariationSum hydroPowerSmoothingUsingVariationSum(problemeHebdo_);
    HydroPowerSmoothingUsingVariationMaxDown hydroPowerSmoothingUsingVariationMaxDown(
      problemeHebdo_);
    HydroPowerSmoothingUsingVariationMaxUp hydroPowerSmoothingUsingVariationMaxUp(problemeHebdo_);

    MinHydroPower minHydroPower(problemeHebdo_);
    MaxHydroPower maxHydroPower(problemeHebdo_);

    MaxPumping maxPumping(problemeHebdo_);

    AreaHydroLevel areaHydroLevel(problemeHebdo_);

    FinalStockEquivalent finalStockEquivalent(problemeHebdo_);
    FinalStockExpression finalStockExpression(problemeHebdo_);

    for (auto& group : constraintgroups_)
    {
        group->Build();
    }

    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        bindingConstraintDay.add(cntCouplante);
    }

    if (nombreDePasDeTempsPourUneOptimisation > nombreDePasDeTempsDUneJournee)
    {
        CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo_->CorrespondanceCntNativesCntOptimHebdomadaires;
        for (uint32_t cntCouplante = 0;
             cntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintWeek.add(cntCouplante);
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        hydroPower.add(pays);
    }

    if (problemeHebdo_->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            if (!problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            hydroPowerSmoothingUsingVariationSum.add(pays);
        }
    }
    else if (problemeHebdo_->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            if (!problemeHebdo_->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            constraintNamer.UpdateArea(problemeHebdo_->NomsDesPays[pays]);
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(pays, pdt);
                hydroPowerSmoothingUsingVariationMaxUp.add(pays, pdt);
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        minHydroPower.add(pays);

        maxHydroPower.add(pays);
    }

    for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
    {
        maxPumping.add(pays);
    }

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        const auto& CorrespondanceVarNativesVarOptim
          = problemeHebdo_->CorrespondanceVarNativesVarOptim[pdt];
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
          = problemeHebdo_->CorrespondanceCntNativesCntOptim[pdt];

        int timeStepInYear = problemeHebdo_->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);
        for (uint32_t pays = 0; pays < problemeHebdo_->NombreDePays; pays++)
        {
            areaHydroLevel.add(pays, pdt);
        }
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
