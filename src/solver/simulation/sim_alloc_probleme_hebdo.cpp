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
    problem.CoutDeDefaillancePositiveSansBruit.assign(nbPays, 0);
    problem.CoutDeDefaillanceNegativeSansBruit.assign(nbPays, 0);

    problem.CoutDeDefaillancePositiveSansBruit.assign(nbPays, 0);
    problem.CoutDeDefaillanceNegativeSansBruit.assign(nbPays, 0);

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
              &variablesMapping.NumeroDeVariableDuNombreDeGroupesQuiTombentEnPanneDuPalierThermique,
              &variablesMapping.powerRampingIncreaseIndex,
              &variablesMapping.powerRampingDecreaseIndex})
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

        auto& cntMapping = problem.CorrespondanceCntNativesCntOptim[k];

        for (auto* v: {&cntMapping.NumeroDeContrainteDesBilansPays,
                       &cntMapping.NumeroDeContraintePourEviterLesChargesFictives,
                       &cntMapping.NumeroDeContrainteDesNiveauxPays,
                       &cntMapping.NumeroDeContraintePourBornerLaDefaillance,
                       &cntMapping.NumeroPremiereContrainteDeReserveParZone,
                       &cntMapping.NumeroDeuxiemeContrainteDeReserveParZone})
        {
            v->assign(nbPays, 0);
        }

        for (auto* v: {&cntMapping.ShortTermStorageLevelConstraint,
                       &cntMapping.ShortTermStorageCostVariationInjectionForward,
                       &cntMapping.ShortTermStorageCostVariationInjectionBackward,
                       &cntMapping.ShortTermStorageCostVariationWithdrawalForward,
                       &cntMapping.ShortTermStorageCostVariationWithdrawalBackward})
        {
            v->assign(shortTermStorageCount, 0);
        }

        for (auto* v:
             {&cntMapping.NumeroDeContrainteDesContraintesDeDureeMinDeMarche,
              &cntMapping.NumeroDeContrainteDesContraintesDeDureeMinDArret,
              &cntMapping.NumeroDeLaDeuxiemeContrainteDesContraintesDesGroupesQuiTombentEnPanne})
        {
            v->assign(thermalCount, 0);
        }

        cntMapping.NumeroDeContrainteDeDissociationDeFlux.assign(linkCount, 0);
        cntMapping.NumeroDeContrainteDesContraintesCouplantes.assign(activeConstraints.size(), 0);

        problem.VariablesDualesDesContraintesDeNTC[k]
          .VariableDualeParInterconnexion.assign(linkCount, 0.);

        if (study.parameters.include.reserves)
        {
            problem.CorrespondanceCntNativesCntOptim[k].reservesIndices.emplace();
            auto& resCorresp = problem.CorrespondanceCntNativesCntOptim[k].reservesIndices.value();

            resCorresp.need.assign(capacityReservationCount, -1);

            for (auto* v: {&resCorresp.maxPowerOffUnitsInThermalCluster,
                           &resCorresp.thermalClusterPOutBoundMin,
                           &resCorresp.thermalClusterPOutBoundMax})
            {
                v->assign(thermalCount, -1);
            }

            resCorresp.powerOffGroupUnitsInThermalClusterParticipating
              .assign(thermalCount * capacityReservationCount, -1);

            for (auto* v: {&resCorresp.STStorageClusterMaxReleaseParticipation,
                           &resCorresp.STStorageClusterMaxStoreParticipation,
                           &resCorresp.STStorageEnergyLevelParticipation})
            {
                v->assign(shortTermStorageCount * capacityReservationCount, -1);
            }

            for (auto* v: {&resCorresp.STStorageClusterReleaseCapacityThresholdsMax,
                           &resCorresp.STStorageClusterReleaseCapacityThresholdsMin,
                           &resCorresp.STStorageClusterStoreCapacityThresholds,
                           &resCorresp.STStorageLevelParticipation.down,
                           &resCorresp.STStorageLevelParticipation.up,
                           &resCorresp.STStorageGlobalStockEnergyLevelParticipation.up,
                           &resCorresp.STStorageGlobalStockEnergyLevelParticipation.down})
            {
                v->assign(shortTermStorageCount, -1);
            }

            for (auto* v: {&resCorresp.HydroMaxReleaseParticipation,
                           &resCorresp.HydroMaxStoreParticipation,
                           &resCorresp.HydroEnergyLevelParticipation})
            {
                v->assign(hydroCount * capacityReservationCount, -1);
            }

            for (auto* v: {&resCorresp.HydroReleaseCapacityThresholdsMax,
                           &resCorresp.HydroReleaseCapacityThresholdsMin,
                           &resCorresp.HydroStoreCapacityThresholds,
                           &resCorresp.HydroLevelParticipation.down,
                           &resCorresp.HydroLevelParticipation.up,
                           &resCorresp.HydroGlobalEnergyLevelParticipationUp,
                           &resCorresp.HydroGlobalEnergyLevelParticipationDown})
            {
                v->assign(hydroCount, -1);
            }
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

        auto& palier = problem.PaliersThermiquesDuPays[k];

        for (auto* v: {&palier.minUpDownTime,
                       &palier.NumeroDuPalierDansLEnsembleDesPaliersThermiques,
                       &palier.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique,
                       &palier.DureeMinimaleDArretDUnGroupeDuPalierThermique})
        {
            v->assign(nbPaliers, 0);
        }

        for (auto* v: {&palier.PminDuPalierThermiquePendantUneHeure,
                       &palier.PminDuPalierThermiquePendantUnJour,
                       &palier.TailleUnitaireDUnGroupeDuPalierThermique,
                       &palier.CoutDeDemarrageDUnGroupeDuPalierThermique,
                       &palier.CoutDArretDUnGroupeDuPalierThermique,
                       &palier.CoutFixeDeMarcheDUnGroupeDuPalierThermique,
                       &palier.pminDUnGroupeDuPalierThermique,
                       &palier.PmaxDUnGroupeDuPalierThermique})
        {
            v->assign(nbPaliers, 0.);
        }

        for (auto* v: {&palier.downwardRampingCost,
                       &palier.downwardRampingCost,
                       &palier.maxUpwardPowerRampingRate,
                       &palier.maxDownwardPowerRampingRate})
        {
            v->assign(nbPaliers, -1);
        }

        palier.NomsDesPaliersThermiques.resize(nbPaliers);
        palier.emissionFactors
          .assign(nbPaliers, std::array<double, Antares::Data::Pollutant::POLLUTANT_MAX>{});

        auto& hydro = problem.CaracteristiquesHydrauliques[k];

        for (auto* v: {&hydro.CntEnergieH2OParIntervalleOptimise,
                       &hydro.CntEnergieH2OParJour,
                       &hydro.MaxEnergieHydrauParIntervalleOptimise,
                       &hydro.MinEnergieHydrauParIntervalleOptimise,
                       &hydro.MaxEnergiePompageParIntervalleOptimise})
        {
            v->assign(7, 0.);
        }

        for (auto* v: {&hydro.ContrainteDePmaxHydrauliqueHoraire,
                       &hydro.ContrainteDePmaxHydrauliqueHoraireRef,
                       &hydro.NiveauHoraireSup,
                       &hydro.NiveauHoraireInf,
                       &hydro.ApportNaturelHoraire,
                       &hydro.MingenHoraire,
                       &hydro.ContrainteDePmaxPompageHoraire})
        {
            v->assign(NombreDePasDeTemps, 0.);
        }

        for (auto* v: {&hydro.WaterLayerValues, &hydro.InflowForTimeInterval})
        {
            v->assign(100, 0.);
        }

        problem.ReserveJMoins1[k].ReserveHoraireJMoins1.assign(NombreDePasDeTemps, 0.);

        auto& resultats = problem.ResultatsHoraires[k];

        for (auto* v: {&resultats.ValeursHorairesDeDefaillancePositive,
                       &resultats.ValeursHorairesDeDefaillancePositiveCSR,
                       &resultats.ValeursHorairesDENS,      // adq patch
                       &resultats.ValeursHorairesDtgMrgCsr, // adq patch
                       &resultats.ValeursHorairesDeDefaillanceNegative,
                       &resultats.TurbinageHoraire,
                       &resultats.PompageHoraire,
                       &resultats.CoutsMarginauxHoraires,
                       &resultats.CoutsMarginauxHorairesCSR,
                       &resultats.niveauxHoraires,
                       &resultats.valeurH2oHoraire,
                       &resultats.debordementsHoraires})
        {
            v->assign(NombreDePasDeTemps, 0.);
        }

        resultats.ValeursHorairesLmrViolations.assign(NombreDePasDeTemps, 0); // adq patch

        palier.PuissanceDisponibleEtCout.resize(nbPaliers);
        resultats.ProductionThermique.resize(NombreDePasDeTemps);
        if (resEnabled)
        {
            resultats.Reserves.emplace(NombreDePasDeTemps);
            resultats.HydroUsage.resize(NombreDePasDeTemps);
        }

        for (unsigned j = 0; j < nbPaliers; ++j)
        {
            auto& puissance = palier.PuissanceDisponibleEtCout[j];

            for (auto* v: {&puissance.CoutHoraireDeProductionDuPalierThermique,
                           &puissance.CoutHoraireDeProductionDuPalierThermiqueSansBruit,
                           &puissance.PuissanceDisponibleDuPalierThermique,
                           &puissance.PuissanceDisponibleDuPalierThermiqueRef,
                           &puissance.PuissanceMinDuPalierThermique,
                           &puissance.PuissanceMinDuPalierThermiqueRef})
            {
                v->assign(NombreDePasDeTemps, 0.);
            }

            for (auto* v: {&puissance.NombreMaxDeGroupesEnMarcheDuPalierThermique,
                           &puissance.NombreMinDeGroupesEnMarcheDuPalierThermique})
            {
                v->assign(NombreDePasDeTemps, 0);
            }
        }
        for (unsigned j = 0; j < NombreDePasDeTemps; j++)
        {
            auto& production = resultats.ProductionThermique[j];

            for (auto* v: {&production.ProductionThermiqueDuPalier,
                           &production.NombreDeGroupesEnMarcheDuPalier,
                           &production.NombreDeGroupesQuiDemarrentDuPalier,
                           &production.NombreDeGroupesQuiSArretentDuPalier,
                           &production.NombreDeGroupesQuiTombentEnPanneDuPalier})
            {
                v->assign(nbPaliers, 0.);
            }
            if (resEnabled)
            {
                production.ParticipationReservesDuPalier.emplace(nbThermalReserveParticipations,
                                                                 0.);
                production.ParticipationReservesDuPalierOn.emplace(nbThermalReserveParticipations,
                                                                   0.);
                production.ParticipationReservesDuPalierOff.emplace(nbThermalReserveParticipations,
                                                                    0.);

                auto& res = resultats.Reserves.value()[j];
                res.ValeursHorairesInternalUnsatisfied.assign(nbReserves, 0.);
                res.ValeursHorairesInternalExcessReserve.assign(nbReserves, 0.);
                res.CoutsMarginauxHoraires.assign(nbReserves, 0.);

                resultats.HydroUsage[j]
                  .reserveParticipationOfCluster.emplace(nbHydroReserveParticipations, 0.);
            }
        }
        // Short term storage results

        const unsigned long nbShortTermStorage = study.areas.byIndex[k]->shortTermStorage.count();
        resultats.ShortTermStorage.resize(nbShortTermStorage);
        for (uint sts = 0; sts < nbShortTermStorage; sts++)
        {
            auto& storage = resultats.ShortTermStorage[sts];
            for (auto* v:
                 {&storage.injection, &storage.withdrawal, &storage.level, &storage.overflow})
            {
                v->resize(NombreDePasDeTemps);
            }
        }

        if (resEnabled)
        {
            resultats.ShortTermStorageReserves.emplace(nbSTStorageReserveParticipations);
            for (uint stsRes = 0; stsRes < nbSTStorageReserveParticipations; stsRes++)
            {
                resultats.ShortTermStorageReserves.value()[stsRes]
                  .reserveParticipationOfCluster.emplace(NombreDePasDeTemps, 0.);
            }
        }
    }
}
