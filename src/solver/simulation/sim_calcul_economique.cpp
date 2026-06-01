// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <algorithm>
#include <sstream>

#include <antares/antares/fatal-error.h>
#include <antares/study/area/scratchpad.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include "antares/solver/optimisation/MipDetection.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_binding_constraints_rhs.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/workflow/generationAndResolutionConfig.h"
#include "antares/study/fwd.h"

using namespace Antares;
using namespace Antares::Data;

constexpr double LEVEL_TOLERANCE_MWH = 1.e-6;

void importCapacityReservations(AreaList& areas, PROBLEME_HEBDO& problem)
{
    int globalReserveIndex = 0;
    problem.allReserves = std::vector<::AREA_RESERVES_VECTOR>(areas.size());
    for (uint areaIndex = 0; areaIndex != areas.size(); areaIndex++)
    {
        int areaReserveIndex = 0;
        auto area = areas[areaIndex];
        auto& areaReserves = problem.allReserves.value()[areaIndex];
        for (auto type: {ReserveType::DOWN, ReserveType::UP})
        {
            areaReserves.referenceGlobalActivationDuration[type]
              = area->allCapacityReservations.value().referenceGlobalActivationDuration[type];
            areaReserves.maxGlobalEnergyActivationRatio[type] = area->allCapacityReservations
                                                                  .value()
                                                                  .maxGlobalEnergyActivationRatio
                                                                    [type];
        }

        for (const auto& [reserveID, reserveCapacity]:
             area->allCapacityReservations.value().areaCapacityReservations)
        {
            CAPACITY_RESERVATION areaCapacityReservation;
            areaCapacityReservation.type = reserveCapacity.type;
            areaCapacityReservation.unsuppliedCost = reserveCapacity.unsuppliedCost;
            areaCapacityReservation.spillageCost = reserveCapacity.spillageCost;
            areaCapacityReservation.powerActivationRatio = reserveCapacity.powerActivationRatio;
            areaCapacityReservation.energyActivationRatio = reserveCapacity.energyActivationRatio;
            areaCapacityReservation.referenceActivationDuration = reserveCapacity
                                                                    .referenceActivationDuration;
            areaCapacityReservation.reserveName = reserveCapacity.name();
            areaCapacityReservation.reserveID = reserveCapacity.id();
            areaCapacityReservation.globalReserveIndex = globalReserveIndex;
            areaCapacityReservation.areaReserveIndex = areaReserveIndex;
            globalReserveIndex++;
            areaReserveIndex++;
            areaCapacityReservation.need = reserveCapacity.need;

            areaReserves.areaCapacityReservations.emplace_back(areaCapacityReservation);
        }
    }
}

static void importShortTermStorages(Data::Parameters parameters,
                                    AreaList& areas,
                                    std::vector<::AREA_INPUT>& ShortTermStorageOut,
                                    PROBLEME_HEBDO& problem)
{
    int clusterGlobalIndex = 0;
    int constraintGlobalIndex = 0;
    int globalReserveIndex = 0;
    int globalSTStorageClusterParticipationIndex = 0;

    for (uint areaIndex = 0; areaIndex != areas.size(); areaIndex++)
    {
        int areaReserveIndex = 0;
        int areaClusterParticipationIndex = 0;
        const auto* area = areas[areaIndex];
        ShortTermStorageOut[areaIndex].resize(area->shortTermStorage.count());
        int storageIndex = 0;
        for (const auto& st: area->shortTermStorage.storagesByIndex)
        {
            PROPERTIES& toInsert = ShortTermStorageOut[areaIndex][storageIndex];
            toInsert.clusterGlobalIndex = clusterGlobalIndex;

            // capacities
            toInsert.reservoirCapacity = st.properties.reservoirCapacity.value();
            toInsert.injectionEfficiency = st.properties.injectionEfficiency;
            toInsert.withdrawalEfficiency = st.properties.withdrawalEfficiency;
            toInsert.injectionNominalCapacity = st.properties.injectionNominalCapacity.value();
            toInsert.withdrawalNominalCapacity = st.properties.withdrawalNominalCapacity.value();
            // initial level
            toInsert.initialLevel = st.properties.initialLevel;
            toInsert.initialLevelOptim = st.properties.initialLevelOptim;

            // optional penalization
            toInsert.penalizeVariationInjection = st.properties.penalizeVariationInjection;
            toInsert.penalizeVariationWithdrawal = st.properties.penalizeVariationWithdrawal;
            // optional overflow
            toInsert.allowOverflow = st.properties.allowOverflow;
            toInsert.overflowCost = area->thermal.spilledEnergyCost
                                    + area->hydro.overflowSpilledCostDifference;

            toInsert.name = st.properties.name;
            for (const auto& constraint: st.additionalConstraints)
            {
                if (constraint->enabled)
                {
                    for (auto& c: constraint->constraints)
                    {
                        c.globalIndex = constraintGlobalIndex;
                        ++constraintGlobalIndex;
                    }
                    toInsert.additionalConstraints.push_back(constraint);
                }
            }

            toInsert.clusterGlobalIndex = clusterGlobalIndex;
            toInsert.series = st.series;

            // TODO add missing properties, or use the same struct
            storageIndex++;
            clusterGlobalIndex++;
        }

        if (parameters.include.reserves && area->allCapacityReservations)
        {
            auto& areaReserves = problem.allReserves.value()[areaIndex];

            int areaReserveIdx = 0;
            for (const auto& [reserveID, _]:
                 area->allCapacityReservations.value().areaCapacityReservations)
            {
                for (size_t idx = 0; auto& cluster: area->shortTermStorage.storagesByIndex)
                {
                    if (cluster.reserveParticipationContainer
                        && cluster.reserveParticipationContainer.value().isParticipatingInReserve(
                          reserveID))
                    {
                        RESERVE_PARTICIPATION_STSTORAGE reserveParticipation;
                        reserveParticipation.maxRelease = cluster.reserveParticipationContainer
                                                            .value()
                                                            .reserveMaxRelease(reserveID);
                        reserveParticipation.maxStore = cluster.reserveParticipationContainer
                                                          .value()
                                                          .reserveMaxStore(reserveID);
                        reserveParticipation.participationCost = cluster
                                                                   .reserveParticipationContainer
                                                                   .value()
                                                                   .reserveCost(reserveID);
                        reserveParticipation.clusterName = cluster.id;
                        reserveParticipation.clusterIdInArea = idx;
                        reserveParticipation.clusterId = cluster.properties.clusterGlobalIndex;
                        reserveParticipation.globalIndexClusterParticipation
                          = globalSTStorageClusterParticipationIndex;
                        reserveParticipation.areaIndexClusterParticipation
                          = areaClusterParticipationIndex;

                        areaReserves.areaCapacityReservations[areaReserveIdx]
                          .AllSTStorageReservesParticipation.emplace(idx, reserveParticipation);

                        for (const auto& symIdx:
                             cluster.reserveParticipationContainer.value().symmetricalIndices(
                               reserveID))
                        {
                            auto& symmetries = areaReserves
                                                 .STStorageReservesParticipationSymmetries[idx];
                            if (symmetries.size() <= symIdx)
                            {
                                symmetries.resize(
                                  cluster.reserveParticipationContainer.value().getNbSymGroups());
                            }
                            symmetries[symIdx].emplace_back(
                              reserveID,
                              areaReserves.areaCapacityReservations[areaReserveIdx]
                                .AllSTStorageReservesParticipation[idx]);
                        }

                        ++globalSTStorageClusterParticipationIndex;
                        ++areaClusterParticipationIndex;
                    }
                    ++idx;
                }
                ++areaReserveIdx;
            };
        }
    }
}

void importHydroReserves(AreaList& areas, PROBLEME_HEBDO& problem)
{
    int globalReserveIndex = 0;
    int globalHydroParticipationIndex = 0;
    for (uint areaIndex = 0; areaIndex != areas.size(); areaIndex++)
    {
        int areaReserveIndex = 0;
        int areaClusterParticipationIndex = 0;
        auto area = areas[areaIndex];
        auto& hydro = area->hydro;

        if (area->allCapacityReservations && hydro.reserveParticipationContainer)
        {
            auto& areaReserves = problem.allReserves.value()[areaIndex];

            int areaReserveIdx = 0;
            for (const auto& [reserveID, _]:
                 area->allCapacityReservations.value().areaCapacityReservations)
            {
                if (hydro.reserveParticipationContainer.value().isParticipatingInReserve(reserveID))
                {
                    RESERVE_PARTICIPATION_HYDRO reserveParticipation;
                    reserveParticipation.maxRelease = hydro.reserveParticipationContainer.value()
                                                        .reserveMaxRelease(reserveID);
                    reserveParticipation.maxStore = hydro.reserveParticipationContainer.value()
                                                      .reserveMaxStore(reserveID);
                    reserveParticipation.participationCost = hydro.reserveParticipationContainer
                                                               .value()
                                                               .reserveCost(reserveID);
                    reserveParticipation.clusterName = "Hydro";
                    reserveParticipation.clusterIdInArea = 0;
                    reserveParticipation.globalIndexClusterParticipation
                      = globalHydroParticipationIndex;
                    reserveParticipation.areaIndexClusterParticipation
                      = areaClusterParticipationIndex;

                    areaReserves.areaCapacityReservations[areaReserveIdx]
                      .AllHydroReservesParticipation.push_back(std::move(reserveParticipation));

                    for (const auto& symIdx:
                         hydro.reserveParticipationContainer.value().symmetricalIndices(reserveID))
                    {
                        if (areaReserves.HydroReservesParticipationSymmetries.size() <= symIdx)
                        {
                            areaReserves.HydroReservesParticipationSymmetries.resize(
                              hydro.reserveParticipationContainer.value().getNbSymGroups());
                        }
                        areaReserves.HydroReservesParticipationSymmetries[symIdx].push_back(
                          {reserveID,
                           areaReserves.areaCapacityReservations[areaReserveIdx]
                             .AllHydroReservesParticipation.back()});
                    }

                    ++globalHydroParticipationIndex;
                    ++areaClusterParticipationIndex;
                }
                ++areaReserveIdx;
            }
        };
    }
}

void SIM_InitialisationProblemeHebdo(Study& study,
                                     PROBLEME_HEBDO& problem,
                                     unsigned int NombreDePasDeTemps,
                                     uint numspace)
{
    int NombrePaliers;

    auto& parameters = study.parameters;

    // For hybrid studies
    problem.modelerData = study.getModelerData();

    problem.Expansion = (parameters.mode == Data::SimulationMode::Expansion);

    // gp adq : to be removed
    if (parameters.adqPatchParams.enabled)
    {
        problem.adequacyPatchRuntimeData = std::make_shared<AdequacyPatchRuntimeData>(
          study.areas,
          study.runtime.areaLink);
    }

    SIM_AllocationProblemeHebdo(study, problem, NombreDePasDeTemps);

    problem.NombreDePasDeTemps = NombreDePasDeTemps;

    problem.NombreDePasDeTempsDUneJournee = (int)(NombreDePasDeTemps / 7);

    problem.NombreDePays = study.areas.size();

    problem.NombreDInterconnexions = study.runtime.interconnectionsCount();

    problem.NumberOfShortTermStorages = study.runtime.counts.shortTermStorages;

    auto activeConstraints = study.bindingConstraints.activeConstraints();
    problem.NombreDeContraintesCouplantes = activeConstraints.size();

    problem.ExportMPS = study.parameters.include.exportMPS;
    problem.exportSolutions = study.parameters.include.exportSolutions;
    problem.NamedProblems = study.parameters.namedProblems;
    problem.exportMPSOnError = Data::exportMPS(parameters.include.unfeasibleProblemBehavior);

    problem.OptimisationNotFastMode = (study.parameters.unitCommitment.ucMode
                                       != Antares::Data::UnitCommitmentMode::ucHeuristicFast);

    auto workflow = Solver::Workflow::getWorkflow(study);

    problem.OptimisationAvecVariablesEntieres = workflow.subproblems
                                                == Solver::Workflow::SolverType::MILP;
    problem.useThermalHeuristic = workflow.useThermalHeuristic;

    problem.OptimisationAuPasHebdomadaire = (parameters.simplexOptimizationRange == Data::sorWeek);

    switch (parameters.power.fluctuations)
    {
    case Data::lssFreeModulations:
        problem.TypeDeLissageHydraulique = PAS_DE_LISSAGE_HYDRAULIQUE;
        break;
    case Data::lssMinimizeRamping:
        problem.TypeDeLissageHydraulique = LISSAGE_HYDRAULIQUE_SUR_SOMME_DES_VARIATIONS;
        break;
    case Data::lssMinimizeExcursions:
        problem.TypeDeLissageHydraulique = LISSAGE_HYDRAULIQUE_SUR_VARIATION_MAX;
        break;
    case Data::lssUnknown:
        problem.TypeDeLissageHydraulique = PAS_DE_LISSAGE_HYDRAULIQUE;
        assert(false && "invalid power fluctuations");
        break;
    }

    Antares::Data::Area::ScratchMap scratchmap = study.areas.buildScratchMap(numspace);

    for (uint i = 0; i != study.areas.size(); i++)
    {
        const auto& area = *(study.areas[i]);
        const auto& scratchpad = scratchmap.at(&area);

        problem.NomsDesPays[i] = area.id.c_str();

        problem.CoutDeDefaillancePositive[i] = area.thermal.unsuppliedEnergyCost;

        problem.CoutDeDefaillanceNegative[i] = area.thermal.spilledEnergyCost;

        // Hydraulic
        problem.CoutDeDebordement[i] = area.thermal.spilledEnergyCost
                                       + area.hydro.overflowSpilledCostDifference;

        problem.DefaillanceNegativeUtiliserPMinThermique[i] = (anoOtherDispatchPower
                                                               & area.nodalOptimization)
                                                              != 0;
        problem.DefaillanceNegativeUtiliserHydro[i] = (anoDispatchHydroPower
                                                       & area.nodalOptimization)
                                                      != 0;
        problem.DefaillanceNegativeUtiliserConsoAbattue[i] = (anoNonDispatchPower
                                                              & area.nodalOptimization)
                                                             != 0;

        problem.CaracteristiquesHydrauliques[i].PresenceDHydrauliqueModulable = scratchpad
                                                                                  .hydroHasMod;

        problem.CaracteristiquesHydrauliques[i].PresenceDePompageModulable
          = area.hydro.reservoirManagement && scratchpad.pumpHasMod
            && area.hydro.pumpingEfficiency > 0.
            && problem.CaracteristiquesHydrauliques[i].PresenceDHydrauliqueModulable;

        problem.CaracteristiquesHydrauliques[i].PumpingRatio = area.hydro.pumpingEfficiency;

        problem.CaracteristiquesHydrauliques[i].SansHeuristique = area.hydro.reservoirManagement
                                                                  && !area.hydro.useHeuristicTarget;

        problem.CaracteristiquesHydrauliques[i].TurbinageEntreBornes = area.hydro
                                                                         .reservoirManagement
                                                                       && (!area.hydro
                                                                              .useHeuristicTarget
                                                                           || area.hydro.useLeeway);

        problem.CaracteristiquesHydrauliques[i].SuiviNiveauHoraire = area.hydro.reservoirManagement;

        problem.CaracteristiquesHydrauliques[i].DirectLevelAccess = false;
        problem.CaracteristiquesHydrauliques[i].AccurateWaterValue = false;
        if (study.parameters.hydroPricing.hpMode == Antares::Data::HydroPricingMode::hpMILP
            && area.hydro.useWaterValue)
        {
            problem.CaracteristiquesHydrauliques[i].AccurateWaterValue = true;
            problem.CaracteristiquesHydrauliques[i].DirectLevelAccess = true;
        }

        problem.CaracteristiquesHydrauliques[i].TailleReservoir = area.hydro.reservoirCapacity;

        for (unsigned pdt = 0; pdt < NombreDePasDeTemps; pdt++)
        {
            problem.CaracteristiquesHydrauliques[i].NiveauHoraireInf[pdt] = 0;
            problem.CaracteristiquesHydrauliques[i].NiveauHoraireSup[pdt]
              = problem.CaracteristiquesHydrauliques[i].TailleReservoir;
        }

        problem.previousSimulationFinalLevel[i] = -1.;

        problem.CaracteristiquesHydrauliques[i].WeeklyWaterValueStateRegular = 0.;

        problem.CaracteristiquesHydrauliques[i].WeeklyGeneratingModulation = 1.;
        problem.CaracteristiquesHydrauliques[i].WeeklyPumpingModulation = 1.;

        assert(area.hydro.intraDailyModulation >= 1. && "Intra-daily modulation must be >= 1.0");
        problem.CoefficientEcretementPMaxHydraulique[i] = area.hydro.intraDailyModulation;
    }

    if (parameters.include.reserves)
    {
        importCapacityReservations(study.areas, problem);
        importHydroReserves(study.areas, problem);
    }

    importShortTermStorages(study.parameters, study.areas, problem.ShortTermStorage, problem);

    for (uint i = 0; i < study.runtime.interconnectionsCount(); ++i)
    {
        auto& link = *(study.runtime.areaLink[i]);
        problem.PaysOrigineDeLInterconnexion[i] = link.from->index;
        problem.PaysExtremiteDeLInterconnexion[i] = link.with->index;
    }

    for (unsigned constraintIndex = 0; constraintIndex < activeConstraints.size();
         constraintIndex++)
    {
        auto bc = activeConstraints[constraintIndex];
        CONTRAINTES_COUPLANTES& PtMat = problem.MatriceDesContraintesCouplantes[constraintIndex];
        PtMat.bindingConstraint = bc;
        PtMat.NombreDInterconnexionsDansLaContrainteCouplante = bc->linkCount();
        PtMat.NombreDePaliersDispatchDansLaContrainteCouplante = bc->clusterCount();
        PtMat.NomDeLaContrainteCouplante = bc->name().c_str();
        switch (bc->type())
        {
        case BindingConstraint::typeHourly:
            PtMat.TypeDeContrainteCouplante = CONTRAINTE_HORAIRE;
            break;
        case BindingConstraint::typeDaily:
            PtMat.TypeDeContrainteCouplante = CONTRAINTE_JOURNALIERE;
            break;
        case BindingConstraint::typeWeekly:
            PtMat.TypeDeContrainteCouplante = CONTRAINTE_HEBDOMADAIRE;
            break;
        case BindingConstraint::typeUnknown:
        case BindingConstraint::typeMax:
            assert(false && "Invalid constraint");
            break;
        }
        PtMat.SensDeLaContrainteCouplante = *Antares::Data::BindingConstraint::
                                              MathOperatorToCString(bc->operatorType());

        BindingConstraintStructures bindingConstraintStructures = bc->initLinkArrays();
        for (uint j = 0; j < bc->linkCount(); ++j)
        {
            PtMat.NumeroDeLInterconnexion[j] = bindingConstraintStructures.linkIndex[j];
            PtMat.PoidsDeLInterconnexion[j] = bindingConstraintStructures.linkWeight[j];

            PtMat.OffsetTemporelSurLInterco[j] = bindingConstraintStructures.linkOffset[j];
        }

        for (uint j = 0; j < bc->clusterCount(); ++j)
        {
            PtMat.NumeroDuPalierDispatch[j] = bindingConstraintStructures.clusterIndex[j];
            PtMat.PaysDuPalierDispatch[j] = bindingConstraintStructures.clustersAreaIndex[j];
            PtMat.PoidsDuPalierDispatch[j] = bindingConstraintStructures.clusterWeight[j];

            PtMat.OffsetTemporelSurLePalierDispatch[j] = bindingConstraintStructures
                                                           .clusterOffset[j];
        }
    }

    NombrePaliers = 0;
    int globalReserveIndex = 0;
    int globalThermalClusterParticipationIndex = 0;
    for (uint i = 0; i < study.areas.size(); ++i)
    {
        int areaReserveIndex = 0;
        int areaClusterParticipationIndex = 0;
        auto& area = *(study.areas.byIndex[i]);
        auto& pbPalier = problem.PaliersThermiquesDuPays[i];
        unsigned int clusterCount = area.thermal.list.enabledAndNotMustRunCount();
        pbPalier.NombreDePaliersThermiques = clusterCount;

        for (const auto& cluster: area.thermal.list.each_enabled_and_not_mustrun())
        {
            cluster->globalIndex = static_cast<int>(NombrePaliers + cluster->index);
            pbPalier.NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster->index]
              = cluster->globalIndex;
            pbPalier.TailleUnitaireDUnGroupeDuPalierThermique[cluster->index]
              = cluster->nominalCapacityWithSpinning;
            pbPalier.PminDuPalierThermiquePendantUneHeure[cluster->index] = cluster->minStablePower;
            pbPalier.PminDuPalierThermiquePendantUnJour[cluster->index] = 0;
            pbPalier.minUpDownTime[cluster->index] = cluster->minUpDownTime;

            pbPalier.CoutDeDemarrageDUnGroupeDuPalierThermique[cluster->index] = cluster
                                                                                   ->startupCost;
            pbPalier.CoutDArretDUnGroupeDuPalierThermique[cluster->index] = 0;
            pbPalier.CoutFixeDeMarcheDUnGroupeDuPalierThermique[cluster->index] = cluster
                                                                                    ->fixedCost;
            pbPalier.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[cluster->index]
              = cluster->minUpTime;
            pbPalier.DureeMinimaleDArretDUnGroupeDuPalierThermique[cluster->index]
              = cluster->minDownTime;

            pbPalier.PmaxDUnGroupeDuPalierThermique[cluster->index]
              = cluster->nominalCapacityWithSpinning;
            pbPalier.pminDUnGroupeDuPalierThermique[cluster->index]
              = (pbPalier.PmaxDUnGroupeDuPalierThermique[cluster->index] < cluster->minStablePower)
                  ? pbPalier.PmaxDUnGroupeDuPalierThermique[cluster->index]
                  : cluster->minStablePower;
            pbPalier.NomsDesPaliersThermiques[cluster->index] = cluster->name().c_str();
        }

        if (study.parameters.unitCommitment.ucMode
              != Antares::Data::UnitCommitmentMode::ucHeuristicFast
            && study.parameters.include.reserves && area.allCapacityReservations)
        {
            auto& areaReserves = problem.allReserves.value()[i];

            int areaReserveIdx = 0;
            for (const auto& [reserveID, _]:
                 area.allCapacityReservations.value().areaCapacityReservations)
            {
                for (auto& cluster: area.thermal.list.all())
                {
                    if (cluster->reserveParticipationContainer
                        && cluster->reserveParticipationContainer.value().isParticipatingInReserve(
                          reserveID)
                        && cluster->isEnabled())
                    {
                        RESERVE_PARTICIPATION_THERMAL reserveParticipation;
                        reserveParticipation.maxPower = cluster->reserveParticipationContainer
                                                          .value()
                                                          .reserveMaxPower(reserveID);
                        reserveParticipation.participationCost = cluster
                                                                   ->reserveParticipationContainer
                                                                   .value()
                                                                   .reserveCost(reserveID);
                        reserveParticipation.maxPowerOff = cluster->reserveParticipationContainer
                                                             .value()
                                                             .reserveMaxPowerOff(reserveID);
                        reserveParticipation.participationCostOff
                          = cluster->reserveParticipationContainer.value().reserveCostOff(
                            reserveID);
                        reserveParticipation.clusterName = cluster->name();
                        reserveParticipation.clusterIdInArea = cluster->index;
                        reserveParticipation.clusterId = NombrePaliers + cluster->index;
                        reserveParticipation.globalIndexClusterParticipation
                          = globalThermalClusterParticipationIndex;
                        reserveParticipation.areaIndexClusterParticipation
                          = areaClusterParticipationIndex;

                        areaReserves.areaCapacityReservations[areaReserveIdx]
                          .AllThermalReservesParticipation.emplace(cluster->index,
                                                                   reserveParticipation);

                        for (const auto& symIdx:
                             cluster->reserveParticipationContainer.value().symmetricalIndices(
                               reserveID))
                        {
                            auto& symmetries = areaReserves.ThermalReservesParticipationSymmetries
                                                 [cluster->index];
                            if (symmetries.size() <= symIdx)
                            {
                                symmetries.resize(
                                  cluster->reserveParticipationContainer.value().getNbSymGroups());
                            }
                            symmetries[symIdx].emplace_back(
                              reserveID,
                              areaReserves.areaCapacityReservations[areaReserveIdx]
                                .AllThermalReservesParticipation[cluster->index]);
                        }

                        ++globalThermalClusterParticipationIndex;
                        ++areaClusterParticipationIndex;
                    }
                }
                ++areaReserveIdx;
            };
        }

        NombrePaliers += clusterCount;
    }

    problem.NombreDePaliersThermiques = NombrePaliers;

    problem.LeProblemeADejaEteInstancie = false;
}

void SIM_RenseignementProblemeHebdo(const Study& study,
                                    PROBLEME_HEBDO& problem,
                                    uint weekInTheYear,
                                    const int PasDeTempsDebut,
                                    const HYDRO_VENTILATION_RESULTS& hydroVentilationResults,
                                    const Antares::Data::Area::ScratchMap& scratchmap)
{
    const auto& parameters = study.parameters;
    auto& studyruntime = study.runtime;
    const uint nbPays = study.areas.size();
    const size_t pasDeTempsSizeDouble = problem.NombreDePasDeTemps * sizeof(double);

    const uint weekFirstDay = study.calendar.hours[PasDeTempsDebut].dayYear;
    const unsigned weekLastDay = weekFirstDay + 6;

    for (int opt = 0; opt < 7; opt++)
    {
        problem.coutOptimalSolution1[opt] = 0.;
        problem.coutOptimalSolution2[opt] = 0.;
    }

    for (uint k = 0; k < studyruntime.interconnectionsCount(); ++k)
    {
        auto* lnk = studyruntime.areaLink[k];

        if (lnk->useHurdlesCost)
        {
            COUTS_DE_TRANSPORT& couts = problem.CoutDeTransport[k];
            couts.IntercoGereeAvecDesCouts = true;
            const double* direct = ((const double*)((void*)lnk->parameters[fhlHurdlesCostDirect]))
                                   + PasDeTempsDebut;
            const double* indirect = ((const double*)((void*)
                                                        lnk->parameters[fhlHurdlesCostIndirect]))
                                     + PasDeTempsDebut;
            memcpy(&couts.CoutDeTransportOrigineVersExtremite[0], direct, pasDeTempsSizeDouble);
            memcpy(&couts.CoutDeTransportOrigineVersExtremiteRef[0], direct, pasDeTempsSizeDouble);
            memcpy(&couts.CoutDeTransportExtremiteVersOrigine[0], indirect, pasDeTempsSizeDouble);
            memcpy(&couts.CoutDeTransportExtremiteVersOrigineRef[0],
                   indirect,
                   pasDeTempsSizeDouble);
        }
        else
        {
            problem.CoutDeTransport[k].IntercoGereeAvecDesCouts = false;
        }

        problem.CoutDeTransport[k].IntercoGereeAvecLoopFlow = lnk->useLoopFlow;
    }

    int weekDayIndex[8];
    for (int day = 0; day < 8; day++)
    {
        weekDayIndex[day] = study.calendar.hours[PasDeTempsDebut + day * 24].dayYear;
    }

    double levelInterpolBeg;
    double levelInterpolEnd;
    double delta;

    for (uint k = 0; k < nbPays; ++k)
    {
        auto& area = *study.areas.byIndex[k];

        if (area.hydro.reservoirManagement)
        {
            double& nivInit = problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir;
            nivInit = problem.previousSimulationFinalLevel[k];
            if (not problem.CaracteristiquesHydrauliques[k].TurbinageEntreBornes)
            {
                nivInit = hydroVentilationResults[k].NiveauxReservoirsDebutJours[weekFirstDay]
                          * area.hydro.reservoirCapacity;
                problem.previousSimulationFinalLevel[k] = nivInit;
            }

            if (nivInit < -LEVEL_TOLERANCE_MWH)
            {
                std::ostringstream msg;
                msg << "Area " << area.name << ", week " << weekInTheYear + 1
                    << " : initial level < 0";
                throw FatalError(msg.str());
            }

            if (nivInit > area.hydro.reservoirCapacity + LEVEL_TOLERANCE_MWH)
            {
                std::ostringstream msg;
                msg << "Area " << area.name << ", week " << weekInTheYear + 1
                    << " : initial level over capacity";
                throw FatalError(msg.str());
            }

            if (area.hydro.powerToLevel)
            {
                problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation = Antares::Data::
                  getWeeklyModulation(nivInit * 100 / area.hydro.reservoirCapacity,
                                      area.hydro.creditModulation,
                                      Data::PartHydro::genMod);

                problem.CaracteristiquesHydrauliques[k].WeeklyPumpingModulation = Antares::Data::
                  getWeeklyModulation(nivInit * 100 / area.hydro.reservoirCapacity,
                                      area.hydro.creditModulation,
                                      Data::PartHydro::pumpMod);
            }

            if (area.hydro.useWaterValue)
            {
                problem.CaracteristiquesHydrauliques[k].WeeklyWaterValueStateRegular
                  = getWaterValue(nivInit * 100 / area.hydro.reservoirCapacity,
                                  area.hydro.waterValues,
                                  weekLastDay);
            }

            if (problem.CaracteristiquesHydrauliques[k].PresenceDHydrauliqueModulable)
            {
                if (area.hydro.hardBoundsOnRuleCurves
                    && problem.CaracteristiquesHydrauliques[k].SuiviNiveauHoraire)
                {
                    const auto* minLvl = area.hydro.series->ruleCurves.min.getColumn(problem.year);
                    const auto* maxLvl = area.hydro.series->ruleCurves.max.getColumn(problem.year);

                    for (int day = 0; day < 7; day++)
                    {
                        levelInterpolBeg = minLvl[weekDayIndex[day]]
                                           * problem.CaracteristiquesHydrauliques[k]
                                               .TailleReservoir;
                        levelInterpolEnd = minLvl[weekDayIndex[day + 1]]
                                           * problem.CaracteristiquesHydrauliques[k]
                                               .TailleReservoir;
                        delta = (levelInterpolEnd - levelInterpolBeg) / 24.;

                        for (int hour = 0; hour < 24; hour++)
                        {
                            problem.CaracteristiquesHydrauliques[k]
                              .NiveauHoraireInf[24 * day + hour]
                              = levelInterpolBeg + hour * delta;
                        }

                        levelInterpolBeg = maxLvl[weekDayIndex[day]]
                                           * problem.CaracteristiquesHydrauliques[k]
                                               .TailleReservoir;
                        levelInterpolEnd = maxLvl[weekDayIndex[day + 1]]
                                           * problem.CaracteristiquesHydrauliques[k]
                                               .TailleReservoir;
                        delta = (levelInterpolEnd - levelInterpolBeg) / 24.;

                        for (int hour = 0; hour < 24; hour++)
                        {
                            problem.CaracteristiquesHydrauliques[k]
                              .NiveauHoraireSup[24 * day + hour]
                              = levelInterpolBeg + hour * delta;
                        }
                    }
                }
            }
            if (problem.CaracteristiquesHydrauliques[k].AccurateWaterValue)
            {
                for (uint layerindex = 0; layerindex < 100; layerindex++)
                {
                    problem.CaracteristiquesHydrauliques[k].WaterLayerValues[layerindex]
                      = area.hydro.waterValues[layerindex][weekLastDay];
                }
            }
        }
    }

    unsigned int year = problem.year;

    uint linkCount = studyruntime.interconnectionsCount();
    for (uint k = 0; k != linkCount; ++k)
    {
        int hourInYear = PasDeTempsDebut;
        auto& lnk = *(studyruntime.areaLink[k]);
        const double* directCapacities = lnk.directCapacities.getColumn(year);
        const double* indirectCapacities = lnk.indirectCapacities.getColumn(year);
        for (unsigned hourInWeek = 0; hourInWeek < problem.NombreDePasDeTemps;
             ++hourInWeek, ++hourInYear)
        {
            VALEURS_DE_NTC_ET_RESISTANCES& ntc = problem.ValeursDeNTC[hourInWeek];

            ntc.ValeurDeNTCOrigineVersExtremite[k] = directCapacities[hourInYear];
            ntc.ValeurDeNTCExtremiteVersOrigine[k] = indirectCapacities[hourInYear];
            ntc.ValeurDeLoopFlowOrigineVersExtremite[k] = lnk.parameters[fhlLoopFlow][hourInYear];
        }
    }

    Simulation::setBindingConstraintsRHS(problem,
                                         study.bindingConstraints,
                                         study.bindingConstraintsGroups,
                                         PasDeTempsDebut,
                                         weekFirstDay);

    int hourInYear = PasDeTempsDebut;
    for (unsigned hourInWeek = 0; hourInWeek < problem.NombreDePasDeTemps;
         ++hourInWeek, ++hourInYear)
    {
        for (uint k = 0; k < nbPays; ++k)
        {
            auto& area = *(study.areas.byIndex[k]);
            const auto& scratchpad = scratchmap.at(&area);
            const double hourlyLoad = area.load.series.getCoefficient(year, hourInYear);
            const double hourlyWind = area.wind.series.getCoefficient(year, hourInYear);
            const double hourlySolar = area.solar.series.getCoefficient(year, hourInYear);
            const double hourlyROR = area.hydro.series->ror.getCoefficient(year, hourInYear);
            const double hourlyHydroGenPower = area.hydro.series->maxHourlyGenPower
                                                 .getCoefficient(year, hourInYear);
            const double hourlyHydroPumpPower = area.hydro.series->maxHourlyPumpPower
                                                  .getCoefficient(year, hourInYear);

            double& mustRunGen = problem.AllMustRunGeneration[hourInWeek]
                                   .AllMustRunGenerationOfArea[k];
            if (parameters.renewableGeneration.isAggregated())
            {
                mustRunGen = hourlyWind + hourlySolar + scratchpad.miscGenSum[hourInYear]
                             + hourlyROR + scratchpad.mustrunSum[hourInYear];
            }

            // Renewable
            if (parameters.renewableGeneration.isClusters())
            {
                mustRunGen = scratchpad.miscGenSum[hourInYear] + hourlyROR
                             + scratchpad.mustrunSum[hourInYear];

                for (const auto& c: area.renewable.list.each_enabled())
                {
                    mustRunGen += c->valueAtTimeStep(year, hourInYear);
                }
            }

            assert(
              !std::isnan(problem.AllMustRunGeneration[hourInWeek].AllMustRunGenerationOfArea[k])
              && "NaN detected for 'AllMustRunGeneration', probably from miscGenSum/mustrunSum");

            problem.ConsommationsAbattues[hourInWeek].ConsommationAbattueDuPays[k]
              = +hourlyLoad
                - problem.AllMustRunGeneration[hourInWeek].AllMustRunGenerationOfArea[k];

            if (problem.CaracteristiquesHydrauliques[k].PresenceDHydrauliqueModulable)
            {
                problem.CaracteristiquesHydrauliques[k]
                  .ContrainteDePmaxHydrauliqueHoraire[hourInWeek]
                  = hourlyHydroGenPower
                    * problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation;
            }

            if (problem.CaracteristiquesHydrauliques[k].PresenceDePompageModulable)
            {
                problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxPompageHoraire[hourInWeek]
                  = hourlyHydroPumpPower
                    * problem.CaracteristiquesHydrauliques[k].WeeklyPumpingModulation;
            }

            problem.ReserveJMoins1[k].ReserveHoraireJMoins1[hourInWeek] = area.reserves
                                                                            [fhrDayBefore]
                                                                            [PasDeTempsDebut
                                                                             + hourInWeek];
        }
    }
    {
        for (uint k = 0; k < nbPays; ++k)
        {
            if (problem.CaracteristiquesHydrauliques[k].PresenceDHydrauliqueModulable)
            {
                auto& area = *study.areas.byIndex[k];
                const auto& scratchpad = scratchmap.at(&area);
                auto& hydroSeries = area.hydro.series;

                const auto& dailyMeanMaxGenPower = scratchpad.meanMaxDailyGenPower.getColumn(year);
                const auto& dailyMeanMaxPumpPower = scratchpad.meanMaxDailyPumpPower.getColumn(
                  year);
                const auto& srcinflows = hydroSeries->storage.getColumn(year);
                const auto& srcmingen = hydroSeries->mingen.getColumn(year);

                for (uint j = 0; j < problem.NombreDePasDeTemps; ++j)
                {
                    problem.CaracteristiquesHydrauliques[k].MingenHoraire[j] = srcmingen
                      [PasDeTempsDebut + j];
                }

                if (area.hydro.reservoirManagement)
                {
                    if (not area.hydro.useHeuristicTarget
                        || (problem.CaracteristiquesHydrauliques[k].PresenceDePompageModulable
                            && problem.OptimisationAuPasHebdomadaire))
                    {
                        for (uint j = 0; j < 7; ++j)
                        {
                            uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                            problem.CaracteristiquesHydrauliques[k]
                              .MinEnergieHydrauParIntervalleOptimise[j]
                              = 0.;
                            problem.CaracteristiquesHydrauliques[k]
                              .MaxEnergieHydrauParIntervalleOptimise[j]
                              = dailyMeanMaxGenPower[day] * area.hydro.dailyNbHoursAtGenPmax[0][day]
                                * problem.CaracteristiquesHydrauliques[k]
                                    .WeeklyGeneratingModulation;
                        }
                    }

                    if (area.hydro.useHeuristicTarget
                        && (area.hydro.useLeeway
                            || (problem.CaracteristiquesHydrauliques[k].PresenceDePompageModulable
                                && !problem.OptimisationAuPasHebdomadaire)))
                    {
                        std::vector<double>& DGU = problem.CaracteristiquesHydrauliques[k]
                                                     .MaxEnergieHydrauParIntervalleOptimise;

                        std::vector<double>& DGL = problem.CaracteristiquesHydrauliques[k]
                                                     .MinEnergieHydrauParIntervalleOptimise;

                        const std::vector<double>& DNT = hydroVentilationResults[k]
                                                           .HydrauliqueModulableQuotidien;

                        double WSL = problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir;

                        double LUB = area.hydro.leewayUpperBound;
                        if (!area.hydro.useLeeway)
                        {
                            LUB = 1;
                        }
                        double LLB = area.hydro.leewayLowerBound;
                        if (!area.hydro.useLeeway)
                        {
                            LLB = 1;
                        }
                        double DGM = problem.CaracteristiquesHydrauliques[k]
                                       .WeeklyGeneratingModulation;

                        double rc = area.hydro.reservoirCapacity;

                        double WNI = 0.;
                        for (uint j = 0; j < 7; ++j)
                        {
                            uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                            WNI += srcinflows[day];
                        }

                        std::vector<double> DGU_tmp(7, -1.);
                        std::vector<double> DGL_tmp(7, -1.);

                        double WGU = 0.;

                        for (uint j = 0; j < 7; ++j)
                        {
                            uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                            double DGC = dailyMeanMaxGenPower[day]
                                         * area.hydro.dailyNbHoursAtGenPmax[0][day];

                            DGU_tmp[j] = DNT[day] * LUB;
                            DGL_tmp[j] = DNT[day] * LLB;
                            double DGCxDGM = DGC * DGM;

                            if (DGCxDGM < DGL_tmp[j])
                            {
                                DGU_tmp[j] = DGCxDGM;
                                DGL_tmp[j] = DGCxDGM;
                            }

                            if (DGCxDGM > DGL_tmp[j] && DGCxDGM < DGU_tmp[j])
                            {
                                DGU_tmp[j] = DGCxDGM;
                            }

                            WGU += DGU_tmp[j];
                        }

                        for (uint j = 0; j < 7; ++j)
                        {
                            if (not area.hydro.hardBoundsOnRuleCurves)
                            {
                                if (Utils::isZero(WGU))
                                {
                                    DGU[j] = 0.;
                                }
                                else
                                {
                                    DGU[j] = DGU_tmp[j] * std::min(WGU, WSL + WNI) / WGU;
                                }
                            }
                            else
                            {
                                const uint nextWeekFirstDay = study.calendar
                                                                .hours[PasDeTempsDebut + 7 * 24]
                                                                .dayYear;

                                const auto& minLvl = area.hydro.series->ruleCurves.min.getColumn(
                                  problem.year);
                                double V = std::max(0., WSL - minLvl[nextWeekFirstDay] * rc + WNI);

                                if (Utils::isZero(WGU))
                                {
                                    DGU[j] = 0.;
                                }
                                else
                                {
                                    DGU[j] = DGU_tmp[j] * std::min(WGU, V) / WGU;
                                }
                            }

                            DGL[j] = std::min(DGU[j], DGL_tmp[j]);
                        }
                    }
                }

                double weekGenerationTarget = 1.;
                double marginGen = 1.;

                if (area.hydro.reservoirManagement && area.hydro.useHeuristicTarget
                    && not area.hydro.useLeeway)
                {
                    double weekTarget_tmp = 0.;
                    for (uint j = 0; j < 7; ++j)
                    {
                        uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                        weekTarget_tmp += hydroVentilationResults[k]
                                            .HydrauliqueModulableQuotidien[day];
                    }

                    if (weekTarget_tmp != 0.)
                    {
                        weekGenerationTarget = weekTarget_tmp;
                    }

                    marginGen = weekGenerationTarget;
                }

                if (not problem.CaracteristiquesHydrauliques[k].TurbinageEntreBornes)
                {
                    for (uint j = 0; j < 7; ++j)
                    {
                        uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                        problem.CaracteristiquesHydrauliques[k]
                          .CntEnergieH2OParIntervalleOptimise[j]
                          = hydroVentilationResults[k].HydrauliqueModulableQuotidien[day]
                            * problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation
                            * marginGen / weekGenerationTarget;
                    }
                }

                for (uint j = 0; j < 7; ++j)
                {
                    uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                    problem.CaracteristiquesHydrauliques[k].InflowForTimeInterval[j] = srcinflows
                      [day];
                    for (int h = 0; h < 24; h++)
                    {
                        problem.CaracteristiquesHydrauliques[k].ApportNaturelHoraire[j * 24 + h]
                          = srcinflows[day] / 24;
                    }
                }

                if (problem.CaracteristiquesHydrauliques[k].PresenceDePompageModulable)
                {
                    if (area.hydro.reservoirManagement) /* No need to include the condition "use
                                                           water value" */
                    {
                        if (problem.CaracteristiquesHydrauliques[k].SuiviNiveauHoraire)
                        {
                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                                problem.CaracteristiquesHydrauliques[k]
                                  .MaxEnergiePompageParIntervalleOptimise[j]
                                  = dailyMeanMaxPumpPower[day]
                                    * area.hydro.dailyNbHoursAtPumpPmax[0][day]
                                    * problem.CaracteristiquesHydrauliques[k]
                                        .WeeklyPumpingModulation;
                            }
                        }

                        if (!problem.CaracteristiquesHydrauliques[k].SuiviNiveauHoraire)
                        {
                            double WNI = 0.;
                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                                WNI += srcinflows[day];
                            }

                            std::vector<double>& DPU = problem.CaracteristiquesHydrauliques[k]
                                                         .MaxEnergiePompageParIntervalleOptimise;

                            double WSL = problem.CaracteristiquesHydrauliques[k]
                                           .NiveauInitialReservoir;

                            double DPM = problem.CaracteristiquesHydrauliques[k]
                                           .WeeklyPumpingModulation;

                            double pumping_ratio = area.hydro.pumpingEfficiency;

                            double WPU = 0.;

                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                                double DPC = dailyMeanMaxPumpPower[day]
                                             * area.hydro.dailyNbHoursAtPumpPmax[0][day];

                                WPU += DPC;
                            }

                            double U = WPU * DPM * pumping_ratio;

                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                                double DPC = dailyMeanMaxPumpPower[day]
                                             * area.hydro.dailyNbHoursAtPumpPmax[0][day];
                                double rc = area.hydro.reservoirCapacity;

                                if (not area.hydro.hardBoundsOnRuleCurves)
                                {
                                    double V = std::max(0., rc - (WNI + WSL));

                                    if (Utils::isZero(U))
                                    {
                                        DPU[j] = 0.;
                                    }
                                    else
                                    {
                                        DPU[j] = DPC * DPM * std::min(U, V) / U;
                                    }
                                }
                                else
                                {
                                    const uint nextWeekFirstDay = study.calendar
                                                                    .hours[PasDeTempsDebut + 7 * 24]
                                                                    .dayYear;

                                    const auto* maxLvl = area.hydro.series->ruleCurves.max
                                                           .getColumn(problem.year);

                                    double V = std::max(0.,
                                                        maxLvl[nextWeekFirstDay] * rc
                                                          - (WNI + WSL));

                                    if (Utils::isZero(U))
                                    {
                                        DPU[j] = 0.;
                                    }
                                    else
                                    {
                                        DPU[j] = DPC * DPM * std::min(U, V) / U;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    for (uint k = 0; k < nbPays; ++k)
    {
        problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxHydrauliqueHoraireRef
          = problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxHydrauliqueHoraire;
    }
}
