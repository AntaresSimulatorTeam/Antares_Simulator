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
static BindingConstraintData GetBindingConstraintDataFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo,
                                                                      int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

    return {MatriceDesContraintesCouplantes.TypeDeContrainteCouplante,
            MatriceDesContraintesCouplantes.NombreDInterconnexionsDansLaContrainteCouplante,
            MatriceDesContraintesCouplantes.NumeroDeLInterconnexion,
            MatriceDesContraintesCouplantes.PoidsDeLInterconnexion,
            MatriceDesContraintesCouplantes.OffsetTemporelSurLInterco,
            MatriceDesContraintesCouplantes.NombreDePaliersDispatchDansLaContrainteCouplante,
            MatriceDesContraintesCouplantes.PaysDuPalierDispatch,
            MatriceDesContraintesCouplantes.NumeroDuPalierDispatch,
            MatriceDesContraintesCouplantes.PoidsDuPalierDispatch,
            MatriceDesContraintesCouplantes.OffsetTemporelSurLePalierDispatch,
            MatriceDesContraintesCouplantes.SensDeLaContrainteCouplante,
            MatriceDesContraintesCouplantes.NomDeLaContrainteCouplante};
}

static BindingConstraintHourData GetBindingConstraintHourDataFromProblemHebdo(
  PROBLEME_HEBDO* problemeHebdo,
  int cntCouplante,
  int pdt)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

    CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
      = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
    BindingConstraintData data
      = GetBindingConstraintDataFromProblemHebdo(problemeHebdo, cntCouplante);
    return {data, CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesContraintesCouplantes};
}

static BindingConstraintDayData GetBindingConstraintDayDataFromProblemHebdo(
  PROBLEME_HEBDO* problemeHebdo,
  int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

    BindingConstraintData data
      = GetBindingConstraintDataFromProblemHebdo(problemeHebdo, cntCouplante);
    // TODO transfrom std::vector<CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES> into
    // std::vector<std::vector<int>&>&
    // std::vector<std::vector<int>> correspondanceCntNativesCntOptimJournalieres;
    // std::transform(problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres.begin(),
    //                problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres.end(),
    //                std::back_inserter(correspondanceCntNativesCntOptimJournalieres),
    //                [](CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES& corr) -> std::vector<int>&
    //                { return corr.NumeroDeContrainteDesContraintesCouplantes; });

    return {data, problemeHebdo->CorrespondanceCntNativesCntOptimJournalieres};
}
static BindingConstraintWeekData GetBindingConstraintWeekDataFromProblemHebdo(
  PROBLEME_HEBDO* problemeHebdo,
  int cntCouplante)
{
    const CONTRAINTES_COUPLANTES& MatriceDesContraintesCouplantes
      = problemeHebdo->MatriceDesContraintesCouplantes[cntCouplante];

    BindingConstraintData data
      = GetBindingConstraintDataFromProblemHebdo(problemeHebdo, cntCouplante);

    return {data,
            problemeHebdo->CorrespondanceCntNativesCntOptimHebdomadaires
              .NumeroDeContrainteDesContraintesCouplantes};
}

static HydroPowerData GetHydroPowerDataFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo,
                                                        uint32_t pays)
{
    return {problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
            problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
            problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
            problemeHebdo->NombreDePasDeTempsPourUneOptimisation,
            problemeHebdo->NumeroDeContrainteEnergieHydraulique,
            problemeHebdo->CaracteristiquesHydrauliques[pays].PumpingRatio};
}

static MinHydroPowerData GetMinHydroPowerDataFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo,
                                                              uint32_t pays)
{
    return {problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
            problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
            problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
            problemeHebdo->NombreDePasDeTempsPourUneOptimisation,
            problemeHebdo->NumeroDeContrainteMinEnergieHydraulique};
}

static MaxHydroPowerData GetMaxHydroPowerDataFromProblemHebdo(PROBLEME_HEBDO* problemeHebdo,
                                                              uint32_t pays)
{
    return {problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable,
            problemeHebdo->CaracteristiquesHydrauliques[pays].TurbinageEntreBornes,
            problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
            problemeHebdo->NombreDePasDeTempsPourUneOptimisation,
            problemeHebdo->NumeroDeContrainteMaxEnergieHydraulique};
}

void OPT_ConstruireLaMatriceDesContraintesDuProblemeLineaire(PROBLEME_HEBDO* problemeHebdo, Solver::IResultWriter& writer)
{
    int var;


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
    ConstraintBuilder builder(GetConstraintBuilderFromProblemHebdo(problemeHebdo));

    AreaBalance areaBalance(builder);
    FictitiousLoad fictitiousLoad(builder);
    ShortTermStorageLevel shortTermStorageLevel(builder);
    FlowDissociation flowDissociation(builder);
    BindingConstraintHour bindingConstraintHour(builder);
    BindingConstraintDay bindingConstraintDay(builder);
    BindingConstraintWeek bindingConstraintWeek(builder);
    HydroPower hydroPower(builder);
    HydroPowerSmoothingUsingVariationSum hydroPowerSmoothingUsingVariationSum(builder);
    HydroPowerSmoothingUsingVariationMaxDown hydroPowerSmoothingUsingVariationMaxDown(builder);
    HydroPowerSmoothingUsingVariationMaxUp hydroPowerSmoothingUsingVariationMaxUp(builder);

    MinHydroPower minHydroPower(builder);
    MaxHydroPower maxHydroPower(builder);

    MaxPumping maxPumping(builder);

    AreaHydroLevel areaHydroLevel(builder);

    FinalStockEquivalent finalStockEquivalent(builder);
    FinalStockExpression finalStockExpression(builder);

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);
        CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim = problemeHebdo->CorrespondanceVarNativesVarOptim[pdt];
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
            = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            AreaBalanceData areaBalanceData{
              CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesBilansPays,
              CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage.InjectionVariable,
              CorrespondanceVarNativesVarOptim.SIM_ShortTermStorage.WithdrawalVariable,
              problemeHebdo->IndexDebutIntercoOrigine,
              problemeHebdo->IndexSuivantIntercoOrigine,
              problemeHebdo->IndexDebutIntercoExtremite,
              problemeHebdo->IndexSuivantIntercoExtremite,
              problemeHebdo->PaliersThermiquesDuPays[pays],
              problemeHebdo->ShortTermStorage};

            areaBalance.add(pdt, pays, areaBalanceData);

            FictitiousLoadData fictitiousLoadData
              = {CorrespondanceCntNativesCntOptim.NumeroDeContraintePourEviterLesChargesFictives,
                 problemeHebdo->PaliersThermiquesDuPays[pays],
                 problemeHebdo->DefaillanceNegativeUtiliserHydro};

            fictitiousLoad.add(pdt, pays, fictitiousLoadData);

            ShortTermStorageLevelData shortTermStorageLevelData = {
              CorrespondanceCntNativesCntOptim.ShortTermStorageLevelConstraint,
              problemeHebdo->ShortTermStorage,
            };
            shortTermStorageLevel.add(pdt, pays, shortTermStorageLevelData);
        }

        for (uint32_t interco = 0; interco < problemeHebdo->NombreDInterconnexions; interco++)
        {
            CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
              = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];
            FlowDissociationData flowDissociationData
              = {CorrespondanceCntNativesCntOptim.NumeroDeContrainteDeDissociationDeFlux,
                 problemeHebdo->CoutDeTransport,
                 problemeHebdo->PaysOrigineDeLInterconnexion,
                 problemeHebdo->PaysExtremiteDeLInterconnexion};

            flowDissociation.add(pdt, interco, flowDissociationData);
        }
        for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            auto data
              = GetBindingConstraintHourDataFromProblemHebdo(problemeHebdo, cntCouplante, pdt);
            bindingConstraintHour.add(pdt, cntCouplante, data);
        }
    }

    for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
         cntCouplante++)
    {
        auto data = GetBindingConstraintDayDataFromProblemHebdo(problemeHebdo, cntCouplante);
        bindingConstraintDay.add(cntCouplante, data);
    }

    if (nombreDePasDeTempsPourUneOptimisation > nombreDePasDeTempsDUneJournee)
    {
        for (uint32_t cntCouplante = 0; cntCouplante < problemeHebdo->NombreDeContraintesCouplantes;
             cntCouplante++)
        {
            auto data = GetBindingConstraintWeekDataFromProblemHebdo(problemeHebdo, cntCouplante);
            bindingConstraintWeek.add(cntCouplante, data);
        }
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        auto data = GetHydroPowerDataFromProblemHebdo(problemeHebdo, pays);
        hydroPower.add(pays, data);
    }

    if (problemeHebdo->TypeDeLissageHydraulique == LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS)
    {
        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            if (!problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDHydrauliqueModulable)
                continue;

            hydroPowerSmoothingUsingVariationSum.add(
              pays, problemeHebdo->NombreDePasDeTempsPourUneOptimisation);
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
        auto minHydroPowerData = GetMinHydroPowerDataFromProblemHebdo(problemeHebdo, pays);
        minHydroPower.add(pays, minHydroPowerData);

        auto maxHydroPowerData = GetMaxHydroPowerDataFromProblemHebdo(problemeHebdo, pays);
        maxHydroPower.add(pays, maxHydroPowerData);
    }

    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        MaxPumpingData data
          = {problemeHebdo->CaracteristiquesHydrauliques[pays].PresenceDePompageModulable,
             problemeHebdo->NumeroDeContrainteMaxPompage};
        maxPumping.add(pays, data);
    }

    for (int pdt = 0; pdt < nombreDePasDeTempsPourUneOptimisation; pdt++)
    {
        CORRESPONDANCES_DES_CONTRAINTES& CorrespondanceCntNativesCntOptim
          = problemeHebdo->CorrespondanceCntNativesCntOptim[pdt];

        int timeStepInYear = problemeHebdo->weekInTheYear * 168 + pdt;
        constraintNamer.UpdateTimeStep(timeStepInYear);

        for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
        {
            areaHydroLevel.add(
              pays, pdt, CorrespondanceCntNativesCntOptim.NumeroDeContrainteDesNiveauxPays);
        }
    }

    /* For each area with ad hoc properties, two possible sets of two additional constraints */
    for (uint32_t pays = 0; pays < problemeHebdo->NombreDePays; pays++)
    {
        FinalStockEquivalentData finalStockEquivalentData
          = {problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1,
             problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue,
             problemeHebdo->CaracteristiquesHydrauliques[pays].DirectLevelAccess,
             problemeHebdo->NumeroDeContrainteEquivalenceStockFinal};
        finalStockEquivalent.add(pays, finalStockEquivalentData);
        FinalStockExpressionData finalStockExpressionData
          = {problemeHebdo->NombreDePasDeTempsPourUneOptimisation - 1,
             problemeHebdo->CaracteristiquesHydrauliques[pays].AccurateWaterValue,
             problemeHebdo->NumeroDeContrainteExpressionStockFinal};

        finalStockExpression.add(pays, finalStockExpressionData);
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
