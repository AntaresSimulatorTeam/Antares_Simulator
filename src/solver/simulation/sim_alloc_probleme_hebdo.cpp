/*
** Copyright 2007-2018 RTE
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
#include <yuni/yuni.h>
#include <antares/study.h>

#include "../optimisation/opt_structure_probleme_a_resoudre.h"

#include "simulation.h"
#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_extern_variables_globales.h"

using namespace Antares;

void SIM_AllocationProblemeHebdo(PROBLEME_HEBDO& problem, int NombreDePasDeTemps)
{
    int k;
    int j;

    auto& study = *Data::Study::Current::Get();

    uint nbPays = study.areas.size();

    uint linkCount = study.runtime->interconnectionsCount;

    problem.DefaillanceNegativeUtiliserPMinThermique = (char*)MemAlloc(nbPays * sizeof(char));
    problem.DefaillanceNegativeUtiliserHydro = (char*)MemAlloc(nbPays * sizeof(char));
    problem.DefaillanceNegativeUtiliserConsoAbattue = (char*)MemAlloc(nbPays * sizeof(char));

    problem.CoefficientEcretementPMaxHydraulique = (double*)MemAlloc(nbPays * sizeof(double));

    problem.BruitSurCoutHydraulique = (double**)MemAlloc(nbPays * sizeof(double*));
    for (uint p = 0; p < nbPays; ++p)
        problem.BruitSurCoutHydraulique[p] = (double*)MemAlloc(8784 * sizeof(double));

    problem.NomsDesPays = (const char**)MemAlloc(nbPays * sizeof(char*));
    problem.PaysExtremiteDeLInterconnexion = (int*)MemAlloc(linkCount * sizeof(int));
    problem.PaysOrigineDeLInterconnexion = (int*)MemAlloc(linkCount * sizeof(int));
    problem.CoutDeTransport = (COUTS_DE_TRANSPORT**)MemAlloc(linkCount * sizeof(void*));
    problem.IndexDebutIntercoOrigine = (int*)MemAlloc(nbPays * sizeof(int));
    problem.IndexDebutIntercoExtremite = (int*)MemAlloc(nbPays * sizeof(int));
    problem.IndexSuivantIntercoOrigine = (int*)MemAlloc(linkCount * sizeof(int));
    problem.IndexSuivantIntercoExtremite = (int*)MemAlloc(linkCount * sizeof(int));
    problem.NumeroDeJourDuPasDeTemps = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
    problem.NumeroDIntervalleOptimiseDuPasDeTemps
      = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
    problem.NbGrpCourbeGuide = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
    problem.NbGrpOpt = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
    problem.CoutDeDefaillancePositive = (double*)MemAlloc(nbPays * sizeof(double));
    problem.CoutDeDefaillanceNegative = (double*)MemAlloc(nbPays * sizeof(double));
    problem.CoutDeDefaillanceEnReserve = (double*)MemAlloc(nbPays * sizeof(double));
    problem.NumeroDeContrainteEnergieHydraulique = (int*)MemAlloc(nbPays * sizeof(int));
    problem.NumeroDeContrainteMinEnergieHydraulique = (int*)MemAlloc(nbPays * sizeof(int));
    problem.NumeroDeContrainteMaxEnergieHydraulique = (int*)MemAlloc(nbPays * sizeof(int));
    problem.NumeroDeContrainteMaxPompage = (int*)MemAlloc(nbPays * sizeof(int));
    problem.NumeroDeContrainteDeSoldeDEchange = (int*)MemAlloc(nbPays * sizeof(int));

    problem.NumeroDeContrainteBorneStockFinal = (int*)MemAlloc(nbPays * sizeof(int));
    problem.NumeroDeContrainteEquivalenceStockFinal = (int*)MemAlloc(nbPays * sizeof(int));
    problem.NumeroDeContrainteExpressionStockFinal = (int*)MemAlloc(nbPays * sizeof(int));

    problem.NumeroDeVariableStockFinal = (int*)MemAlloc(nbPays * sizeof(int));
    problem.NumeroDeVariableDeTrancheDeStock = (int**)MemAlloc(nbPays * sizeof(int*));
    for (uint p = 0; p < nbPays; ++p)
        problem.NumeroDeVariableDeTrancheDeStock[p] = (int*)MemAlloc(100 * sizeof(int));

    problem.ValeursDeNTC
      = (VALEURS_DE_NTC_ET_RESISTANCES**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.ValeursDeNTCRef
      = (VALEURS_DE_NTC_ET_RESISTANCES**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.ConsommationsAbattues
      = (CONSOMMATIONS_ABATTUES**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.ConsommationsAbattuesRef
      = (CONSOMMATIONS_ABATTUES**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.AllMustRunGeneration
      = (ALL_MUST_RUN_GENERATION**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.SoldeMoyenHoraire
      = (SOLDE_MOYEN_DES_ECHANGES**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.CorrespondanceVarNativesVarOptim
      = (CORRESPONDANCES_DES_VARIABLES**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.CorrespondanceCntNativesCntOptim
      = (CORRESPONDANCES_DES_CONTRAINTES**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.VariablesDualesDesContraintesDeNTC
      = (VARIABLES_DUALES_INTERCONNEXIONS**)MemAlloc(NombreDePasDeTemps * sizeof(void*));
    problem.MatriceDesContraintesCouplantes
      = (CONTRAINTES_COUPLANTES**)MemAlloc(study.runtime->bindingConstraintCount * sizeof(void*));
    problem.PaliersThermiquesDuPays = (PALIERS_THERMIQUES**)MemAlloc(nbPays * sizeof(void*));
    problem.CaracteristiquesHydrauliques
      = (ENERGIES_ET_PUISSANCES_HYDRAULIQUES**)MemAlloc(nbPays * sizeof(void*));
    problem.previousSimulationFinalLevel = (double*)MemAlloc(nbPays * sizeof(double));

    problem.previousYearFinalLevels = nullptr;
    if (problem.hydroHotStart)
    {
        for (uint i = 0; i != nbPays; i++)
        {
            auto& area = *(study.areas[i]);
            if (area.hydro.reservoirManagement)
            {
                problem.previousYearFinalLevels = (double*)MemAlloc(nbPays * sizeof(double));
                break;
            }
        }
    }

    problem.NumeroDeZoneDeReserveJMoins1 = (int*)MemAlloc(nbPays * sizeof(int));

    problem.CoutsMarginauxDesContraintesDeReserveParZone
      = (COUTS_MARGINAUX_ZONES_DE_RESERVE**)MemAlloc(nbPays * sizeof(void*));

    problem.ReserveJMoins1 = (RESERVE_JMOINS1**)MemAlloc(nbPays * sizeof(void*));
    problem.ResultatsHoraires = (RESULTATS_HORAIRES**)MemAlloc(nbPays * sizeof(void*));

    for (uint p = 0; p != nbPays; ++p)
    {
        problem.IndexDebutIntercoOrigine[p] = -1;
        problem.IndexDebutIntercoExtremite[p] = -1;
    }

    for (k = 0; k < NombreDePasDeTemps; k++)
    {
        problem.ValeursDeNTC[k]
          = (VALEURS_DE_NTC_ET_RESISTANCES*)MemAlloc(sizeof(VALEURS_DE_NTC_ET_RESISTANCES));
        problem.ValeursDeNTCRef[k]
          = (VALEURS_DE_NTC_ET_RESISTANCES*)MemAlloc(sizeof(VALEURS_DE_NTC_ET_RESISTANCES));
        problem.ConsommationsAbattues[k]
          = (CONSOMMATIONS_ABATTUES*)MemAlloc(sizeof(CONSOMMATIONS_ABATTUES));
        problem.ConsommationsAbattuesRef[k]
          = (CONSOMMATIONS_ABATTUES*)MemAlloc(sizeof(CONSOMMATIONS_ABATTUES));
        problem.AllMustRunGeneration[k]
          = (ALL_MUST_RUN_GENERATION*)MemAlloc(sizeof(ALL_MUST_RUN_GENERATION));
        problem.SoldeMoyenHoraire[k]
          = (SOLDE_MOYEN_DES_ECHANGES*)MemAlloc(sizeof(SOLDE_MOYEN_DES_ECHANGES));
        problem.CorrespondanceVarNativesVarOptim[k]
          = (CORRESPONDANCES_DES_VARIABLES*)MemAlloc(sizeof(CORRESPONDANCES_DES_VARIABLES));
        problem.CorrespondanceCntNativesCntOptim[k]
          = (CORRESPONDANCES_DES_CONTRAINTES*)MemAlloc(sizeof(CORRESPONDANCES_DES_CONTRAINTES));
        problem.VariablesDualesDesContraintesDeNTC[k]
          = (VARIABLES_DUALES_INTERCONNEXIONS*)MemAlloc(sizeof(VARIABLES_DUALES_INTERCONNEXIONS));
        problem.ValeursDeNTC[k]->ResistanceApparente
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTC[k]->ValeurDeNTCExtremiteVersOrigine
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTC[k]->ValeurDeNTCOrigineVersExtremite
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTC[k]->ValeurDeLoopFlowOrigineVersExtremite
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTC[k]->ValeurDuFlux = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTC[k]->ValeurDuFluxUp = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTC[k]->ValeurDuFluxDown = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTCRef[k]->ResistanceApparente
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTCRef[k]->ValeurDeNTCExtremiteVersOrigine
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTCRef[k]->ValeurDeLoopFlowOrigineVersExtremite
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTCRef[k]->ValeurDeNTCOrigineVersExtremite
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTCRef[k]->ValeurDuFlux = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTCRef[k]->ValeurDuFluxUp = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ValeursDeNTCRef[k]->ValeurDuFluxDown
          = (double*)MemAlloc(linkCount * sizeof(double));
        problem.ConsommationsAbattues[k]->ConsommationAbattueDuPays
          = (double*)MemAlloc(nbPays * sizeof(double));
        problem.ConsommationsAbattuesRef[k]->ConsommationAbattueDuPays
          = (double*)MemAlloc(nbPays * sizeof(double));
        problem.AllMustRunGeneration[k]->AllMustRunGenerationOfArea
          = (double*)MemAlloc(nbPays * sizeof(double));
        problem.SoldeMoyenHoraire[k]->SoldeMoyenDuPays
          = (double*)MemAllocMemset(nbPays * sizeof(double));

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDeLInterconnexion
          = (int*)MemAlloc(linkCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion
          = (int*)MemAlloc(linkCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion
          = (int*)MemAlloc(linkCount * sizeof(int));

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermique
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermiqueUp
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermiqueDown
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHyd
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHydUp
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHydDown
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDePompage
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeNiveau
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeDebordement
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositive
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositiveUp
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositiveDown
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositiveAny
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegative
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegativeUp
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegativeDown
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegativeAny
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeGrosseVariableDefaillancePositive
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeGrosseVariableDefaillanceNegative
          = (int*)MemAlloc(nbPays * sizeof(int));

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceEnReserve
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeGrosseVariableDefaillanceEnReserve
          = (int*)MemAlloc(nbPays * sizeof(int));

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisse
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisseUp
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisseDown
          = (int*)MemAlloc(nbPays * sizeof(int));

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausse
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausseUp
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausseDown
          = (int*)MemAlloc(nbPays * sizeof(int));

        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));

        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesBilansPays
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContraintePourEviterLesChargesFictives
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesNiveauxPays
          = (int*)MemAlloc(nbPays * sizeof(int));

        problem.CorrespondanceCntNativesCntOptim[k]->NumeroPremiereContrainteDeReserveParZone
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeuxiemeContrainteDeReserveParZone
          = (int*)MemAlloc(nbPays * sizeof(int));
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDeDissociationDeFlux
          = (int*)MemAlloc(linkCount * sizeof(int));
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesContraintesCouplantes
          = (int*)MemAlloc(study.runtime->bindingConstraintCount * sizeof(int));

        problem.CorrespondanceCntNativesCntOptim[k]
          ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));
        problem.CorrespondanceCntNativesCntOptim[k]
          ->NumeroDeContrainteDesContraintesDeDureeMinDArret
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));

        problem.CorrespondanceCntNativesCntOptim[k]
          ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
          = (int*)MemAlloc(study.runtime->thermalPlantTotalCount * sizeof(int));

        problem.VariablesDualesDesContraintesDeNTC[k]->VariableDualeParInterconnexion
          = (double*)MemAlloc(linkCount * sizeof(double));
    }

    for (k = 0; k < (int)linkCount; ++k)
    {
        problem.CoutDeTransport[k] = (COUTS_DE_TRANSPORT*)MemAlloc(sizeof(COUTS_DE_TRANSPORT));
        problem.CoutDeTransport[k]->IntercoGereeAvecDesCouts = NON_ANTARES;
        problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremite
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigine
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));

        problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremiteRef
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigineRef
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
    }

    problem.CorrespondanceCntNativesCntOptimJournalieres
      = (CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES**)MemAlloc(7 * sizeof(void*));
    for (k = 0; k < 7; k++)
    {
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          = (CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES*)MemAlloc(
            sizeof(CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES));
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          ->NumeroDeContrainteDesContraintesCouplantes
          = (int*)MemAlloc(study.runtime->bindingConstraintCount * sizeof(int));
    }

    problem.CorrespondanceCntNativesCntOptimHebdomadaires
      = (CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES**)MemAlloc(1 * sizeof(void*));
    for (k = 0; k < 1; k++)
    {
        problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
          = (CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES*)MemAlloc(
            sizeof(CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES));
        problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
          ->NumeroDeContrainteDesContraintesCouplantes
          = (int*)MemAlloc(study.runtime->bindingConstraintCount * sizeof(int));
    }

    for (k = 0; k < (int)study.runtime->bindingConstraintCount; k++)
    {
        problem.MatriceDesContraintesCouplantes[k]
          = (CONTRAINTES_COUPLANTES*)MemAlloc(sizeof(CONTRAINTES_COUPLANTES));

        assert((uint)k < study.runtime->bindingConstraintCount);
        assert(study.runtime->bindingConstraint[k].linkCount < 50000000);
        assert(study.runtime->bindingConstraint[k].clusterCount < 50000000);

        problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));

        problem.MatriceDesContraintesCouplantes[k]->NumeroDeLInterconnexion
          = (int*)MemAlloc(study.runtime->bindingConstraint[k].linkCount * sizeof(int));
        problem.MatriceDesContraintesCouplantes[k]->PoidsDeLInterconnexion
          = (double*)MemAlloc(study.runtime->bindingConstraint[k].linkCount * sizeof(double));
        problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLInterco
          = (int*)MemAlloc(study.runtime->bindingConstraint[k].linkCount * sizeof(int));

        problem.MatriceDesContraintesCouplantes[k]->NumeroDuPalierDispatch
          = (int*)MemAlloc(study.runtime->bindingConstraint[k].clusterCount * sizeof(int));
        problem.MatriceDesContraintesCouplantes[k]->PoidsDuPalierDispatch
          = (double*)MemAlloc(study.runtime->bindingConstraint[k].clusterCount * sizeof(double));
        problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLePalierDispatch
          = (int*)MemAlloc(study.runtime->bindingConstraint[k].clusterCount * sizeof(int));
        problem.MatriceDesContraintesCouplantes[k]->PaysDuPalierDispatch
          = (int*)MemAlloc(study.runtime->bindingConstraint[k].clusterCount * sizeof(int));
    }

    for (k = 0; k < (int)nbPays; k++)
    {
        const uint nbPaliers = (study.areas.byIndex[k])->thermal.list.size();

        problem.PaliersThermiquesDuPays[k]
          = (PALIERS_THERMIQUES*)MemAlloc(sizeof(PALIERS_THERMIQUES));
        problem.CaracteristiquesHydrauliques[k] = (ENERGIES_ET_PUISSANCES_HYDRAULIQUES*)MemAlloc(
          sizeof(ENERGIES_ET_PUISSANCES_HYDRAULIQUES));

        problem.CoutsMarginauxDesContraintesDeReserveParZone[k]
          = (COUTS_MARGINAUX_ZONES_DE_RESERVE*)MemAlloc(sizeof(COUTS_MARGINAUX_ZONES_DE_RESERVE));
        problem.CoutsMarginauxDesContraintesDeReserveParZone[k]
          ->CoutsMarginauxHorairesDeLaReserveParZone
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));

        problem.ReserveJMoins1[k] = (RESERVE_JMOINS1*)MemAlloc(sizeof(RESERVE_JMOINS1));
        problem.ResultatsHoraires[k] = (RESULTATS_HORAIRES*)MemAlloc(sizeof(RESULTATS_HORAIRES));

        problem.PaliersThermiquesDuPays[k]->minUpDownTime = (int*)MemAlloc(nbPaliers * sizeof(int));
        problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUneHeure
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUneSemaine
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUnJour
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->TailleUnitaireDUnGroupeDuPalierThermique
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->NumeroDuPalierDansLEnsembleDesPaliersThermiques
          = (int*)MemAlloc(nbPaliers * sizeof(int));

        problem.PaliersThermiquesDuPays[k]->CoutDeDemarrageDUnGroupeDuPalierThermique
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->CoutDArretDUnGroupeDuPalierThermique
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->CoutFixeDeMarcheDUnGroupeDuPalierThermique
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->PminDUnGroupeDuPalierThermique
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->PmaxDUnGroupeDuPalierThermique
          = (double*)MemAlloc(nbPaliers * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
          = (int*)MemAlloc(nbPaliers * sizeof(int));
        problem.PaliersThermiquesDuPays[k]->DureeMinimaleDArretDUnGroupeDuPalierThermique
          = (int*)MemAlloc(nbPaliers * sizeof(int));

        problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise
          = (double*)MemAllocMemset(7 * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParJour
          = (double*)MemAllocMemset(7 * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimiseRef
          = (double*)MemAlloc(7 * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire
          = (double*)MemAllocMemset(NombreDePasDeTemps * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraireRef
          = (double*)MemAllocMemset(NombreDePasDeTemps * sizeof(double));

        problem.CaracteristiquesHydrauliques[k]->MaxEnergieHydrauParIntervalleOptimise
          = (double*)MemAllocMemset(7 * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->MinEnergieHydrauParIntervalleOptimise
          = (double*)MemAllocMemset(7 * sizeof(double));

        problem.CaracteristiquesHydrauliques[k]->NiveauHoraireSup
          = (double*)MemAllocMemset(NombreDePasDeTemps * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->NiveauHoraireInf
          = (double*)MemAllocMemset(NombreDePasDeTemps * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->ApportNaturelHoraire
          = (double*)MemAllocMemset(NombreDePasDeTemps * sizeof(double));

        problem.CaracteristiquesHydrauliques[k]->WaterLayerValues
          = (double*)MemAllocMemset(100 * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->InflowForTimeInterval
          = (double*)MemAllocMemset(100 * sizeof(double));

        problem.CaracteristiquesHydrauliques[k]->MaxEnergiePompageParIntervalleOptimise
          = (double*)MemAllocMemset(7 * sizeof(double));
        problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxPompageHoraire
          = (double*)MemAllocMemset(NombreDePasDeTemps * sizeof(double));

        problem.ReserveJMoins1[k]->ReserveHoraireJMoins1
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ReserveJMoins1[k]->ReserveHoraireJMoins1Ref
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositive
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDENS
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double)); // adq patch
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositiveUp
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositiveDown
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositiveAny
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegative
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegativeUp
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegativeDown
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegativeAny
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceEnReserve
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->TurbinageHoraire
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->PompageHoraire
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->TurbinageHoraireUp
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->TurbinageHoraireDown
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->CoutsMarginauxHoraires
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->niveauxHoraires
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->valeurH2oHoraire
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.ResultatsHoraires[k]->debordementsHoraires
          = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout
          = (PDISP_ET_COUTS_HORAIRES_PAR_PALIER**)MemAlloc(nbPaliers * sizeof(void*));
        problem.ResultatsHoraires[k]->ProductionThermique
          = (PRODUCTION_THERMIQUE_OPTIMALE**)MemAlloc(NombreDePasDeTemps * sizeof(void*));

        for (j = 0; j < (int)nbPaliers; ++j)
        {
            problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[j]
              = (PDISP_ET_COUTS_HORAIRES_PAR_PALIER*)MemAlloc(
                sizeof(PDISP_ET_COUTS_HORAIRES_PAR_PALIER));

            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDeProductionDuPalierThermique
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDeProductionDuPalierThermiqueRef
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceDisponibleDuPalierThermique
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceDisponibleDuPalierThermiqueRef
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceDisponibleDuPalierThermiqueRef_SV
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceMinDuPalierThermique
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceMinDuPalierThermique_SV
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->NombreMaxDeGroupesEnMarcheDuPalierThermique
              = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->NombreMinDeGroupesEnMarcheDuPalierThermique
              = (int*)MemAlloc(NombreDePasDeTemps * sizeof(int));

            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDuPalierThermiqueUp
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDuPalierThermiqueDown
              = (double*)MemAlloc(NombreDePasDeTemps * sizeof(double));
        }
        for (j = 0; j < NombreDePasDeTemps; j++)
        {
            problem.ResultatsHoraires[k]->ProductionThermique[j]
              = (PRODUCTION_THERMIQUE_OPTIMALE*)MemAlloc(sizeof(PRODUCTION_THERMIQUE_OPTIMALE));
            problem.ResultatsHoraires[k]->ProductionThermique[j]->ProductionThermiqueDuPalier
              = (double*)MemAlloc(nbPaliers * sizeof(double));
            problem.ResultatsHoraires[k]->ProductionThermique[j]->ProductionThermiqueDuPalierUp
              = (double*)MemAlloc(nbPaliers * sizeof(double));
            problem.ResultatsHoraires[k]->ProductionThermique[j]->ProductionThermiqueDuPalierDown
              = (double*)MemAlloc(nbPaliers * sizeof(double));
            problem.ResultatsHoraires[k]->ProductionThermique[j]->NombreDeGroupesEnMarcheDuPalier
              = (double*)MemAlloc(nbPaliers * sizeof(double));
            problem.ResultatsHoraires[k]
              ->ProductionThermique[j]
              ->NombreDeGroupesQuiDemarrentDuPalier
              = (double*)MemAlloc(nbPaliers * sizeof(double));
            problem.ResultatsHoraires[k]
              ->ProductionThermique[j]
              ->NombreDeGroupesQuiSArretentDuPalier
              = (double*)MemAlloc(nbPaliers * sizeof(double));
            problem.ResultatsHoraires[k]
              ->ProductionThermique[j]
              ->NombreDeGroupesQuiTombentEnPanneDuPalier
              = (double*)MemAlloc(nbPaliers * sizeof(double));
        }
    }

    problem.numeroOptimisation = (int*)MemAlloc(7 * sizeof(int));
    problem.coutOptimalSolution1 = (double*)MemAlloc(7 * sizeof(double));
    problem.coutOptimalSolution2 = (double*)MemAlloc(7 * sizeof(double));
}

void SIM_DesallocationProblemeHebdo(PROBLEME_HEBDO& problem)
{
    auto& study = *Data::Study::Current::Get();

    uint nbPays = study.areas.size();

    MemFree(problem.NomsDesPays);
    MemFree(problem.PaysExtremiteDeLInterconnexion);
    MemFree(problem.PaysOrigineDeLInterconnexion);
    MemFree(problem.IndexDebutIntercoOrigine);
    MemFree(problem.IndexDebutIntercoExtremite);
    MemFree(problem.IndexSuivantIntercoOrigine);
    MemFree(problem.IndexSuivantIntercoExtremite);
    MemFree(problem.NumeroDeJourDuPasDeTemps);
    MemFree(problem.NumeroDIntervalleOptimiseDuPasDeTemps);
    MemFree(problem.NbGrpCourbeGuide);
    MemFree(problem.NbGrpOpt);

    for (int k = 0; k < problem.NombreDePasDeTemps; k++)
    {
        MemFree(problem.ValeursDeNTC[k]->ResistanceApparente);
        MemFree(problem.ValeursDeNTC[k]->ValeurDeNTCExtremiteVersOrigine);
        MemFree(problem.ValeursDeNTC[k]->ValeurDeNTCOrigineVersExtremite);
        MemFree(problem.ValeursDeNTC[k]->ValeurDeLoopFlowOrigineVersExtremite);
        MemFree(problem.ValeursDeNTC[k]->ValeurDuFlux);
        MemFree(problem.ValeursDeNTC[k]->ValeurDuFluxUp);
        MemFree(problem.ValeursDeNTC[k]->ValeurDuFluxDown);
        MemFree(problem.ValeursDeNTC[k]);
        MemFree(problem.ConsommationsAbattues[k]->ConsommationAbattueDuPays);
        MemFree(problem.ConsommationsAbattues[k]);
        MemFree(problem.ValeursDeNTCRef[k]->ResistanceApparente);
        MemFree(problem.ValeursDeNTCRef[k]->ValeurDeNTCExtremiteVersOrigine);
        MemFree(problem.ValeursDeNTCRef[k]->ValeurDeNTCOrigineVersExtremite);
        MemFree(problem.ValeursDeNTCRef[k]->ValeurDeLoopFlowOrigineVersExtremite);
        MemFree(problem.ValeursDeNTCRef[k]->ValeurDuFlux);
        MemFree(problem.ValeursDeNTCRef[k]->ValeurDuFluxUp);
        MemFree(problem.ValeursDeNTCRef[k]->ValeurDuFluxDown);
        MemFree(problem.ValeursDeNTCRef[k]);
        MemFree(problem.ConsommationsAbattuesRef[k]->ConsommationAbattueDuPays);
        MemFree(problem.ConsommationsAbattuesRef[k]);
        MemFree(problem.AllMustRunGeneration[k]->AllMustRunGenerationOfArea);
        MemFree(problem.AllMustRunGeneration[k]);
        MemFree(problem.SoldeMoyenHoraire[k]->SoldeMoyenDuPays);
        MemFree(problem.SoldeMoyenHoraire[k]);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDeLInterconnexion);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermique);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermiqueUp);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermiqueDown);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHyd);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHydUp);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHydDown);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDePompage);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeNiveau);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeDebordement);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositive);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositiveUp);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositiveDown);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositiveAny);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegative);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegativeUp);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegativeDown);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegativeAny);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeGrosseVariableDefaillancePositive);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeGrosseVariableDefaillanceNegative);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceEnReserve);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeGrosseVariableDefaillanceEnReserve);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisse);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisseUp);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisseDown);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausse);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausseUp);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausseDown);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique);

        MemFree(problem.CorrespondanceVarNativesVarOptim[k]);
        MemFree(problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesBilansPays);
        MemFree(problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesNiveauxPays);
        MemFree(problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeContraintePourEviterLesChargesFictives);

        MemFree(
          problem.CorrespondanceCntNativesCntOptim[k]->NumeroPremiereContrainteDeReserveParZone);
        MemFree(
          problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeuxiemeContrainteDeReserveParZone);
        MemFree(
          problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDeDissociationDeFlux);
        MemFree(
          problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesContraintesCouplantes);
        MemFree(problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche);
        MemFree(problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeContrainteDesContraintesDeDureeMinDArret);
        MemFree(problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne);
        MemFree(problem.CorrespondanceCntNativesCntOptim[k]);
        MemFree(problem.VariablesDualesDesContraintesDeNTC[k]->VariableDualeParInterconnexion);
        MemFree(problem.VariablesDualesDesContraintesDeNTC[k]);
    }
    MemFree(problem.ValeursDeNTC);
    MemFree(problem.ConsommationsAbattues);
    MemFree(problem.ValeursDeNTCRef);
    MemFree(problem.ConsommationsAbattuesRef);
    MemFree(problem.AllMustRunGeneration);
    MemFree(problem.SoldeMoyenHoraire);
    MemFree(problem.CorrespondanceVarNativesVarOptim);
    MemFree(problem.CorrespondanceCntNativesCntOptim);
    MemFree(problem.VariablesDualesDesContraintesDeNTC);

    for (int k = 0; k < (int)study.runtime->interconnectionsCount; k++)
    {
        MemFree(problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremite);
        MemFree(problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigine);

        MemFree(problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremiteRef);
        MemFree(problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigineRef);
        MemFree(problem.CoutDeTransport[k]);
    }
    MemFree(problem.CoutDeTransport);

    for (int k = 0; k < 7; k++)
    {
        MemFree(problem.CorrespondanceCntNativesCntOptimJournalieres[k]);
    }
    MemFree(problem.CorrespondanceCntNativesCntOptimJournalieres);

    for (int k = 0; k < 1; k++)
    {
        MemFree(problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
                  ->NumeroDeContrainteDesContraintesCouplantes);
    }
    MemFree(problem.CorrespondanceCntNativesCntOptimHebdomadaires);

    for (int k = 0; k < (int)study.runtime->bindingConstraintCount; k++)
    {
        MemFree(problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante);
        MemFree(problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef);

        MemFree(problem.MatriceDesContraintesCouplantes[k]->NumeroDeLInterconnexion);
        MemFree(problem.MatriceDesContraintesCouplantes[k]->PoidsDeLInterconnexion);
        MemFree(problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLInterco);

        MemFree(problem.MatriceDesContraintesCouplantes[k]->PoidsDuPalierDispatch);
        MemFree(problem.MatriceDesContraintesCouplantes[k]->PaysDuPalierDispatch);
        MemFree(problem.MatriceDesContraintesCouplantes[k]->NumeroDuPalierDispatch);
        MemFree(problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLePalierDispatch);

        MemFree(problem.MatriceDesContraintesCouplantes[k]);
    }
    MemFree(problem.MatriceDesContraintesCouplantes);

    for (int k = 0; k < (int)nbPays; ++k)
    {
        const uint nbPaliers = (study.areas.byIndex[k])->thermal.list.size();

        MemFree(problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUneHeure);
        MemFree(problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUneSemaine);
        MemFree(problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUnJour);
        MemFree(problem.PaliersThermiquesDuPays[k]->minUpDownTime);
        MemFree(problem.PaliersThermiquesDuPays[k]->TailleUnitaireDUnGroupeDuPalierThermique);
        MemFree(
          problem.PaliersThermiquesDuPays[k]->NumeroDuPalierDansLEnsembleDesPaliersThermiques);

        MemFree(problem.PaliersThermiquesDuPays[k]->CoutDeDemarrageDUnGroupeDuPalierThermique);
        MemFree(problem.PaliersThermiquesDuPays[k]->CoutDArretDUnGroupeDuPalierThermique);
        MemFree(problem.PaliersThermiquesDuPays[k]->CoutFixeDeMarcheDUnGroupeDuPalierThermique);
        MemFree(problem.PaliersThermiquesDuPays[k]->PminDUnGroupeDuPalierThermique);
        MemFree(problem.PaliersThermiquesDuPays[k]->PmaxDUnGroupeDuPalierThermique);
        MemFree(
          problem.PaliersThermiquesDuPays[k]->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique);
        MemFree(problem.PaliersThermiquesDuPays[k]->DureeMinimaleDArretDUnGroupeDuPalierThermique);

        MemFree(problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise);
        MemFree(problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParJour);
        MemFree(problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimiseRef);

        MemFree(problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire);
        MemFree(problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraireRef);

        MemFree(problem.CaracteristiquesHydrauliques[k]->MaxEnergieHydrauParIntervalleOptimise);
        MemFree(problem.CaracteristiquesHydrauliques[k]->MinEnergieHydrauParIntervalleOptimise);

        MemFree(problem.CaracteristiquesHydrauliques[k]->MaxEnergiePompageParIntervalleOptimise);
        MemFree(problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxPompageHoraire);

        MemFree(problem.CaracteristiquesHydrauliques[k]->NiveauHoraireSup);
        MemFree(problem.CaracteristiquesHydrauliques[k]->NiveauHoraireInf);
        MemFree(problem.CaracteristiquesHydrauliques[k]->ApportNaturelHoraire);

        MemFree(problem.CaracteristiquesHydrauliques[k]->WaterLayerValues);
        MemFree(problem.CaracteristiquesHydrauliques[k]->InflowForTimeInterval);
        MemFree(problem.CaracteristiquesHydrauliques[k]);

        MemFree(problem.CoutsMarginauxDesContraintesDeReserveParZone[k]
                  ->CoutsMarginauxHorairesDeLaReserveParZone);
        MemFree(problem.CoutsMarginauxDesContraintesDeReserveParZone[k]);
        MemFree(problem.ReserveJMoins1[k]->ReserveHoraireJMoins1);
        MemFree(problem.ReserveJMoins1[k]->ReserveHoraireJMoins1Ref);
        MemFree(problem.ReserveJMoins1[k]);

        for (int j = 0; j < (int)nbPaliers; j++)
        {
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDeProductionDuPalierThermique);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDeProductionDuPalierThermiqueRef);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceDisponibleDuPalierThermique);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceDisponibleDuPalierThermiqueRef);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceDisponibleDuPalierThermiqueRef_SV);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceMinDuPalierThermique);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceMinDuPalierThermique_SV);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->NombreMaxDeGroupesEnMarcheDuPalierThermique);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->NombreMinDeGroupesEnMarcheDuPalierThermique);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDuPalierThermiqueUp);
            MemFree(problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDuPalierThermiqueDown);
            MemFree(problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[j]);
        }
        MemFree(problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout);
        MemFree(problem.PaliersThermiquesDuPays[k]);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositive);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDENS);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositiveUp);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositiveDown);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillancePositiveAny);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegative);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegativeUp);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegativeDown);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceNegativeAny);
        MemFree(problem.ResultatsHoraires[k]->ValeursHorairesDeDefaillanceEnReserve);
        MemFree(problem.ResultatsHoraires[k]->TurbinageHoraire);
        MemFree(problem.ResultatsHoraires[k]->PompageHoraire);
        MemFree(problem.ResultatsHoraires[k]->TurbinageHoraireUp);
        MemFree(problem.ResultatsHoraires[k]->TurbinageHoraireDown);
        MemFree(problem.ResultatsHoraires[k]->niveauxHoraires);
        MemFree(problem.ResultatsHoraires[k]->valeurH2oHoraire);
        MemFree(problem.ResultatsHoraires[k]->debordementsHoraires);
        MemFree(problem.ResultatsHoraires[k]->CoutsMarginauxHoraires);
        for (int j = 0; j < problem.NombreDePasDeTemps; j++)
        {
            MemFree(
              problem.ResultatsHoraires[k]->ProductionThermique[j]->ProductionThermiqueDuPalier);
            MemFree(
              problem.ResultatsHoraires[k]->ProductionThermique[j]->ProductionThermiqueDuPalierUp);
            MemFree(problem.ResultatsHoraires[k]
                      ->ProductionThermique[j]
                      ->ProductionThermiqueDuPalierDown);
            MemFree(problem.ResultatsHoraires[k]
                      ->ProductionThermique[j]
                      ->NombreDeGroupesEnMarcheDuPalier);
            MemFree(problem.ResultatsHoraires[k]
                      ->ProductionThermique[j]
                      ->NombreDeGroupesQuiDemarrentDuPalier);
            MemFree(problem.ResultatsHoraires[k]
                      ->ProductionThermique[j]
                      ->NombreDeGroupesQuiSArretentDuPalier);
            MemFree(problem.ResultatsHoraires[k]
                      ->ProductionThermique[j]
                      ->NombreDeGroupesQuiTombentEnPanneDuPalier);
            MemFree(problem.ResultatsHoraires[k]->ProductionThermique[j]);
        }
        MemFree(problem.ResultatsHoraires[k]->ProductionThermique);
        MemFree(problem.ResultatsHoraires[k]);

        MemFree(problem.BruitSurCoutHydraulique[k]);
    }
    MemFree(problem.PaliersThermiquesDuPays);
    MemFree(problem.CaracteristiquesHydrauliques);
    MemFree(problem.previousSimulationFinalLevel);
    if (problem.previousYearFinalLevels)
        MemFree(problem.previousYearFinalLevels);

    MemFree(problem.NumeroDeZoneDeReserveJMoins1);
    MemFree(problem.CoutsMarginauxDesContraintesDeReserveParZone);
    MemFree(problem.ReserveJMoins1);
    MemFree(problem.ResultatsHoraires);

    MemFree(problem.CoutDeDefaillancePositive);
    MemFree(problem.CoutDeDefaillanceNegative);
    MemFree(problem.CoutDeDefaillanceEnReserve);
    MemFree(problem.NumeroDeContrainteEnergieHydraulique);
    MemFree(problem.NumeroDeContrainteMinEnergieHydraulique);
    MemFree(problem.NumeroDeContrainteMaxEnergieHydraulique);
    MemFree(problem.NumeroDeContrainteMaxPompage);
    MemFree(problem.NumeroDeContrainteDeSoldeDEchange);

    MemFree(problem.NumeroDeContrainteBorneStockFinal);
    MemFree(problem.NumeroDeContrainteEquivalenceStockFinal);
    MemFree(problem.NumeroDeContrainteExpressionStockFinal);
    MemFree(problem.NumeroDeVariableStockFinal);
    for (uint p = 0; p < nbPays; ++p)
        MemFree(problem.NumeroDeVariableDeTrancheDeStock[p]);
    MemFree(problem.NumeroDeVariableDeTrancheDeStock);

    MemFree(problem.DefaillanceNegativeUtiliserConsoAbattue);
    MemFree(problem.DefaillanceNegativeUtiliserHydro);
    MemFree(problem.DefaillanceNegativeUtiliserPMinThermique);

    MemFree(problem.CoefficientEcretementPMaxHydraulique);

    MemFree(problem.BruitSurCoutHydraulique);

    MemFree(problem.numeroOptimisation);
    MemFree(problem.coutOptimalSolution1);
    MemFree(problem.coutOptimalSolution2);
}
