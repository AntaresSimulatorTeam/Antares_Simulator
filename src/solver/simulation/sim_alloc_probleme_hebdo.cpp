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

    problem.DefaillanceNegativeUtiliserPMinThermique.assign(nbPays, false);
    problem.DefaillanceNegativeUtiliserHydro.assign(nbPays, false);
    problem.DefaillanceNegativeUtiliserConsoAbattue.assign(nbPays, false);

    problem.CoefficientEcretementPMaxHydraulique.assign(nbPays, 0.);

    problem.BruitSurCoutHydraulique.assign(nbPays, std::vector<double>(8784));

    problem.NomsDesPays.resize(nbPays);

    problem.PaysExtremiteDeLInterconnexion.assign(linkCount, 0);
    problem.PaysOrigineDeLInterconnexion.assign(linkCount, 0);

    problem.CoutDeTransport.resize(linkCount);

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

    problem.ValeursDeNTC.resize(NombreDePasDeTemps);
    problem.ValeursDeNTCRef.resize(NombreDePasDeTemps);

    problem.ConsommationsAbattues.resize(NombreDePasDeTemps);
    problem.ConsommationsAbattuesRef.resize(NombreDePasDeTemps);

    problem.AllMustRunGeneration.resize(NombreDePasDeTemps);
    problem.SoldeMoyenHoraire.resize(NombreDePasDeTemps);
    problem.CorrespondanceVarNativesVarOptim.resize(NombreDePasDeTemps);
    problem.CorrespondanceCntNativesCntOptim.resize(NombreDePasDeTemps);
    problem.VariablesDualesDesContraintesDeNTC.resize(NombreDePasDeTemps);

    auto activeContraints = study.bindingConstraints.activeContraints();
    problem.NombreDeContraintesCouplantes = activeContraints.size();
    problem.MatriceDesContraintesCouplantes.resize(activeContraints.size());
    problem.PaliersThermiquesDuPays.resize(nbPays);
    problem.CaracteristiquesHydrauliques.resize(nbPays);
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

    problem.ReserveJMoins1.resize(nbPays);
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

        // TODO VP: Remove this allocation
        problem.CorrespondanceVarNativesVarOptim[k]
          = new CORRESPONDANCES_DES_VARIABLES;

        problem.ConsommationsAbattues[k].ConsommationAbattueDuPays.assign(nbPays, 0.);
        problem.ConsommationsAbattuesRef[k].ConsommationAbattueDuPays.assign(nbPays, 0.);

        problem.AllMustRunGeneration[k].AllMustRunGenerationOfArea.assign(nbPays, 0.);

        problem.SoldeMoyenHoraire[k].SoldeMoyenDuPays.assign(nbPays, 0.);

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDeLInterconnexion
          .assign(linkCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableCoutOrigineVersExtremiteDeLInterconnexion
          .assign(linkCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableCoutExtremiteVersOrigineDeLInterconnexion
          .assign(linkCount, 0);

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDuPalierThermique
          .assign(study.runtime->thermalPlantTotalCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeLaProdHyd
          .assign(nbPays, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDePompage
          .assign(nbPays, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeNiveau
          .assign(nbPays, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesDeDebordement
          .assign(nbPays, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillancePositive
          .assign(nbPays, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariableDefaillanceNegative
          .assign(nbPays, 0);

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaBaisse
          .assign(nbPays, 0);

        problem.CorrespondanceVarNativesVarOptim[k]->NumeroDeVariablesVariationHydALaHausse
          .assign(nbPays, 0);

        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique
          .assign(study.runtime->thermalPlantTotalCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique
          .assign(study.runtime->thermalPlantTotalCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique
          .assign(study.runtime->thermalPlantTotalCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]
          ->NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique
          .assign(study.runtime->thermalPlantTotalCount, 0);

        problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.InjectionVariable
          .assign(shortTermStorageCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.WithdrawalVariable
          .assign(shortTermStorageCount, 0);
        problem.CorrespondanceVarNativesVarOptim[k]->SIM_ShortTermStorage.LevelVariable
          .assign(shortTermStorageCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDesBilansPays
          .assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContraintePourEviterLesChargesFictives
          .assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDesNiveauxPays
          .assign(nbPays, 0);

        problem.CorrespondanceCntNativesCntOptim[k].ShortTermStorageLevelConstraint
          .assign(shortTermStorageCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k].NumeroPremiereContrainteDeReserveParZone
          .assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeuxiemeContrainteDeReserveParZone
          .assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDeDissociationDeFlux
          .assign(linkCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDesContraintesCouplantes
          .assign(activeContraints.size(), 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesDeDureeMinDeMarche
          .assign(study.runtime->thermalPlantTotalCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesDeDureeMinDArret
          .assign(study.runtime->thermalPlantTotalCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
          .assign(study.runtime->thermalPlantTotalCount, 0);

        problem.VariablesDualesDesContraintesDeNTC[k].VariableDualeParInterconnexion
          .assign(linkCount, 0.);
    }

    for (unsigned k = 0; k < linkCount; ++k)
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

    problem.CorrespondanceCntNativesCntOptimJournalieres.resize(7);
    for (uint k = 0; k < 7; k++)
    {
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          .NumeroDeContrainteDesContraintesCouplantes
          .assign(activeContraints.size(), 0);
    }

    problem.CorrespondanceCntNativesCntOptimHebdomadaires
        .NumeroDeContrainteDesContraintesCouplantes
        .assign(activeContraints.size(), 0);

    const auto& bindingConstraintCount = activeContraints.size();
    problem.ResultatsContraintesCouplantes.resize(bindingConstraintCount);

    for (unsigned k = 0; k < bindingConstraintCount; k++)
    {
        auto activeConstraints = study.bindingConstraints.activeContraints();
        assert(k < activeConstraints.size());
        assert(activeConstraints[k]->linkCount() < 50000000);
        assert(activeConstraints[k]->clusterCount() < 50000000);

        problem.MatriceDesContraintesCouplantes[k].SecondMembreDeLaContrainteCouplante
          .assign(NombreDePasDeTemps, 0.);
        problem.MatriceDesContraintesCouplantes[k].SecondMembreDeLaContrainteCouplanteRef
          .assign(NombreDePasDeTemps, 0.);

        problem.MatriceDesContraintesCouplantes[k].NumeroDeLInterconnexion
          .assign(activeContraints[k]->linkCount(), 0);
        problem.MatriceDesContraintesCouplantes[k].PoidsDeLInterconnexion
          .assign(activeContraints[k]->linkCount(), 0.);
        problem.MatriceDesContraintesCouplantes[k].OffsetTemporelSurLInterco
          .assign(activeContraints[k]->linkCount(), 0);

        problem.MatriceDesContraintesCouplantes[k].NumeroDuPalierDispatch
          .assign(activeContraints[k]->clusterCount(), 0);
        problem.MatriceDesContraintesCouplantes[k].PoidsDuPalierDispatch
          .assign(activeContraints[k]->clusterCount(), 0.);
        problem.MatriceDesContraintesCouplantes[k].OffsetTemporelSurLePalierDispatch
          .assign(activeContraints[k]->clusterCount(), 0);
        problem.MatriceDesContraintesCouplantes[k].PaysDuPalierDispatch
          .assign(activeContraints[k]->clusterCount(), 0);


        // TODO : create a numberOfTimeSteps method in class of runtime->bindingConstraint
        unsigned int nbTimeSteps;
        switch (activeContraints[k]->type())
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
            problem.ResultatsContraintesCouplantes[k].variablesDuales.assign(nbTimeSteps, 0.);
    }

    for (unsigned k = 0; k < nbPays; k++)
    {
        const uint nbPaliers = study.areas.byIndex[k]->thermal.list.size();

        problem.PaliersThermiquesDuPays[k].minUpDownTime.assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k].PminDuPalierThermiquePendantUneHeure
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].PminDuPalierThermiquePendantUnJour
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].TailleUnitaireDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].NumeroDuPalierDansLEnsembleDesPaliersThermiques
         .assign(nbPaliers, 0);

        problem.PaliersThermiquesDuPays[k].CoutDeDemarrageDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].CoutDArretDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].CoutFixeDeMarcheDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].pminDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].PmaxDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].DureeMinimaleDeMarcheDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k].DureeMinimaleDArretDUnGroupeDuPalierThermique
         .assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k].NomsDesPaliersThermiques.resize(nbPaliers);

        problem.CaracteristiquesHydrauliques[k].CntEnergieH2OParIntervalleOptimise
          .assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].CntEnergieH2OParJour
          .assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].CntEnergieH2OParIntervalleOptimiseRef
          .assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxHydrauliqueHoraire
          .assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxHydrauliqueHoraireRef
          .assign(NombreDePasDeTemps, 0.);

        problem.CaracteristiquesHydrauliques[k].MaxEnergieHydrauParIntervalleOptimise
          .assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].MinEnergieHydrauParIntervalleOptimise
          .assign(7, 0.);

        problem.CaracteristiquesHydrauliques[k].NiveauHoraireSup
          .assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].NiveauHoraireInf
          .assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].ApportNaturelHoraire
          .assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].MingenHoraire
          .assign(NombreDePasDeTemps, 0.);

        problem.CaracteristiquesHydrauliques[k].WaterLayerValues
          .assign(100, 0.);
        problem.CaracteristiquesHydrauliques[k].InflowForTimeInterval
          .assign(100, 0.);

        problem.CaracteristiquesHydrauliques[k].MaxEnergiePompageParIntervalleOptimise
          .assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxPompageHoraire
          .assign(NombreDePasDeTemps, 0.);

        problem.ReserveJMoins1[k].ReserveHoraireJMoins1
          .assign(NombreDePasDeTemps, 0.);
        problem.ReserveJMoins1[k].ReserveHoraireJMoins1Ref
          .assign(NombreDePasDeTemps, 0.);

        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositive
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDENS
          .assign(NombreDePasDeTemps, 0.); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesLmrViolations
          .assign(NombreDePasDeTemps, 0); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesSpilledEnergyAfterCSR
          .assign(NombreDePasDeTemps, 0.); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesDtgMrgCsr
          .assign(NombreDePasDeTemps, 0.); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveUp
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveDown
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositiveAny
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegative
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeUp
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeDown
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegativeAny
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceEnReserve
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].TurbinageHoraire
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].PompageHoraire
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].TurbinageHoraireUp
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].TurbinageHoraireDown
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].CoutsMarginauxHoraires
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].niveauxHoraires
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].valeurH2oHoraire
          .assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].debordementsHoraires
          .assign(NombreDePasDeTemps, 0.);
        problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout.resize(nbPaliers);
        problem.ResultatsHoraires[k].ProductionThermique.resize(NombreDePasDeTemps);

        for (unsigned j = 0; j < nbPaliers; ++j)
        {
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .CoutHoraireDeProductionDuPalierThermique
              .assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .CoutHoraireDeProductionDuPalierThermiqueRef
              .assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .PuissanceDisponibleDuPalierThermique
              .assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .PuissanceDisponibleDuPalierThermiqueRef
              .assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .PuissanceMinDuPalierThermique
              .assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .PuissanceMinDuPalierThermique_SV
              .assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique
              .assign(NombreDePasDeTemps, 0);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .NombreMinDeGroupesEnMarcheDuPalierThermique
              .assign(NombreDePasDeTemps, 0);

            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .CoutHoraireDuPalierThermiqueUp
              .assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout[j]
              .CoutHoraireDuPalierThermiqueDown
              .assign(NombreDePasDeTemps, 0.);
        }
        for (unsigned j = 0; j < NombreDePasDeTemps; j++)
        {
            problem.ResultatsHoraires[k].ProductionThermique[j].ProductionThermiqueDuPalier
              .assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k].ProductionThermique[j].ProductionThermiqueDuPalierUp
              .assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k].ProductionThermique[j].ProductionThermiqueDuPalierDown
              .assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k].ProductionThermique[j].NombreDeGroupesEnMarcheDuPalier
              .assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k].ProductionThermique[j].NombreDeGroupesQuiDemarrentDuPalier
              .assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k].ProductionThermique[j].NombreDeGroupesQuiSArretentDuPalier
              .assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k].ProductionThermique[j]
              .NombreDeGroupesQuiTombentEnPanneDuPalier
              .assign(nbPaliers, 0.);
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

    problem.coutOptimalSolution1.assign(7, 0.);
    problem.coutOptimalSolution2.assign(7, 0.);

    problem.tempsResolution1.assign(7, 0.);
    problem.tempsResolution2.assign(7, 0.);
}

void SIM_DesallocationProblemeHebdo(PROBLEME_HEBDO& problem)
{
    for (uint k = 0; k < problem.NombreDePasDeTemps; k++)
    {
        delete problem.CorrespondanceVarNativesVarOptim[k];
    }
}
