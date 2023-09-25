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
#include "constraints/AreaBalance.h"
#include "constraints/FictitiousLoad.h"
#include "constraints/ShortTermStorageLevel.h"
#include "constraints/FlowDissociation.h"
#include "constraints/BindingConstraintHour.h"
#include "constraints/BindingConstraintDay.h"
#include "constraints/BindingConstraintWeek.h"
#include "constraints/HydroPower.h"
#include "constraints/HydroPowerSmoothingUsingVariationSum.h"
#include "constraints/HydroPowerSmoothingUsingVariationMaxDown.h"
#include "constraints/HydroPowerSmoothingUsingVariationMaxUp.h"
#include "constraints/MinHydroPower.h"
#include "constraints/MaxHydroPower.h"
#include "constraints/MaxPumping.h"
#include "constraints/AreaHydroLevel.h"
#include "constraints/FinalStockEquivalent.h"
#include "constraints/FinalStockExpression.h"

#include <antares/study/study.h>

using namespace Antares::Data;

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo, Solver::IResultWriter& writer)
{
    PROBLEME_ANTARES_A_RESOUDRE* ProblemeAResoudre = problemeHebdo->ProblemeAResoudre.get();

    int nombreDePasDeTempsDUneJournee = problemeHebdo->NombreDePasDeTempsDUneJournee;
    int nombreDePasDeTempsPourUneOptimisation
      = problemeHebdo->NombreDePasDeTempsPourUneOptimisation;

    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;
    ConstraintNamer constraintNamer(ProblemeAResoudre->NomDesContraintes);

    AreaBalance areaBalance(problemeHebdo);
    FictitiousLoad fictitiousLoad(problemeHebdo);
    ShortTermStorageLevel shortTermStorageLevel(problemeHebdo);
    FlowDissociation flowDissociation(problemeHebdo);
    BindingConstraintHour bindingConstraintHour(problemeHebdo);
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
    if (problemeHebdo->ExportStructure && problemeHebdo->firstWeekOfSimulation)
    {
        OPT_ExportInterco(writer, problemeHebdo);
        OPT_ExportAreaName(writer, problemeHebdo->NomsDesPays);
    }

    return;
}
