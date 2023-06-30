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
#include <yuni/yuni.h>
#include <antares/study.h>

#include "../optimisation/opt_structure_probleme_a_resoudre.h"

#include "simulation.h"
#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_extern_variables_globales.h"

using namespace Antares;

void SIM_AllocationProblemeHebdo(PROBLEME_HEBDO& problem, unsigned NombreDePasDeTemps)
{
    auto& study = *Data::Study::Current::Get();

    uint nbPays = study.areas.size();

    const uint linkCount = study.runtime->interconnectionsCount();
    const uint shortTermStorageCount = study.runtime->shortTermStorageCount;

    problem.DefaillanceNegativeUtiliserPMinThermique = new bool[nbPays];
    problem.DefaillanceNegativeUtiliserHydro = new bool[nbPays];
    problem.DefaillanceNegativeUtiliserConsoAbattue = new bool[nbPays];

    problem.CoefficientEcretementPMaxHydraulique = new double[nbPays];

    problem.BruitSurCoutHydraulique = new double*[nbPays];

    for (uint p = 0; p < nbPays; ++p)
        problem.BruitSurCoutHydraulique[p] = new double[8784];

    problem.NomsDesPays =  new const char*[nbPays];
    problem.PaysExtremiteDeLInterconnexion = new int[linkCount];
    problem.PaysOrigineDeLInterconnexion = new int[linkCount];
    problem.CoutDeTransport = new COUTS_DE_TRANSPORT*[linkCount];
    problem.IndexDebutIntercoOrigine = new int[nbPays];
    problem.IndexDebutIntercoExtremite = new int[nbPays];
    problem.IndexSuivantIntercoOrigine = new int[linkCount];
    problem.IndexSuivantIntercoExtremite = new int[linkCount];
    problem.NumeroDeJourDuPasDeTemps = new int[NombreDePasDeTemps];
    problem.NumeroDIntervalleOptimiseDuPasDeTemps
      = new int[NombreDePasDeTemps];
    problem.NbGrpCourbeGuide = new int[NombreDePasDeTemps];
    problem.NbGrpOpt = new int[NombreDePasDeTemps];
    problem.CoutDeDefaillancePositive = new double[nbPays];
    problem.CoutDeDefaillanceNegative = new double[nbPays];
    problem.CoutDeDefaillanceEnReserve = new double[nbPays];
    problem.NumeroDeContrainteEnergieHydraulique = new int[nbPays];
    problem.NumeroDeContrainteMinEnergieHydraulique = new int[nbPays];
    problem.NumeroDeContrainteMaxEnergieHydraulique = new int[nbPays];
    problem.NumeroDeContrainteMaxPompage = new int[nbPays];
    problem.NumeroDeContrainteDeSoldeDEchange = new int[nbPays];

    problem.NumeroDeContrainteEquivalenceStockFinal = new int[nbPays];
    problem.NumeroDeContrainteExpressionStockFinal = new int[nbPays];

    problem.NumeroDeVariableStockFinal = new int[nbPays];
    problem.NumeroDeVariableDeTrancheDeStock = new int*[nbPays];
    for (uint p = 0; p < nbPays; ++p)
        problem.NumeroDeVariableDeTrancheDeStock[p] = new int[100];

    problem.ValeursDeNTC = new VALEURS_DE_NTC_ET_RESISTANCES*[NombreDePasDeTemps];
    problem.ValeursDeNTCRef = new VALEURS_DE_NTC_ET_RESISTANCES*[NombreDePasDeTemps];
    problem.ConsommationsAbattues = new CONSOMMATIONS_ABATTUES*[NombreDePasDeTemps];
    problem.ConsommationsAbattuesRef = new CONSOMMATIONS_ABATTUES*[NombreDePasDeTemps];
    problem.AllMustRunGeneration = new ALL_MUST_RUN_GENERATION*[NombreDePasDeTemps];
    problem.SoldeMoyenHoraire = new SOLDE_MOYEN_DES_ECHANGES*[NombreDePasDeTemps];
    problem.CorrespondanceVarNativesVarOptim
      = new CORRESPONDANCES_DES_VARIABLES*[NombreDePasDeTemps];
    problem.CorrespondanceCntNativesCntOptim
      = new CORRESPONDANCES_DES_CONTRAINTES*[NombreDePasDeTemps];
    problem.VariablesDualesDesContraintesDeNTC
      = new VARIABLES_DUALES_INTERCONNEXIONS*[NombreDePasDeTemps];
    auto enabledBindingConstraints = study.bindingConstraints.enabled();
    problem.MatriceDesContraintesCouplantes
      = new CONTRAINTES_COUPLANTES*[enabledBindingConstraints.size()];
    problem.PaliersThermiquesDuPays = new PALIERS_THERMIQUES*[nbPays];
    problem.CaracteristiquesHydrauliques = new ENERGIES_ET_PUISSANCES_HYDRAULIQUES*[nbPays];
    problem.previousSimulationFinalLevel = new double[nbPays];

    problem.ShortTermStorage.resize(nbPays);

    problem.previousYearFinalLevels = nullptr;
    if (problem.hydroHotStart)
    {
        for (uint i = 0; i != nbPays; i++)
        {
            auto& area = *(study.areas[i]);
            if (area.hydro.reservoirManagement)
            {
                problem.previousYearFinalLevels = new double[nbPays];
                break;
            }
        }
    }

    problem.ReserveJMoins1 = new RESERVE_JMOINS1*[nbPays];
    problem.ResultatsHoraires.resize(nbPays);

    for (uint p = 0; p != nbPays; ++p)
    {
        problem.IndexDebutIntercoOrigine[p] = -1;
        problem.IndexDebutIntercoExtremite[p] = -1;
    }

    for (unsigned k = 0; k < NombreDePasDeTemps; k++)
    {
        problem.ValeursDeNTC[k]
          = new VALEURS_DE_NTC_ET_RESISTANCES;
        problem.ValeursDeNTCRef[k]
          = new VALEURS_DE_NTC_ET_RESISTANCES;
        problem.ConsommationsAbattues[k]
          = new CONSOMMATIONS_ABATTUES;
        problem.ConsommationsAbattuesRef[k]
          = new CONSOMMATIONS_ABATTUES;
        problem.AllMustRunGeneration[k]
          = new ALL_MUST_RUN_GENERATION;
        problem.SoldeMoyenHoraire[k]
          = new SOLDE_MOYEN_DES_ECHANGES;
        problem.CorrespondanceVarNativesVarOptim[k]
          = new CORRESPONDANCES_DES_VARIABLES;
        problem.CorrespondanceCntNativesCntOptim[k]
          = new CORRESPONDANCES_DES_CONTRAINTES;
        problem.VariablesDualesDesContraintesDeNTC[k]
          = new VARIABLES_DUALES_INTERCONNEXIONS;
        problem.ValeursDeNTC[k]->ResistanceApparente
          = new double[linkCount];
        problem.ValeursDeNTC[k]->ValeurDeNTCExtremiteVersOrigine
          = new double[linkCount];
        problem.ValeursDeNTC[k]->ValeurDeNTCOrigineVersExtremite
          = new double[linkCount];
        problem.ValeursDeNTC[k]->ValeurDeLoopFlowOrigineVersExtremite
          = new double[linkCount];
        problem.ValeursDeNTC[k]->ValeurDuFlux = new double[linkCount];
        problem.ValeursDeNTC[k]->ValeurDuFluxUp = new double[linkCount];
        problem.ValeursDeNTC[k]->ValeurDuFluxDown = new double[linkCount];
        problem.ValeursDeNTCRef[k]->ResistanceApparente
          = new double[linkCount];
        problem.ValeursDeNTCRef[k]->ValeurDeNTCExtremiteVersOrigine
          = new double[linkCount];
        problem.ValeursDeNTCRef[k]->ValeurDeLoopFlowOrigineVersExtremite
          = new double[linkCount];
        problem.ValeursDeNTCRef[k]->ValeurDeNTCOrigineVersExtremite
          = new double[linkCount];
        problem.ValeursDeNTCRef[k]->ValeurDuFlux = new double[linkCount];
        problem.ValeursDeNTCRef[k]->ValeurDuFluxUp = new double[linkCount];
        problem.ValeursDeNTCRef[k]->ValeurDuFluxDown
          = new double[linkCount];
        problem.ConsommationsAbattues[k]->ConsommationAbattueDuPays
          = new double[nbPays];
        problem.ConsommationsAbattuesRef[k]->ConsommationAbattueDuPays
          = new double[nbPays];
        problem.AllMustRunGeneration[k]->AllMustRunGenerationOfArea
          = new double[nbPays];
        problem.SoldeMoyenHoraire[k]->SoldeMoyenDuPays
          = new double[nbPays];

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDeLInterconnexion
          = new int[linkCount];
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion
          = new int[linkCount];
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion
          = new int[linkCount];

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermique
          = new int[study.runtime->thermalPlantTotalCount];
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHyd
          = new int[nbPays];
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDePompage
          = new int[nbPays];
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeNiveau
          = new int[nbPays];
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeDebordement
          = new int[nbPays];
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositive
          = new int[nbPays];
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegative
          = new int[nbPays];

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisse
          = new int[nbPays];

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausse
          = new int[nbPays];

        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique
          = new int[study.runtime->thermalPlantTotalCount];
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
          = new int[study.runtime->thermalPlantTotalCount];
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
          = new int[study.runtime->thermalPlantTotalCount];
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
          = new int[study.runtime->thermalPlantTotalCount];

        problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.InjectionVariable
          = new int[shortTermStorageCount];
        problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.WithdrawalVariable
          = new int[shortTermStorageCount];
        problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.LevelVariable
          = new int[shortTermStorageCount];

        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesBilansPays
          = new int[nbPays];
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContraintePourEviterLesChargesFictives
          = new int[nbPays];
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesNiveauxPays
          = new int[nbPays];

        problem.CorrespondanceCntNativesCntOptim[k]->ShortTermStorageLevelConstraint
          = new int[shortTermStorageCount];

        problem.CorrespondanceCntNativesCntOptim[k]->NumeroPremiereContrainteDeReserveParZone
          = new int[nbPays];
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeuxiemeContrainteDeReserveParZone
          = new int[nbPays];
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDeDissociationDeFlux
          = new int[linkCount];
        problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesContraintesCouplantes
          = new int[enabledBindingConstraints.size()];

        problem.CorrespondanceCntNativesCntOptim[k]
          ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche
          = new int[study.runtime->thermalPlantTotalCount];
        problem.CorrespondanceCntNativesCntOptim[k]
          ->NumeroDeContrainteDesContraintesDeDureeMinDArret
          = new int[study.runtime->thermalPlantTotalCount];

        problem.CorrespondanceCntNativesCntOptim[k]
          ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
          = new int[study.runtime->thermalPlantTotalCount];

        problem.VariablesDualesDesContraintesDeNTC[k]->VariableDualeParInterconnexion
          = new double[linkCount];
    }

    for (unsigned k = 0; k < linkCount; ++k)
    {
        problem.CoutDeTransport[k] = new COUTS_DE_TRANSPORT;
        problem.CoutDeTransport[k]->IntercoGereeAvecDesCouts = false;
        problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremite
          = new double[NombreDePasDeTemps];
        problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigine
          = new double[NombreDePasDeTemps];

        problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremiteRef
          = new double[NombreDePasDeTemps];
        problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigineRef
          = new double[NombreDePasDeTemps];
    }

    problem.CorrespondanceCntNativesCntOptimJournalieres
      = new CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES*[7];
    for (unsigned k = 0; k < 7; k++)
    {
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          = new CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES;
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          ->NumeroDeContrainteDesContraintesCouplantes
          = new int[enabledBindingConstraints.size()];
    }

    problem.CorrespondanceCntNativesCntOptimHebdomadaires
      = new CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES*[1];
    for (unsigned k = 0; k < 1; k++)
    {
        problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
          = new CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES;
        problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
          ->NumeroDeContrainteDesContraintesCouplantes
          = new int[enabledBindingConstraints.size()];
    }

    const auto& bindingConstraintCount = enabledBindingConstraints.size();
    problem.ResultatsContraintesCouplantes
        = new RESULTATS_CONTRAINTES_COUPLANTES[bindingConstraintCount];

    for (unsigned k = 0; k < bindingConstraintCount; k++)
    {
        problem.MatriceDesContraintesCouplantes[k] = new CONTRAINTES_COUPLANTES;

        auto enabledConstraints = study.bindingConstraints.enabled();
        assert(k < enabledConstraints.size());
        assert(enabledConstraints[k]->linkCount() < 50000000);
        assert(enabledConstraints[k]->clusterCount() < 50000000);

        problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante
          = new double[NombreDePasDeTemps];
        problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef
          = new double[NombreDePasDeTemps];

        problem.MatriceDesContraintesCouplantes[k]->NumeroDeLInterconnexion
          = new int[enabledBindingConstraints[k]->linkCount()];
        problem.MatriceDesContraintesCouplantes[k]->PoidsDeLInterconnexion
          = new double[enabledBindingConstraints[k]->linkCount()];
        problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLInterco
          = new int[enabledBindingConstraints[k]->linkCount()];

        problem.MatriceDesContraintesCouplantes[k]->NumeroDuPalierDispatch
          = new int[enabledBindingConstraints[k]->clusterCount()];
        problem.MatriceDesContraintesCouplantes[k]->PoidsDuPalierDispatch
          = new double[enabledBindingConstraints[k]->clusterCount()];
        problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLePalierDispatch
          = new int[enabledBindingConstraints[k]->clusterCount()];
        problem.MatriceDesContraintesCouplantes[k]->PaysDuPalierDispatch
          = new int[enabledBindingConstraints[k]->clusterCount()];

        // TODO : create a numberOfTimeSteps method in class of runtime->bindingConstraint
        unsigned int nbTimeSteps;
        switch (enabledBindingConstraints[k]->type())
        {
            using namespace Antares::Data;
        case BindingConstraint::typeHourly:
            nbTimeSteps = 168;
            break;
        case BindingConstraint::typeDaily:
            nbTimeSteps = 7;
            break;
        case BindingConstraint::typeWeekly:
            nbTimeSteps = 1;
            break;
        default:
            nbTimeSteps = 0;
            break;
        }
        if (nbTimeSteps > 0)
        {
            problem.ResultatsContraintesCouplantes[k].variablesDuales
              = new double[nbTimeSteps];
        }
        else
        {
            problem.ResultatsContraintesCouplantes[k].variablesDuales = nullptr;
        }
    }

    for (unsigned k = 0; k < nbPays; k++)
    {
        const uint nbPaliers = study.areas.byIndex[k]->thermal.list.size();

        problem.PaliersThermiquesDuPays[k] = new PALIERS_THERMIQUES;
        problem.CaracteristiquesHydrauliques[k] =  new ENERGIES_ET_PUISSANCES_HYDRAULIQUES;

        problem.ReserveJMoins1[k] = new RESERVE_JMOINS1;
        problem.PaliersThermiquesDuPays[k]->minUpDownTime = new int[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUneHeure
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUnJour
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->TailleUnitaireDUnGroupeDuPalierThermique
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->NumeroDuPalierDansLEnsembleDesPaliersThermiques
          = new int[nbPaliers];

        problem.PaliersThermiquesDuPays[k]->CoutDeDemarrageDUnGroupeDuPalierThermique
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->CoutDArretDUnGroupeDuPalierThermique
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->CoutFixeDeMarcheDUnGroupeDuPalierThermique
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->pminDUnGroupeDuPalierThermique
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->PmaxDUnGroupeDuPalierThermique
          = new double[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
          = new int[nbPaliers];
        problem.PaliersThermiquesDuPays[k]->DureeMinimaleDArretDUnGroupeDuPalierThermique
          = new int[nbPaliers];

        problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise
          = new double[7];
        problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParJour
          = new double[7];
        problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimiseRef
          = new double[7];
        problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire
          = new double[NombreDePasDeTemps];
        problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraireRef
          = new double[NombreDePasDeTemps];

        problem.CaracteristiquesHydrauliques[k]->MaxEnergieHydrauParIntervalleOptimise
          = new double[7];
        problem.CaracteristiquesHydrauliques[k]->MinEnergieHydrauParIntervalleOptimise
          = new double[7];

        problem.CaracteristiquesHydrauliques[k]->NiveauHoraireSup
          = new double[NombreDePasDeTemps];
        problem.CaracteristiquesHydrauliques[k]->NiveauHoraireInf
          = new double[NombreDePasDeTemps];
        problem.CaracteristiquesHydrauliques[k]->ApportNaturelHoraire
          = new double[NombreDePasDeTemps]();
        problem.CaracteristiquesHydrauliques[k]->MingenHoraire
          = new double[NombreDePasDeTemps];

        problem.CaracteristiquesHydrauliques[k]->WaterLayerValues
          = new double[100];
        problem.CaracteristiquesHydrauliques[k]->InflowForTimeInterval
          = new double[100];

        problem.CaracteristiquesHydrauliques[k]->MaxEnergiePompageParIntervalleOptimise
          = new double[7];
        problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxPompageHoraire
          = new double[NombreDePasDeTemps];

        problem.ReserveJMoins1[k]->ReserveHoraireJMoins1
          = new double[NombreDePasDeTemps];
        problem.ReserveJMoins1[k]->ReserveHoraireJMoins1Ref
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositive
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDENS
          = new double[NombreDePasDeTemps](); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesLmrViolations
          = new int[NombreDePasDeTemps](); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesSpilledEnergyAfterCSR
          = new double[NombreDePasDeTemps](); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesDtgMrgCsr
          = new double[NombreDePasDeTemps](); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveUp
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveDown
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveAny
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegative
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeUp
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeDown
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeAny
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceEnReserve
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].TurbinageHoraire
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].PompageHoraire
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].TurbinageHoraireUp
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].TurbinageHoraireDown
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].CoutsMarginauxHoraires
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].niveauxHoraires
          = new double[NombreDePasDeTemps]();
        problem.ResultatsHoraires[k].valeurH2oHoraire
          = new double[NombreDePasDeTemps];
        problem.ResultatsHoraires[k].debordementsHoraires
          = new double[NombreDePasDeTemps];
        problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout
          = new PDISP_ET_COUTS_HORAIRES_PAR_PALIER*[nbPaliers];
        problem.ResultatsHoraires[k].ProductionThermique
          = new PRODUCTION_THERMIQUE_OPTIMALE*[NombreDePasDeTemps];

        for (unsigned j = 0; j < nbPaliers; ++j)
        {
            problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[j]
              = new PDISP_ET_COUTS_HORAIRES_PAR_PALIER;

            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDeProductionDuPalierThermique
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDeProductionDuPalierThermiqueRef
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceDisponibleDuPalierThermique
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceDisponibleDuPalierThermiqueRef
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceDisponibleDuPalierThermiqueRef_SV
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceMinDuPalierThermique
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->PuissanceMinDuPalierThermique_SV
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->NombreMaxDeGroupesEnMarcheDuPalierThermique
              = new int[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->NombreMinDeGroupesEnMarcheDuPalierThermique
              = new int[NombreDePasDeTemps];

            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDuPalierThermiqueUp
              = new double[NombreDePasDeTemps];
            problem.PaliersThermiquesDuPays[k]
              ->PuissanceDisponibleEtCout[j]
              ->CoutHoraireDuPalierThermiqueDown
              = new double[NombreDePasDeTemps];
        }
        for (unsigned j = 0; j < NombreDePasDeTemps; j++)
        {
            problem.ResultatsHoraires[k].ProductionThermique[j] = new PRODUCTION_THERMIQUE_OPTIMALE;
            problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalier
              = new double[nbPaliers];
            problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalierUp
              = new double[nbPaliers];
            problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalierDown
              = new double[nbPaliers];
            problem.ResultatsHoraires[k].ProductionThermique[j]->NombreDeGroupesEnMarcheDuPalier
              = new double[nbPaliers];
            problem.ResultatsHoraires[k].ProductionThermique[j]->NombreDeGroupesQuiDemarrentDuPalier
              = new double[nbPaliers];
            problem.ResultatsHoraires[k].ProductionThermique[j]->NombreDeGroupesQuiSArretentDuPalier
              = new double[nbPaliers];
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              ->NombreDeGroupesQuiTombentEnPanneDuPalier
              = new double[nbPaliers];
        }
        // Short term storage results
        const unsigned long nbShortTermStorage = study.areas.byIndex[k]->shortTermStorage.count();
        problem.ResultatsHoraires[k].ShortTermStorage.resize(NombreDePasDeTemps);
        for (uint pdt = 0; pdt < NombreDePasDeTemps; pdt++)
        {
            problem.ResultatsHoraires[k].ShortTermStorage[pdt].injection.resize(nbShortTermStorage);
            problem.ResultatsHoraires[k].ShortTermStorage[pdt].withdrawal.resize(
              nbShortTermStorage);
            problem.ResultatsHoraires[k].ShortTermStorage[pdt].level.resize(nbShortTermStorage);
        }
    }

    problem.coutOptimalSolution1 = new double[7];
    problem.coutOptimalSolution2 = new double[7];

    problem.tempsResolution1 = new double[7];
    problem.tempsResolution2 = new double[7];
}

void SIM_DesallocationProblemeHebdo(PROBLEME_HEBDO& problem)
{
    auto& study = *Data::Study::Current::Get();

    uint nbPays = study.areas.size();

    delete[] problem.NomsDesPays;
    delete[] problem.PaysExtremiteDeLInterconnexion;
    delete[] problem.PaysOrigineDeLInterconnexion;
    delete[] problem.IndexDebutIntercoOrigine;
    delete[] problem.IndexDebutIntercoExtremite;
    delete[] problem.IndexSuivantIntercoOrigine;
    delete[] problem.IndexSuivantIntercoExtremite;
    delete[] problem.NumeroDeJourDuPasDeTemps;
    delete[] problem.NumeroDIntervalleOptimiseDuPasDeTemps;
    delete[] problem.NbGrpCourbeGuide;
    delete[] problem.NbGrpOpt;

    for (unsigned k = 0; k < problem.NombreDePasDeTemps; k++)
    {
        delete[] problem.ValeursDeNTC[k]->ResistanceApparente;
        delete[] problem.ValeursDeNTC[k]->ValeurDeNTCExtremiteVersOrigine;
        delete[] problem.ValeursDeNTC[k]->ValeurDeNTCOrigineVersExtremite;
        delete[] problem.ValeursDeNTC[k]->ValeurDeLoopFlowOrigineVersExtremite;
        delete[] problem.ValeursDeNTC[k]->ValeurDuFlux;
        delete[] problem.ValeursDeNTC[k]->ValeurDuFluxUp;
        delete[] problem.ValeursDeNTC[k]->ValeurDuFluxDown;
        delete problem.ValeursDeNTC[k];
        delete[] problem.ConsommationsAbattues[k]->ConsommationAbattueDuPays;
        delete problem.ConsommationsAbattues[k];
        delete[] problem.ValeursDeNTCRef[k]->ResistanceApparente;
        delete[] problem.ValeursDeNTCRef[k]->ValeurDeNTCExtremiteVersOrigine;
        delete[] problem.ValeursDeNTCRef[k]->ValeurDeNTCOrigineVersExtremite;
        delete[] problem.ValeursDeNTCRef[k]->ValeurDeLoopFlowOrigineVersExtremite;
        delete[] problem.ValeursDeNTCRef[k]->ValeurDuFlux;
        delete[] problem.ValeursDeNTCRef[k]->ValeurDuFluxUp;
        delete[] problem.ValeursDeNTCRef[k]->ValeurDuFluxDown;
        delete problem.ValeursDeNTCRef[k];
        delete[] problem.ConsommationsAbattuesRef[k]->ConsommationAbattueDuPays;
        delete problem.ConsommationsAbattuesRef[k];
        delete[] problem.AllMustRunGeneration[k]->AllMustRunGenerationOfArea;
        delete problem.AllMustRunGeneration[k];
        delete[] problem.SoldeMoyenHoraire[k]->SoldeMoyenDuPays;
        delete problem.SoldeMoyenHoraire[k];
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDeLInterconnexion;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermique;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHyd;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDePompage;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeNiveau;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeDebordement;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositive;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegative;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisse;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausse;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique;

        delete[] problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.InjectionVariable;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.WithdrawalVariable;
        delete[] problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.LevelVariable;

        delete problem.CorrespondanceVarNativesVarOptim[k];

        delete[] problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesBilansPays;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesNiveauxPays;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeContraintePourEviterLesChargesFictives;

        delete[] problem.CorrespondanceCntNativesCntOptim[k]->NumeroPremiereContrainteDeReserveParZone;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeuxiemeContrainteDeReserveParZone;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDeDissociationDeFlux;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesContraintesCouplantes;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeContrainteDesContraintesDeDureeMinDeMarche;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeContrainteDesContraintesDeDureeMinDArret;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]
                  ->NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne;
        delete[] problem.CorrespondanceCntNativesCntOptim[k]->ShortTermStorageLevelConstraint;

        delete problem.CorrespondanceCntNativesCntOptim[k];
        delete[] problem.VariablesDualesDesContraintesDeNTC[k]->VariableDualeParInterconnexion;
        delete problem.VariablesDualesDesContraintesDeNTC[k];
    }
    delete[] problem.ValeursDeNTC;
    delete[] problem.ConsommationsAbattues;
    delete[] problem.ValeursDeNTCRef;
    delete[] problem.ConsommationsAbattuesRef;
    delete[] problem.AllMustRunGeneration;
    delete[] problem.SoldeMoyenHoraire;
    delete[] problem.CorrespondanceVarNativesVarOptim;
    delete[] problem.CorrespondanceCntNativesCntOptim;
    delete[] problem.VariablesDualesDesContraintesDeNTC;

    for (int k = 0; k < (int)study.runtime->interconnectionsCount(); k++)
    {
        delete[] problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremite;
        delete[] problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigine;

        delete[] problem.CoutDeTransport[k]->CoutDeTransportOrigineVersExtremiteRef;
        delete[] problem.CoutDeTransport[k]->CoutDeTransportExtremiteVersOrigineRef;
        delete problem.CoutDeTransport[k];
    }
    delete[] problem.CoutDeTransport;

    for (int k = 0; k < 7; k++)
    {
        delete[] problem.CorrespondanceCntNativesCntOptimJournalieres[k]
                  ->NumeroDeContrainteDesContraintesCouplantes;
        delete problem.CorrespondanceCntNativesCntOptimJournalieres[k];
    }
    delete[] problem.CorrespondanceCntNativesCntOptimJournalieres;

    for (int k = 0; k < 1; k++)
    {
        delete[] problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
                  ->NumeroDeContrainteDesContraintesCouplantes;
        delete problem.CorrespondanceCntNativesCntOptimHebdomadaires[k];
    }
    delete[] problem.CorrespondanceCntNativesCntOptimHebdomadaires;

    auto enabledBindingConstraints = study.bindingConstraints.enabled();
    for (int k = 0; k < (int)enabledBindingConstraints.size(); k++)
    {
        delete[] problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante;
        delete[] problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef;

        delete[] problem.MatriceDesContraintesCouplantes[k]->NumeroDeLInterconnexion;
        delete[] problem.MatriceDesContraintesCouplantes[k]->PoidsDeLInterconnexion;
        delete[] problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLInterco;

        delete[] problem.MatriceDesContraintesCouplantes[k]->PoidsDuPalierDispatch;
        delete[] problem.MatriceDesContraintesCouplantes[k]->PaysDuPalierDispatch;
        delete[] problem.MatriceDesContraintesCouplantes[k]->NumeroDuPalierDispatch;
        delete[] problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLePalierDispatch;

        delete[] problem.MatriceDesContraintesCouplantes[k];

        if (problem.ResultatsContraintesCouplantes[k].variablesDuales != nullptr)
            delete[] problem.ResultatsContraintesCouplantes[k].variablesDuales;
    }
    delete[] problem.MatriceDesContraintesCouplantes;
    delete[] problem.ResultatsContraintesCouplantes;

    for (int k = 0; k < (int)nbPays; ++k)
    {
        const uint nbPaliers = study.areas.byIndex[k]->thermal.list.size();

        delete[] problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUneHeure;
        delete[] problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUnJour;
        delete[] problem.PaliersThermiquesDuPays[k]->minUpDownTime;
        delete[] problem.PaliersThermiquesDuPays[k]->TailleUnitaireDUnGroupeDuPalierThermique;
        delete[] problem.PaliersThermiquesDuPays[k]->NumeroDuPalierDansLEnsembleDesPaliersThermiques;

        delete[] problem.PaliersThermiquesDuPays[k]->CoutDeDemarrageDUnGroupeDuPalierThermique;
        delete[] problem.PaliersThermiquesDuPays[k]->CoutDArretDUnGroupeDuPalierThermique;
        delete[] problem.PaliersThermiquesDuPays[k]->CoutFixeDeMarcheDUnGroupeDuPalierThermique;
        delete[] problem.PaliersThermiquesDuPays[k]->pminDUnGroupeDuPalierThermique;
        delete[] problem.PaliersThermiquesDuPays[k]->PmaxDUnGroupeDuPalierThermique;
        delete[] problem.PaliersThermiquesDuPays[k]->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique;
        delete[] problem.PaliersThermiquesDuPays[k]->DureeMinimaleDArretDUnGroupeDuPalierThermique;

        delete[] problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise;
        delete[] problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParJour;
        delete[] problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimiseRef;

        delete[] problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire;
        delete[] problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraireRef;

        delete[] problem.CaracteristiquesHydrauliques[k]->MaxEnergieHydrauParIntervalleOptimise;
        delete[] problem.CaracteristiquesHydrauliques[k]->MinEnergieHydrauParIntervalleOptimise;

        delete[] problem.CaracteristiquesHydrauliques[k]->MaxEnergiePompageParIntervalleOptimise;
        delete[] problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxPompageHoraire;

        delete[] problem.CaracteristiquesHydrauliques[k]->NiveauHoraireSup;
        delete[] problem.CaracteristiquesHydrauliques[k]->NiveauHoraireInf;
        delete[] problem.CaracteristiquesHydrauliques[k]->ApportNaturelHoraire;
        delete[] problem.CaracteristiquesHydrauliques[k]->MingenHoraire;

        delete[] problem.CaracteristiquesHydrauliques[k]->WaterLayerValues;
        delete[] problem.CaracteristiquesHydrauliques[k]->InflowForTimeInterval;
        delete problem.CaracteristiquesHydrauliques[k];

        delete[] problem.ReserveJMoins1[k]->ReserveHoraireJMoins1;
        delete[] problem.ReserveJMoins1[k]->ReserveHoraireJMoins1Ref;
        delete problem.ReserveJMoins1[k];

        for (int j = 0; j < (int)nbPaliers; j++)
        {
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDeProductionDuPalierThermique;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDeProductionDuPalierThermiqueRef;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceDisponibleDuPalierThermique;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceDisponibleDuPalierThermiqueRef;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceDisponibleDuPalierThermiqueRef_SV;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceMinDuPalierThermique;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->PuissanceMinDuPalierThermique_SV;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->NombreMaxDeGroupesEnMarcheDuPalierThermique;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->NombreMinDeGroupesEnMarcheDuPalierThermique;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDuPalierThermiqueUp;
            delete[] problem.PaliersThermiquesDuPays[k]
                      ->PuissanceDisponibleEtCout[j]
                      ->CoutHoraireDuPalierThermiqueDown;
            delete problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[j];
        }
        delete[] problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout;
        delete problem.PaliersThermiquesDuPays[k];

        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositive;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDENS;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesLmrViolations;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesSpilledEnergyAfterCSR;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDtgMrgCsr;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveUp;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveDown;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveAny;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegative;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeUp;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeDown;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeAny;
        delete[] problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceEnReserve;
        delete[] problem.ResultatsHoraires[k].TurbinageHoraire;
        delete[] problem.ResultatsHoraires[k].PompageHoraire;
        delete[] problem.ResultatsHoraires[k].TurbinageHoraireUp;
        delete[] problem.ResultatsHoraires[k].TurbinageHoraireDown;
        delete[] problem.ResultatsHoraires[k].niveauxHoraires;
        delete[] problem.ResultatsHoraires[k].valeurH2oHoraire;
        delete[] problem.ResultatsHoraires[k].debordementsHoraires;
        delete[] problem.ResultatsHoraires[k].CoutsMarginauxHoraires;

        for (uint j = 0; j < problem.NombreDePasDeTemps; j++)
        {
            delete[] problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalier;
            delete[] problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalierUp;
            delete[] problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalierDown;
            delete[] problem.ResultatsHoraires[k].ProductionThermique[j]->NombreDeGroupesEnMarcheDuPalier;
            delete[] problem.ResultatsHoraires[k]
                      .ProductionThermique[j]
                      ->NombreDeGroupesQuiDemarrentDuPalier;
            delete[] problem.ResultatsHoraires[k]
                      .ProductionThermique[j]
                      ->NombreDeGroupesQuiSArretentDuPalier;
            delete[] problem.ResultatsHoraires[k]
                      .ProductionThermique[j]
                      ->NombreDeGroupesQuiTombentEnPanneDuPalier;
            delete problem.ResultatsHoraires[k].ProductionThermique[j];
        }
        delete[] problem.ResultatsHoraires[k].ProductionThermique;
        delete[] problem.BruitSurCoutHydraulique[k];
    }
    delete[] problem.PaliersThermiquesDuPays;
    delete[] problem.CaracteristiquesHydrauliques;
    delete[] problem.previousSimulationFinalLevel;
    if (problem.previousYearFinalLevels)
        delete[] problem.previousYearFinalLevels;

    delete[] problem.ReserveJMoins1;

    delete[] problem.CoutDeDefaillancePositive;
    delete[] problem.CoutDeDefaillanceNegative;
    delete[] problem.CoutDeDefaillanceEnReserve;
    delete[] problem.NumeroDeContrainteEnergieHydraulique;
    delete[] problem.NumeroDeContrainteMinEnergieHydraulique;
    delete[] problem.NumeroDeContrainteMaxEnergieHydraulique;
    delete[] problem.NumeroDeContrainteMaxPompage;
    delete[] problem.NumeroDeContrainteDeSoldeDEchange;

    delete[] problem.NumeroDeContrainteEquivalenceStockFinal;
    delete[] problem.NumeroDeContrainteExpressionStockFinal;
    delete[] problem.NumeroDeVariableStockFinal;
    for (uint p = 0; p < nbPays; ++p)
        delete[] problem.NumeroDeVariableDeTrancheDeStock[p];
    delete[] problem.NumeroDeVariableDeTrancheDeStock;

    delete[] problem.DefaillanceNegativeUtiliserConsoAbattue;
    delete[] problem.DefaillanceNegativeUtiliserHydro;
    delete[] problem.DefaillanceNegativeUtiliserPMinThermique;

    delete[] problem.CoefficientEcretementPMaxHydraulique;

    delete[] problem.BruitSurCoutHydraulique;

    delete[] problem.coutOptimalSolution1;
    delete[] problem.coutOptimalSolution2;
    delete[] problem.tempsResolution1;
    delete[] problem.tempsResolution2;
}
