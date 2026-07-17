// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/simulation/sim_alloc_probleme_hebdo.h"

#include <antares/study/study.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_donnees.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares;

void SIM_AllocationProblemeHebdo(const Data::Study& study,
                                 PROBLEME_HEBDO& problem,
                                 unsigned NombreDePasDeTemps)
{
    try
    {
        SIM_AllocationProblemeDonneesGenerales(problem, study, NombreDePasDeTemps);
        SIM_AllocationProblemePasDeTemps(problem, study, NombreDePasDeTemps);
        SIM_AllocationLinks(problem, study.runtime.interconnectionsCount(), NombreDePasDeTemps);
        SIM_AllocationConstraints(problem, study, NombreDePasDeTemps);
        SIM_AllocationShortermStorageCumulation(problem, study);
        SIM_AllocateAreas(problem, study, NombreDePasDeTemps);
    }
    catch (const std::bad_alloc& e)
    {
        logs.error() << "Memory allocation failed, aborting (" << e.what() << ")";
    }
}

void SIM_AllocationProblemeDonneesGenerales(PROBLEME_HEBDO& problem,
                                            const Antares::Data::Study& study,
                                            unsigned NombreDePasDeTemps)
{
    uint nbPays = study.areas.size();

    const uint linkCount = study.runtime.interconnectionsCount();

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

    problem.CoutDeDebordement.assign(nbPays, 0);

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

    problem.ConsommationsAbattues.resize(NombreDePasDeTemps);

    problem.AllMustRunGeneration.resize(NombreDePasDeTemps);
    problem.SoldeMoyenHoraire.resize(NombreDePasDeTemps);
    problem.CorrespondanceVarNativesVarOptim.resize(NombreDePasDeTemps);
    problem.CorrespondanceCntNativesCntOptim.resize(NombreDePasDeTemps);
    problem.VariablesDualesDesContraintesDeNTC.resize(NombreDePasDeTemps);

    auto activeConstraints = study.bindingConstraints.activeConstraints();
    problem.NombreDeContraintesCouplantes = activeConstraints.size();
    problem.MatriceDesContraintesCouplantes.resize(activeConstraints.size());
    problem.PaliersThermiquesDuPays.resize(nbPays);
    problem.CaracteristiquesHydrauliques.resize(nbPays);
    problem.previousSimulationFinalLevel.assign(nbPays, 0.);

    problem.ShortTermStorage.resize(nbPays);

    problem.ReserveJMoins1.resize(nbPays);
    problem.ResultatsHoraires.resize(nbPays);

    problem.coutOptimalSolution1.assign(7, 0.);
    problem.coutOptimalSolution2.assign(7, 0.);
}

void SIM_AllocationProblemePasDeTemps(PROBLEME_HEBDO& problem,
                                      const Antares::Data::Study& study,
                                      unsigned NombreDePasDeTemps)
{
    uint nbPays = study.areas.size();

    const uint linkCount = study.runtime.interconnectionsCount();
    const uint thermalCount = study.runtime.counts.thermalPlants;
    const uint shortTermStorageCount = study.runtime.counts.shortTermStorages;
    const uint hydroCount = study.runtime.counts.hydros;
    const uint capacityReservationCount = study.runtime.counts.capacityReservations;

    auto activeConstraints = study.bindingConstraints.activeConstraints();

    for (uint k = 0; k < NombreDePasDeTemps; k++)
    {
        problem.ValeursDeNTC[k].ValeurDeNTCOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDeNTCExtremiteVersOrigine.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDeLoopFlowOrigineVersExtremite.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ValeurDuFlux.assign(linkCount, 0.);
        problem.ValeursDeNTC[k].ResistanceApparente.assign(linkCount, 0.);

        problem.ConsommationsAbattues[k].ConsommationAbattueDuPays.assign(nbPays, 0.);

        problem.AllMustRunGeneration[k].AllMustRunGenerationOfArea.assign(nbPays, 0.);

        problem.SoldeMoyenHoraire[k].SoldeMoyenDuPays.assign(nbPays, 0.);

        auto& variablesMapping = problem.CorrespondanceVarNativesVarOptim[k];

        for (auto* v: {&variablesMapping.NumeroDeVariableDuFluxDirect,
                       &variablesMapping.NumeroDeVariableDuFluxDirectPositif,
                       &variablesMapping.NumeroDeVariableDuFluxIndirectPositif})
        {
            v->assign(linkCount, 0);
        }

        for (auto* v:
             {&variablesMapping.NumeroDeVariableDuPalierThermique,
              &variablesMapping.NumeroDeVariableDuNombreDeGroupesEnMarcheDuPalierThermique,
              &variablesMapping.NumeroDeVariableDuNombreDeGroupesQuiDemarrentDuPalierThermique,
              &variablesMapping.NumeroDeVariableDuNombreDeGroupesQuiSArretentDuPalierThermique,
              &variablesMapping.NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique})
        {
            v->assign(thermalCount, 0);
        }

        for (auto* v: {&variablesMapping.NumeroDeVariablesDeLaProdHyd,
                       &variablesMapping.NumeroDeVariablesDePompage,
                       &variablesMapping.NumeroDeVariablesDeNiveau,
                       &variablesMapping.NumeroDeVariablesDeDebordement,
                       &variablesMapping.NumeroDeVariableDefaillancePositive,
                       &variablesMapping.NumeroDeVariableDefaillanceNegative,
                       &variablesMapping.NumeroDeVariablesVariationHydALaBaisse,
                       &variablesMapping.NumeroDeVariablesVariationHydALaHausse})
        {
            v->assign(nbPays, 0);
        }

        for (auto* v: {&variablesMapping.SIM_ShortTermStorage.InjectionVariable,
                       &variablesMapping.SIM_ShortTermStorage.WithdrawalVariable,
                       &variablesMapping.SIM_ShortTermStorage.LevelVariable,
                       &variablesMapping.SIM_ShortTermStorage.CostVariationInjection,
                       &variablesMapping.SIM_ShortTermStorage.CostVariationWithdrawal,
                       &variablesMapping.SIM_ShortTermStorage.OverflowVariable})
        {
            v->assign(shortTermStorageCount, 0);
        }

        if (study.parameters.include.reserves)
        {
            variablesMapping.reservesIndices.emplace();
            auto& varMapping = variablesMapping.reservesIndices.value();

            for (auto* v: {&varMapping.runningThermalClusterParticipation,
                           &varMapping.offThermalClusterParticipation,
                           &varMapping.thermalClusterParticipation})
            {
                v->assign(thermalCount * capacityReservationCount, 0);
            }

            for (auto* v: {&varMapping.STStorageClusterParticipation.down,
                           &varMapping.STStorageClusterParticipation.up,
                           &varMapping.STStorageReleaseClusterParticipation,
                           &varMapping.STStorageStoreClusterParticipation})
            {
                v->assign(shortTermStorageCount * capacityReservationCount, 0);
            }

            for (auto* v: {&varMapping.HydroParticipation.up,
                           &varMapping.HydroParticipation.down,
                           &varMapping.HydroReleaseParticipation,
                           &varMapping.HydroStoreParticipation})
            {
                v->assign(hydroCount * capacityReservationCount, 0);
            }

            for (auto* v: {&varMapping.internalUnsatisfied, &varMapping.internalExcess})
            {
                v->assign(capacityReservationCount, 0);
            }
        }

        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDesBilansPays.assign(nbPays,
                                                                                           0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContraintePourEviterLesChargesFictives.assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k].NumeroDeContrainteDesNiveauxPays.assign(nbPays,
                                                                                            0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContraintePourBornerLaDefaillance.assign(nbPays, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageLevelConstraint.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationInjectionForward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationInjectionBackward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationWithdrawalForward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationWithdrawalBackward.assign(shortTermStorageCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationInjectionForward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationInjectionBackward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationWithdrawalForward.assign(shortTermStorageCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .ShortTermStorageCostVariationWithdrawalBackward.assign(shortTermStorageCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroPremiereContrainteDeReserveParZone.assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeuxiemeContrainteDeReserveParZone.assign(nbPays, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDeDissociationDeFlux.assign(linkCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesCouplantes.assign(activeConstraints.size(), 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesDeDureeMinDeMarche.assign(thermalCount, 0);
        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeContrainteDesContraintesDeDureeMinDArret.assign(thermalCount, 0);

        problem.CorrespondanceCntNativesCntOptim[k]
          .NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne
          .assign(thermalCount, 0);

        problem.VariablesDualesDesContraintesDeNTC[k]
          .VariableDualeParInterconnexion.assign(linkCount, 0.);

        if (study.parameters.include.reserves)
        {
            problem.CorrespondanceCntNativesCntOptim[k].reservesIndices.emplace();
            auto& resCorresp = problem.CorrespondanceCntNativesCntOptim[k].reservesIndices.value();
            resCorresp.need.assign(capacityReservationCount, -1);
            resCorresp.powerOffGroupUnitsInThermalClusterParticipating
              .assign(thermalCount * capacityReservationCount, -1);
            resCorresp.maxPowerOffUnitsInThermalCluster.assign(thermalCount, -1);
            resCorresp.thermalClusterPOutBoundMin.assign(thermalCount, -1);
            resCorresp.thermalClusterPOutBoundMax.assign(thermalCount, -1);

            resCorresp.STStorageClusterMaxReleaseParticipation.assign(shortTermStorageCount
                                                                        * capacityReservationCount,
                                                                      -1);
            resCorresp.STStorageClusterMaxStoreParticipation.assign(shortTermStorageCount
                                                                      * capacityReservationCount,
                                                                    -1);

            resCorresp.STStorageClusterReleaseCapacityThresholdsMax.assign(shortTermStorageCount,
                                                                           -1);
            resCorresp.STStorageClusterReleaseCapacityThresholdsMin.assign(shortTermStorageCount,
                                                                           -1);
            resCorresp.STStorageClusterStoreCapacityThresholds.assign(shortTermStorageCount, -1);
            resCorresp.STStorageLevelParticipation.down.assign(shortTermStorageCount, -1);
            resCorresp.STStorageLevelParticipation.up.assign(shortTermStorageCount, -1);
            resCorresp.STStorageEnergyLevelParticipation.assign(shortTermStorageCount
                                                                  * capacityReservationCount,
                                                                -1);
            resCorresp.STStorageGlobalStockEnergyLevelParticipation.up.assign(shortTermStorageCount,
                                                                              -1);
            resCorresp.STStorageGlobalStockEnergyLevelParticipation.down
              .assign(shortTermStorageCount, -1);

            resCorresp.HydroMaxReleaseParticipation.assign(hydroCount * capacityReservationCount,
                                                           -1);
            resCorresp.HydroMaxStoreParticipation.assign(hydroCount * capacityReservationCount, -1);
            resCorresp.HydroReleaseCapacityThresholdsMax.assign(hydroCount, -1);
            resCorresp.HydroReleaseCapacityThresholdsMin.assign(hydroCount, -1);
            resCorresp.HydroStoreCapacityThresholds.assign(hydroCount, -1);
            resCorresp.HydroLevelParticipation.down.assign(hydroCount, -1);
            resCorresp.HydroLevelParticipation.up.assign(hydroCount, -1);
            resCorresp.HydroEnergyLevelParticipation.assign(hydroCount * capacityReservationCount,
                                                            -1);
            resCorresp.HydroGlobalEnergyLevelParticipationUp.assign(hydroCount, -1);
            resCorresp.HydroGlobalEnergyLevelParticipationDown.assign(hydroCount, -1);
        }
    }
}

void SIM_AllocationLinks(PROBLEME_HEBDO& problem, const uint linkCount, unsigned NombreDePasDeTemps)
{
    for (unsigned k = 0; k < linkCount; ++k)
    {
        problem.CoutDeTransport[k].IntercoGereeAvecDesCouts = false;
        problem.CoutDeTransport[k].CoutDeTransportOrigineVersExtremite.assign(NombreDePasDeTemps,
                                                                              0.);
        problem.CoutDeTransport[k].CoutDeTransportExtremiteVersOrigine.assign(NombreDePasDeTemps,
                                                                              0.);
        problem.CoutDeTransport[k].CoutDeTransportOrigineVersExtremiteRef.assign(NombreDePasDeTemps,
                                                                                 0.);
        problem.CoutDeTransport[k].CoutDeTransportExtremiteVersOrigineRef.assign(NombreDePasDeTemps,
                                                                                 0.);
    }
}

void SIM_AllocationShortermStorageCumulation(PROBLEME_HEBDO& problem,
                                             const Antares::Data::Study& study)
{
    problem.CorrespondanceCntNativesCntOptimHebdomadaires.ShortTermStorageCumulation
      .assign(study.runtime.counts.shortTermStorageCumulativeConstraints, 0);
}

void SIM_AllocationConstraints(PROBLEME_HEBDO& problem,
                               const Antares::Data::Study& study,
                               unsigned NombreDePasDeTemps)
{
    auto activeConstraints = study.bindingConstraints.activeConstraints();

    problem.CorrespondanceCntNativesCntOptimJournalieres.resize(7);
    for (uint k = 0; k < 7; k++)
    {
        problem.CorrespondanceCntNativesCntOptimJournalieres[k]
          .NumeroDeContrainteDesContraintesCouplantes.assign(activeConstraints.size(), 0);
    }

    problem.CorrespondanceCntNativesCntOptimHebdomadaires.NumeroDeContrainteDesContraintesCouplantes
      .assign(activeConstraints.size(), 0);

    const auto& bindingConstraintCount = activeConstraints.size();

    for (unsigned constraintIndex = 0; constraintIndex != bindingConstraintCount; ++constraintIndex)
    {
        assert(constraintIndex < bindingConstraintCount);

        auto bc = activeConstraints[constraintIndex];

        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .SecondMembreDeLaContrainteCouplante.assign(NombreDePasDeTemps, 0.);

        auto linkCount = bc->linkCount();
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .NumeroDeLInterconnexion.assign(linkCount, 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PoidsDeLInterconnexion.assign(linkCount, 0.);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .OffsetTemporelSurLInterco.assign(linkCount, 0);

        auto clusterCount = bc->clusterCount();
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .NumeroDuPalierDispatch.assign(clusterCount, 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PoidsDuPalierDispatch.assign(clusterCount, 0.);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .OffsetTemporelSurLePalierDispatch.assign(clusterCount, 0);
        problem.MatriceDesContraintesCouplantes[constraintIndex]
          .PaysDuPalierDispatch.assign(clusterCount, 0);

        // TODO : create a numberOfTimeSteps method in class of runtime.bindingConstraint
        unsigned int nbTimeSteps;
        switch (bc->type())
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
            problem.ResultatsContraintesCouplantes.emplace(std::piecewise_construct,
                                                           std::forward_as_tuple(bc),
                                                           std::forward_as_tuple(nbTimeSteps, 0.));
        }
    }
}

void SIM_AllocateAreas(PROBLEME_HEBDO& problem,
                       const Antares::Data::Study& study,
                       unsigned NombreDePasDeTemps)
{
    uint nbPays = study.areas.size();

    for (unsigned k = 0; k < nbPays; k++)
    {
        const uint nbPaliers = study.areas.byIndex[k]->thermal.list.enabledAndNotMustRunCount();
        bool resEnabled = study.parameters.include.reserves;
        const uint nbThermalReserveParticipations = resEnabled ? study.areas.byIndex[k]
                                                                   ->thermal.list
                                                                   .reserveParticipationsCount()
                                                               : 0;
        const uint nbSTStorageReserveParticipations = resEnabled ? study.areas.byIndex[k]
                                                                     ->shortTermStorage
                                                                     .reserveParticipationsCount()
                                                                 : 0;
        const uint nbHydroReserveParticipations = resEnabled
                                                    ? study.areas.byIndex[k]
                                                        ->hydro.reserveParticipationsCount()
                                                    : 0;
        const uint nbReserves = resEnabled
                                  ? study.areas.byIndex[k]->allCapacityReservations.value().size()
                                  : 0;

        problem.PaliersThermiquesDuPays[k].minUpDownTime.assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k].PminDuPalierThermiquePendantUneHeure.assign(nbPaliers,
                                                                                       0.);
        problem.PaliersThermiquesDuPays[k].PminDuPalierThermiquePendantUnJour.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k]
          .TailleUnitaireDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k]
          .NumeroDuPalierDansLEnsembleDesPaliersThermiques.assign(nbPaliers, 0);

        problem.PaliersThermiquesDuPays[k]
          .CoutDeDemarrageDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].CoutDArretDUnGroupeDuPalierThermique.assign(nbPaliers,
                                                                                       0.);
        problem.PaliersThermiquesDuPays[k]
          .CoutFixeDeMarcheDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].pminDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k].PmaxDUnGroupeDuPalierThermique.assign(nbPaliers, 0.);
        problem.PaliersThermiquesDuPays[k]
          .DureeMinimaleDeMarcheDUnGroupeDuPalierThermique.assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k]
          .DureeMinimaleDArretDUnGroupeDuPalierThermique.assign(nbPaliers, 0);
        problem.PaliersThermiquesDuPays[k].NomsDesPaliersThermiques.resize(nbPaliers);
        problem.PaliersThermiquesDuPays[k].emissionFactors.assign(
          nbPaliers,
          std::array<double, Antares::Data::Pollutant::POLLUTANT_MAX>{});

        problem.CaracteristiquesHydrauliques[k].CntEnergieH2OParIntervalleOptimise.assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].CntEnergieH2OParJour.assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k]
          .ContrainteDePmaxHydrauliqueHoraire.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k]
          .ContrainteDePmaxHydrauliqueHoraireRef.assign(NombreDePasDeTemps, 0.);

        problem.CaracteristiquesHydrauliques[k].MaxEnergieHydrauParIntervalleOptimise.assign(7, 0.);
        problem.CaracteristiquesHydrauliques[k].MinEnergieHydrauParIntervalleOptimise.assign(7, 0.);

        problem.CaracteristiquesHydrauliques[k].NiveauHoraireSup.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].NiveauHoraireInf.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].ApportNaturelHoraire.assign(NombreDePasDeTemps, 0.);
        problem.CaracteristiquesHydrauliques[k].MingenHoraire.assign(NombreDePasDeTemps, 0.);

        problem.CaracteristiquesHydrauliques[k].WaterLayerValues.assign(100, 0.);
        problem.CaracteristiquesHydrauliques[k].InflowForTimeInterval.assign(100, 0.);

        problem.CaracteristiquesHydrauliques[k].MaxEnergiePompageParIntervalleOptimise.assign(7,
                                                                                              0.);
        problem.CaracteristiquesHydrauliques[k]
          .ContrainteDePmaxPompageHoraire.assign(NombreDePasDeTemps, 0.);

        problem.ReserveJMoins1[k].ReserveHoraireJMoins1.assign(NombreDePasDeTemps, 0.);

        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillancePositive.assign(NombreDePasDeTemps,
                                                                                 0.);
        problem.ResultatsHoraires[k]
          .ValeursHorairesDeDefaillancePositiveCSR.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].ValeursHorairesDENS.assign(NombreDePasDeTemps,
                                                                0.); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesLmrViolations.assign(NombreDePasDeTemps,
                                                                         0); // adq patch
        problem.ResultatsHoraires[k].ValeursHorairesDtgMrgCsr.assign(NombreDePasDeTemps,
                                                                     0.); // adq patch

        problem.ResultatsHoraires[k].ValeursHorairesDeDefaillanceNegative.assign(NombreDePasDeTemps,
                                                                                 0.);
        problem.ResultatsHoraires[k].TurbinageHoraire.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].PompageHoraire.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].CoutsMarginauxHoraires.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].CoutsMarginauxHorairesCSR.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].niveauxHoraires.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].valeurH2oHoraire.assign(NombreDePasDeTemps, 0.);
        problem.ResultatsHoraires[k].debordementsHoraires.assign(NombreDePasDeTemps, 0.);

        problem.PaliersThermiquesDuPays[k].PuissanceDisponibleEtCout.resize(nbPaliers);
        problem.ResultatsHoraires[k].ProductionThermique.resize(NombreDePasDeTemps);
        if (resEnabled)
        {
            problem.ResultatsHoraires[k].Reserves.emplace(NombreDePasDeTemps);
            problem.ResultatsHoraires[k].HydroUsage.resize(NombreDePasDeTemps);
        }

        for (unsigned j = 0; j < nbPaliers; ++j)
        {
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .CoutHoraireDeProductionDuPalierThermique.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceDisponibleDuPalierThermique.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceDisponibleDuPalierThermiqueRef.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceMinDuPalierThermique.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .PuissanceMinDuPalierThermiqueRef.assign(NombreDePasDeTemps, 0.);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .NombreMaxDeGroupesEnMarcheDuPalierThermique.assign(NombreDePasDeTemps, 0);
            problem.PaliersThermiquesDuPays[k]
              .PuissanceDisponibleEtCout[j]
              .NombreMinDeGroupesEnMarcheDuPalierThermique.assign(NombreDePasDeTemps, 0);
        }
        for (unsigned j = 0; j < NombreDePasDeTemps; j++)
        {
            problem.ResultatsHoraires[k].ProductionThermique[j].ProductionThermiqueDuPalier.assign(
              nbPaliers,
              0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesEnMarcheDuPalier.assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesQuiDemarrentDuPalier.assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesQuiSArretentDuPalier.assign(nbPaliers, 0.);
            problem.ResultatsHoraires[k]
              .ProductionThermique[j]
              .NombreDeGroupesQuiTombentEnPanneDuPalier.assign(nbPaliers, 0.);
            if (resEnabled)
            {
                {
                    auto& thermal = problem.ResultatsHoraires[k].ProductionThermique[j];
                    thermal.ParticipationReservesDuPalier.emplace(nbThermalReserveParticipations,
                                                                  0.);
                    thermal.ParticipationReservesDuPalierOn.emplace(nbThermalReserveParticipations,
                                                                    0.);
                    thermal.ParticipationReservesDuPalierOff.emplace(nbThermalReserveParticipations,
                                                                     0.);
                }
                {
                    auto& res = problem.ResultatsHoraires[k].Reserves.value()[j];
                    res.ValeursHorairesInternalUnsatisfied.assign(nbReserves, 0.);
                    res.ValeursHorairesInternalExcessReserve.assign(nbReserves, 0.);
                    res.CoutsMarginauxHoraires.assign(nbReserves, 0.);
                }
                problem.ResultatsHoraires[k].HydroUsage[j].reserveParticipationOfCluster.emplace(
                  nbHydroReserveParticipations,
                  0.);
            }
        }
        // Short term storage results

        const unsigned long nbShortTermStorage = study.areas.byIndex[k]->shortTermStorage.count();
        problem.ResultatsHoraires[k].ShortTermStorage.resize(nbShortTermStorage);
        for (uint sts = 0; sts < nbShortTermStorage; sts++)
        {
            problem.ResultatsHoraires[k].ShortTermStorage[sts].injection.resize(NombreDePasDeTemps);
            problem.ResultatsHoraires[k].ShortTermStorage[sts].withdrawal.resize(
              NombreDePasDeTemps);
            problem.ResultatsHoraires[k].ShortTermStorage[sts].level.resize(NombreDePasDeTemps);
            problem.ResultatsHoraires[k].ShortTermStorage[sts].overflow.resize(NombreDePasDeTemps);
        }

        if (resEnabled)
        {
            problem.ResultatsHoraires[k].ShortTermStorageReserves.emplace(
              nbSTStorageReserveParticipations);
            for (uint stsRes = 0; stsRes < nbSTStorageReserveParticipations; stsRes++)
            {
                problem.ResultatsHoraires[k]
                  .ShortTermStorageReserves.value()[stsRes]
                  .reserveParticipationOfCluster.emplace(NombreDePasDeTemps, 0.);
            }
        }
    }
}
