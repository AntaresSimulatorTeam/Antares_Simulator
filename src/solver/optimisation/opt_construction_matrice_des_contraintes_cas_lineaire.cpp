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

#include "../simulation/sim_structure_donnees.h"
#include "../simulation/sim_extern_variables_globales.h"

#include "opt_fonctions.h"
#include "opt_rename_problem.h"
#include "constraint_builder.h"
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

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo,
                                                             Antares::Solver::IResultWriter& writer)
{
    int weekFirstHour = problemeHebdo->weekInTheYear * 168;

    auto& ProblemeAResoudre = problemeHebdo->ProblemeAResoudre;

    std::vector<double>& SecondMembre = ProblemeAResoudre->SecondMembre;

    std::vector<double*>& AdresseOuPlacerLaValeurDesCoutsMarginaux
      = ProblemeAResoudre->AdresseOuPlacerLaValeurDesCoutsMarginaux;

    const std::vector<int>& NumeroDeJourDuPasDeTemps = problemeHebdo->NumeroDeJourDuPasDeTemps;

    // TODO reset selectively
    ProblemeAResoudre->NombreDeContraintes = 0;
    ProblemeAResoudre->NombreDeTermesDansLaMatriceDesContraintes = 0;

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

    for (int pdt = 0; pdt < problemeHebdo->NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            areaBalance.add(pdt, pays);
            fictitiousLoad.add(pdt, pays);
            shortTermStorageLevel.add(pdt, pays);
        }

        for (int interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            flowDissociation.add(pdt, interco);
        }

        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintHour.add(pdt, cntCouplante);
        }
    }

    for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        bindingConstraintDay.add(cntCouplante);
    }

    // Weekly binding constraints are only added if the opt period is a week
    // ignored otherwise
    if (problemeHebdo->NombreDePasDeTempsPourUneOptimisation
        > problemeHebdo->NombreDePasDeTempsDUneJournee)
    {
        for (int cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            bindingConstraintWeek.add(cntCouplante);
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        hydroPower.add(pays);
    }

    if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            hydroPowerSmoothingUsingVariationSum.add(pays);
        }
    }
    else if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            for (int pdt = 0; pdt < problemeHebdo->NombreDePasDeTempsPourUneOptimisation; pdt++)
            {
                hydroPowerSmoothingUsingVariationMaxDown.add(pays, pdt);
                hydroPowerSmoothingUsingVariationMaxUp.add(pays, pdt);
            }
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        minHydroPower.add(pays);
        maxHydroPower.add(pays);
    }

    // TODO after this
    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        maxPumping.add(pays);
    }

    for (int pdt = 0; pdt < problemeHebdo->NombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].SuiviNiveauHoraire)
                continue;

            areaHydroLevel.add(pays, pdt);
        }
    }

    for (int pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue
            && problemeHebdo->CaracteristiquesHydrauliques[pays].DirectLevelAccess)
        {
            finalStockEquivalent.add(pays);
        }
        if (problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue)
        {
            finalStockExpression.add(pays);
        }
    }

    // if (problemeHebdo->OptimisationAvecCoutsDeDemarrage)
    // {
    //     OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaireCoutsDeDemarrage(problemeHebdo,
    //                                                                             false);
    // }
}
