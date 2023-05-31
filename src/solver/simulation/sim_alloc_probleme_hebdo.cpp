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

void SIM_AllocationProblemeHebdo(PROBLEME_HEBDO& problem, uint NombreDePasDeTemps)
{
    auto& study = *Data::Study::Current::Get();

    uint nbPays = study.areas.size();

    const uint linkCount = study.runtime->interconnectionsCount();
    const uint shortTermStorageCount = study.runtime->shortTermStorageCount;

    problem.DefaillanceNegativeUtiliserPMinThermique.assign(nbPays, false);
    problem.DefaillanceNegativeUtiliserHydro.assign(nbPays, false);
    problem.DefaillanceNegativeUtiliserConsoAbattue.assign(nbPays, false);

    problem.CoefficientEcretementPMaxHydraulique.assign(nbPays, 0.);

    problem.BruitSurCoutHydraulique.assign(nbPays, std::vector<double>(8784));

    problem.NomsDesPays = std::vector<const char*>(nbPays);

    problem.PaysExtremiteDeLInterconnexion.assign(linkCount, 0);
    problem.PaysOrigineDeLInterconnexion.assign(linkCount, 0);

    problem.CoutDeTransport = std::vector<COUTS_DE_TRANSPORT>(linkCount);

    // was previously set to -1 with a loop, now use assign
    problem.IndexDebutIntercoOrigine.assign(nbPays, -1);
    problem.IndexDebutIntercoExtremite.assign(nbPays, -1);

    problem.IndexSuivantIntercoOrigine.assign(linkCount, 0);
    problem.IndexSuivantIntercoExtremite.assign(linkCount, 0);

    problem.NumeroDeJourDuPasDeTemps.assign(NombreDePasDeTemps, 0);
    problem.NumeroDIntervalleOptimiseDuPasDeTemps.assign(NombreDePasDeTemps, 0);
    problem.NbGrpCourbeGuide.assign(NombreDePasDeTemps, 0);
    problem.NbGrpOpt.assign(NombreDePasDeTemps, 0);

    problem.CoutDeDefaillancePositive.assign(nbPays, 0);
    problem.CoutDeDefaillanceNegative.assign(nbPays, 0);
    problem.CoutDeDefaillanceEnReserve.assign(nbPays, 0);

    problem.NumeroDeContrainteEnergieHydraulique.assign(nbPays, 0);
    problem.NumeroDeContrainteMinEnergieHydraulique.assign(nbPays, 0);
    problem.NumeroDeContrainteMaxEnergieHydraulique.assign(nbPays, 0);
    problem.NumeroDeContrainteMaxPompage.assign(nbPays, 0);
    problem.NumeroDeContrainteDeSoldeDEchange.assign(nbPays, 0);

    problem.NumeroDeContrainteEquivalenceStockFinal.assign(nbPays, 0);
    problem.NumeroDeContrainteExpressionStockFinal.assign(nbPays, 0);

    problem.NumeroDeVariableStockFinal.assign(nbPays, 0);
    problem.NumeroDeVariableDeTrancheDeStock.assign(nbPays, std::vector<int>(100));

    problem.ValeursDeNTC = std::vector<VALEURS_DE_NTC_ET_RESISTANCES>(NombreDePasDeTemps);
    problem.ValeursDeNTCRef = std::vector<VALEURS_DE_NTC_ET_RESISTANCES>(NombreDePasDeTemps);

    problem.ConsommationsAbattues = std::vector<CONSOMMATIONS_ABATTUES>(NombreDePasDeTemps);
    problem.ConsommationsAbattuesRef = std::vector<CONSOMMATIONS_ABATTUES>(NombreDePasDeTemps);

    problem.AllMustRunGeneration = std::vector<ALL_MUST_RUN_GENERATION>(NombreDePasDeTemps);
    problem.SoldeMoyenHoraire = new SOLDE_MOYEN_DES_ECHANGES*[NombreDePasDeTemps];
    problem.CorrespondanceVarNativesVarOptim
      = new CORRESPONDANCES_DES_VARIABLES*[NombreDePasDeTemps];
    problem.CorrespondanceCntNativesCntOptim
      = new CORRESPONDANCES_DES_CONTRAINTES*[NombreDePasDeTemps];
    problem.VariablesDualesDesContraintesDeNTC
      = new VARIABLES_DUALES_INTERCONNEXIONS*[NombreDePasDeTemps];
    problem.MatriceDesContraintesCouplantes
      = new CONTRAINTES_COUPLANTES*[study.runtime->bindingConstraintCount];
    problem.PaliersThermiquesDuPays = new PALIERS_THERMIQUES*[nbPays];
    problem.CaracteristiquesHydrauliques = new ENERGIES_ET_PUISSANCES_HYDRAULIQUES*[nbPays];
    problem.previousSimulationFinalLevel.assign(nbPays, 0.);

    problem.ShortTermStorage.resize(nbPays);

    problem.previousYearFinalLevels.resize(0);
    if (problem.hydroHotStart)
    {
        for (uint i = 0; i <= nbPays; i++)
        {
            auto& area = *(study.areas[i]);
            if (area.hydro.reservoirManagement)
            {
                problem.previousYearFinalLevels.assign(nbPays, 0.);
                break;
            }
        }
    }

    problem.CoutsMarginauxDesContraintesDeReserveParZone
      = new COUTS_MARGINAUX_ZONES_DE_RESERVE*[nbPays];

    problem.ReserveJMoins1 = new RESERVE_JMOINS1*[nbPays];
    problem.ResultatsHoraires.resize(nbPays);

    for (uint k = 0; k < NombreDePasDeTemps; k++)
    {
        problem.ValeursDeNTC[k].ValeurDeNTCOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDeNTCExtremiteVersOrigine.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDeLoopFlowOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDuFlux.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDuFluxUp.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDuFluxDown.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ResistanceApparente.assign(linkCount, 0.);

        problem.ValeursDeNTCRef[k].ValeurDeNTCOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTCRef[k].ValeurDeNTCExtremiteVersOrigine.assign(linkCount, 0.);
        problem.ValeursDeNTCRef[k].ValeurDeLoopFlowOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTCRef[k].ValeurDuFlux.assign(linkCount, 0.);
        problem.ValeursDeNTCRef[k].ValeurDuFluxUp.assign(linkCount, 0.);
        problem.ValeursDeNTCRef[k].ValeurDuFluxDown.assign(linkCount, 0.);
        problem.ValeursDeNTCRef[k].ResistanceApparente.assign(linkCount, 0.);

        problem.SoldeMoyenHoraire[k]
          = new SOLDE_MOYEN_DES_ECHANGES;
        problem.CorrespondanceVarNativesVarOptim[k]
          = new CORRESPONDANCES_DES_VARIABLES;
        problem.CorrespondanceCntNativesCntOptim[k]
          = new CORRESPONDANCES_DES_CONTRAINTES;
        problem.VariablesDualesDesContraintesDeNTC[k]
          = new VARIABLES_DUALES_INTERCONNEXIONS;

        problem.ConsommationsAbattues[k].ConsommationAbattueDuPays.assign(nbPays, 0.);
        problem.ConsommationsAbattuesRef[k].ConsommationAbattueDuPays.assign(nbPays, 0.);

        problem.AllMustRunGeneration[k].AllMustRunGenerationOfArea.assign(nbPays, 0.);

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
          = new int[study.runtime->bindingConstraintCount];

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

    for (uint k = 0; k < linkCount; ++k)
    {
        problem.CoutDeTransport[k].IntercoGereeAvecDesCouts = false;
        problem.CoutDeTransport[k].CoutDeTransportOrigineVersExtremite
            .assign(NombreDePasDeTemps, 0.);
        problem.CoutDeTransport[k].CoutDeTransportExtremiteVersOrigine
            .assign(NombreDePasDeTemps, 0.);
        problem.CoutDeTransport[k].CoutDeTransportOrigineVersExtremiteRef
            .assign(NombreDePasDeTemps, 0.);
        problem.CoutDeTransport[k].CoutDeTransportExtremiteVersOrigineRef
            .assign(NombreDePasDeTemps, 0.);
    }

    problem.CorrespondanceCntNativesCntOptimJournalieres
      = new CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES*[7];
    for (uint k = 0; k < 7; k++)
    {
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          = new CORRESPONDANCES_DES_CONTRAINTES_JOURNALIERES;
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          ->NumeroDeContrainteDesContraintesCouplantes
          = new int[study.runtime->bindingConstraintCount];
    }

    problem.CorrespondanceCntNativesCntOptimHebdomadaires
      = new CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES*[1];
    for (uint k = 0; k < 1; k++)
    {
        problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
          = new CORRESPONDANCES_DES_CONTRAINTES_HEBDOMADAIRES;
        problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
          ->NumeroDeContrainteDesContraintesCouplantes
          = new int[study.runtime->bindingConstraintCount];
    }

    const auto& bindingConstraintCount = study.runtime->bindingConstraintCount;
    problem.ResultatsContraintesCouplantes
        = new RESULTATS_CONTRAINTES_COUPLANTES[bindingConstraintCount];

    for (uint k = 0; k < bindingConstraintCount; k++)
    {
        problem.MatriceDesContraintesCouplantes[k] = new CONTRAINTES_COUPLANTES;

        assert(k < study.runtime->bindingConstraintCount);
        assert(study.runtime->bindingConstraint[k].linkCount < 50000000);
        assert(study.runtime->bindingConstraint[k].clusterCount < 50000000);

        problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante
          = new double[NombreDePasDeTemps];
        problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef
          = new double[NombreDePasDeTemps];

        problem.MatriceDesContraintesCouplantes[k]->NumeroDeLInterconnexion
          = new int[study.runtime->bindingConstraint[k].linkCount];
        problem.MatriceDesContraintesCouplantes[k]->PoidsDeLInterconnexion
          = new double[study.runtime->bindingConstraint[k].linkCount];
        problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLInterco
          = new int[study.runtime->bindingConstraint[k].linkCount];

        problem.MatriceDesContraintesCouplantes[k]->NumeroDuPalierDispatch
          = new int[study.runtime->bindingConstraint[k].clusterCount];
        problem.MatriceDesContraintesCouplantes[k]->PoidsDuPalierDispatch
          = new double[study.runtime->bindingConstraint[k].clusterCount];
        problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLePalierDispatch
          = new int[study.runtime->bindingConstraint[k].clusterCount];
        problem.MatriceDesContraintesCouplantes[k]->PaysDuPalierDispatch
          = new int[study.runtime->bindingConstraint[k].clusterCount];

        // TODO : create a numberOfTimeSteps method in class of runtime->bindingConstraint
        unsigned int nbTimeSteps;
        switch (study.runtime->bindingConstraint[k].type)
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

    for (uint k = 0; k < nbPays; k++)
    {
        const uint nbPaliers = study.areas.byIndex[k]->thermal.list.size();

        problem.PaliersThermiquesDuPays[k] = new PALIERS_THERMIQUES;
        problem.CaracteristiquesHydrauliques[k] =  new ENERGIES_ET_PUISSANCES_HYDRAULIQUES;

        problem.CoutsMarginauxDesContraintesDeReserveParZone[k]
          = new COUTS_MARGINAUX_ZONES_DE_RESERVE;
        problem.CoutsMarginauxDesContraintesDeReserveParZone[k]
          ->CoutsMarginauxHorairesDeLaReserveParZone
          = new double[NombreDePasDeTemps];

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
          = new double[NombreDePasDeTemps]; // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesDtgMrgCsr
          = new double[NombreDePasDeTemps]; // adq patch
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

        for (uint j = 0; j < nbPaliers; ++j)
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
        for (uint j = 0; j < NombreDePasDeTemps; j++)
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

    for (uint k = 0; k < problem.NombreDePasDeTemps; k++)
    {
        delete problem.SoldeMoyenHoraire[k]->SoldeMoyenDuPays;
        delete problem.SoldeMoyenHoraire[k];
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDeLInterconnexion;
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion);
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermique;
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHyd;
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDePompage;
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeNiveau;
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeDebordement;
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositive;
        delete problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegative;
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisse);
        MemFree(
          problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausse);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique);
        MemFree(problem.CorrespondanceVarNativesVarOptim[k]
                  ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique);

        delete problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.InjectionVariable;
        delete problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.WithdrawalVariable;
        delete problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.LevelVariable;

        delete problem.CorrespondanceVarNativesVarOptim[k];
        delete problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesBilansPays;
        delete problem.CorrespondanceCntNativesCntOptim[k]->NumeroDeContrainteDesNiveauxPays;
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
        delete problem.CorrespondanceCntNativesCntOptim[k]->ShortTermStorageLevelConstraint;

        delete problem.CorrespondanceCntNativesCntOptim[k];
        delete problem.VariablesDualesDesContraintesDeNTC[k]->VariableDualeParInterconnexion;
        delete problem.VariablesDualesDesContraintesDeNTC[k];
    }
    delete problem.SoldeMoyenHoraire;
    delete problem.CorrespondanceVarNativesVarOptim;
    delete problem.CorrespondanceCntNativesCntOptim;
    delete problem.VariablesDualesDesContraintesDeNTC;

    for (int k = 0; k < 7; k++)
    {
        MemFree(problem.CorrespondanceCntNativesCntOptimJournalieres[k]
                  ->NumeroDeContrainteDesContraintesCouplantes);
        delete problem.CorrespondanceCntNativesCntOptimJournalieres[k];
    }
    delete problem.CorrespondanceCntNativesCntOptimJournalieres;

    for (int k = 0; k < 1; k++)
    {
        MemFree(problem.CorrespondanceCntNativesCntOptimHebdomadaires[k]
                  ->NumeroDeContrainteDesContraintesCouplantes);
        delete problem.CorrespondanceCntNativesCntOptimHebdomadaires[k];
    }
    delete problem.CorrespondanceCntNativesCntOptimHebdomadaires;

    for (int k = 0; k < (int)study.runtime->bindingConstraintCount; k++)
    {
        delete problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante;
        delete problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplanteRef;

        delete problem.MatriceDesContraintesCouplantes[k]->NumeroDeLInterconnexion;
        delete problem.MatriceDesContraintesCouplantes[k]->PoidsDeLInterconnexion;
        delete problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLInterco;

        delete problem.MatriceDesContraintesCouplantes[k]->PoidsDuPalierDispatch;
        delete problem.MatriceDesContraintesCouplantes[k]->PaysDuPalierDispatch;
        delete problem.MatriceDesContraintesCouplantes[k]->NumeroDuPalierDispatch;
        delete problem.MatriceDesContraintesCouplantes[k]->OffsetTemporelSurLePalierDispatch;

        delete problem.MatriceDesContraintesCouplantes[k];

        if (problem.ResultatsContraintesCouplantes[k].variablesDuales != nullptr)
            delete problem.ResultatsContraintesCouplantes[k].variablesDuales;
    }
    delete problem.MatriceDesContraintesCouplantes;
    delete problem.ResultatsContraintesCouplantes;

    for (int k = 0; k < (int)nbPays; ++k)
    {
        const uint nbPaliers = study.areas.byIndex[k]->thermal.list.size();

        delete problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUneHeure;
        delete problem.PaliersThermiquesDuPays[k]->PminDuPalierThermiquePendantUnJour;
        delete problem.PaliersThermiquesDuPays[k]->minUpDownTime;
        delete problem.PaliersThermiquesDuPays[k]->TailleUnitaireDUnGroupeDuPalierThermique;
        MemFree(
          problem.PaliersThermiquesDuPays[k]->NumeroDuPalierDansLEnsembleDesPaliersThermiques);

        delete problem.PaliersThermiquesDuPays[k]->CoutDeDemarrageDUnGroupeDuPalierThermique;
        delete problem.PaliersThermiquesDuPays[k]->CoutDArretDUnGroupeDuPalierThermique;
        delete problem.PaliersThermiquesDuPays[k]->CoutFixeDeMarcheDUnGroupeDuPalierThermique;
        delete problem.PaliersThermiquesDuPays[k]->pminDUnGroupeDuPalierThermique;
        delete problem.PaliersThermiquesDuPays[k]->PmaxDUnGroupeDuPalierThermique;
        MemFree(
          problem.PaliersThermiquesDuPays[k]->DureeMinimaleDeMarcheDUnGroupeDuPalierThermique);
        delete problem.PaliersThermiquesDuPays[k]->DureeMinimaleDArretDUnGroupeDuPalierThermique;

        delete problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise;
        delete problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParJour;
        delete problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimiseRef;

        delete problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire;
        delete problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraireRef;

        delete problem.CaracteristiquesHydrauliques[k]->MaxEnergieHydrauParIntervalleOptimise;
        delete problem.CaracteristiquesHydrauliques[k]->MinEnergieHydrauParIntervalleOptimise;

        delete problem.CaracteristiquesHydrauliques[k]->MaxEnergiePompageParIntervalleOptimise;
        delete problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxPompageHoraire;

        delete problem.CaracteristiquesHydrauliques[k]->NiveauHoraireSup;
        delete problem.CaracteristiquesHydrauliques[k]->NiveauHoraireInf;
        delete problem.CaracteristiquesHydrauliques[k]->ApportNaturelHoraire;
        delete problem.CaracteristiquesHydrauliques[k]->MingenHoraire;

        delete problem.CaracteristiquesHydrauliques[k]->WaterLayerValues;
        delete problem.CaracteristiquesHydrauliques[k]->InflowForTimeInterval;
        delete problem.CaracteristiquesHydrauliques[k];

        MemFree(problem.CoutsMarginauxDesContraintesDeReserveParZone[k]
                  ->CoutsMarginauxHorairesDeLaReserveParZone);
        delete problem.CoutsMarginauxDesContraintesDeReserveParZone[k];
        delete problem.ReserveJMoins1[k]->ReserveHoraireJMoins1;
        delete problem.ReserveJMoins1[k]->ReserveHoraireJMoins1Ref;
        delete problem.ReserveJMoins1[k];

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
            delete problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[j];
        }
        delete problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout;
        delete problem.PaliersThermiquesDuPays[k];

        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositive;
        delete problem.ResultatsHoraires[k].ValeursHorairesDENS;
        delete problem.ResultatsHoraires[k].ValeursHorairesLmrViolations;
        delete problem.ResultatsHoraires[k].ValeursHorairesSpilledEnergyAfterCSR;
        delete problem.ResultatsHoraires[k].ValeursHorairesDtgMrgCsr;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveUp;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveDown;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveAny;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegative;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeUp;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeDown;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeAny;
        delete problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceEnReserve;
        delete problem.ResultatsHoraires[k].TurbinageHoraire;
        delete problem.ResultatsHoraires[k].PompageHoraire;
        delete problem.ResultatsHoraires[k].TurbinageHoraireUp;
        delete problem.ResultatsHoraires[k].TurbinageHoraireDown;
        delete problem.ResultatsHoraires[k].niveauxHoraires;
        delete problem.ResultatsHoraires[k].valeurH2oHoraire;
        delete problem.ResultatsHoraires[k].debordementsHoraires;
        delete problem.ResultatsHoraires[k].CoutsMarginauxHoraires;

        for (uint j = 0; j < problem.NombreDePasDeTemps; j++)
        {
            MemFree(
              problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalier);
            MemFree(
              problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalierUp);
            MemFree(
              problem.ResultatsHoraires[k].ProductionThermique[j]->ProductionThermiqueDuPalierDown);
            MemFree(
              problem.ResultatsHoraires[k].ProductionThermique[j]->NombreDeGroupesEnMarcheDuPalier);
            MemFree(problem.ResultatsHoraires[k]
                      .ProductionThermique[j]
                      ->NombreDeGroupesQuiDemarrentDuPalier);
            MemFree(problem.ResultatsHoraires[k]
                      .ProductionThermique[j]
                      ->NombreDeGroupesQuiSArretentDuPalier);
            MemFree(problem.ResultatsHoraires[k]
                      .ProductionThermique[j]
                      ->NombreDeGroupesQuiTombentEnPanneDuPalier);
            delete problem.ResultatsHoraires[k].ProductionThermique[j];
        }
        delete problem.ResultatsHoraires[k].ProductionThermique;
    }
    delete problem.PaliersThermiquesDuPays;
    delete problem.CaracteristiquesHydrauliques;

    delete problem.CoutsMarginauxDesContraintesDeReserveParZone;
    delete problem.ReserveJMoins1;

    delete problem.coutOptimalSolution1;
    delete problem.coutOptimalSolution2;
    delete problem.tempsResolution1;
    delete problem.tempsResolution2;
}
