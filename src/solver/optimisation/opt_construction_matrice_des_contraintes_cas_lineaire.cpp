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
#include "opt_structure_probleme_a_resoudre.h"
#include "opt_export_structure.h"
#include "../utils/filename.h"
#include "opt_fonctions.h"
#include "opt_rename_problem.h"
#include "sim_structure_probleme_economique.h"
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

#include <antares/study.h>

using namespace Antares::Data;

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo, Solver::IResultWriter& writer)
{
    int var;

    const bool exportStructure = problemeHebdo->ExportStructure;
    const bool firstWeekOfSimulation = problemeHebdo->firstWeekOfSimulation;

    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    int nombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    std::vector<double>& Pi = ProblemeAResoudre->Pi;
    std::vector<int>& Colonne = ProblemeAResoudre->Colonne;

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    ConstraintNamer constraintNamer(ProblemeAResoudre->NomDesContraintes,
                                    problemeHebdo->NamedProblems);

    BindingConstraintDay bindingConstraintDay(problemeHebdo);
    BindingConstraintWeek bindingConstraintWeek(problemeHebdo);
    HydroPower hydroPower(problemeHebdo);
    HydroPowerSmoothingUsingVariationSum hydroPowerSmoothingUsingVariationSum(problemeHebdo);
    HydroPowerSmoothingUsingVariationMaxDown hydroPowerSmoothingUsingVariationMaxDown(
      problemeHebdo);
    HydroPowerSmoothingUsingVariationMaxUp hydroPowerSmoothingUsingVariationMaxUp(problemeHebdo);

    MinHydroPower minHydroPower(problemeHebdo);
    MaxHydroPower maxHydroPower(problemeHebdo);

    MaxPumping maxPumping(problemeHebdo);

    AreaHydroLevel areaHydroLevel(problemeHebdo);

    FinalStockEquivalent finalStockEquivalent(problemeHebdo);
    FinalStockExpression finalStockExpression(problemeHebdo);

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);
        CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
            = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        AreaBalance areaBalance(problemeHebdo);
        FictitiousLoad fictitiousLoad(problemeHebdo);
        ShortTermStorageLevel shortTermStorageLevel(problemeHebdo);
        FlowDissociation flowDissociation(problemeHebdo);
        BindingConstraintHour bindingConstraintHour(problemeHebdo);

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            areaBalance.add(pdt, pays);

            fictitiousLoad.add(pdt, pays);

            shortTermStorageLevel.add(pdt, pays);
        }

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            flowDissociation.add(pdt, interco);
        }
        for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintHour.add(pdt, cntCouplante);
        }
    }

    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        bindingConstraintDay.add(cntCouplante);
    }

    if (nombreDePasDeTempsPourUneOptimisation > nombreDePasDeTempsDUneJournee)
    {
        int semaine = problemeHebdo->weekInTheYear;
        CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES& CorrespondanceCntNativesCntOptimHebdomadaires
          = problemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires;
        for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintWeek.add(cntCouplante);
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        hydroPower.add(pays);
    }

    if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            hydroPowerSmoothingUsingVariationSum.add(pays);
        }
    }
    else if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            constraintNamer.UpdateArea(problemeHebdo->NomsDesPays[pays]);
            for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(pays, pdt);
                hydroPowerSmoothingUsingVariationMaxUp.add(pays, pdt);
            }
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        minHydroPower.add(pays);

        maxHydroPower.add(pays);
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        maxPumping.add(pays);
    }

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        const auto& CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
            = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            areaHydroLevel.add(pays, pdt);
        }
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        finalStockEquivalent.add(pays);

        finalStockExpression.add(pays);
    }

    if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    {
        OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
                                                                                false);
    }

    // Export structure
    if (exportStructure && firstWeekOfSimulation)
    {
        OPT_ExportInterco(writer, problemeHebdo);
        OPT_ExportAreaName(writer, problemeHebdo->NomsDesPays);
    }

    return;
}
