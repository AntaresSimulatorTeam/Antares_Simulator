/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/

#include <cmath>
#include <sstream>

#include <antares/antares/fatal-error.h>
#include <antares/study/area/constants.h>
#include <antares/study/area/scratchpad.h>
#include <antares/study/study.h>
#include <antares/utils/utils.h>
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_extern_variables_globales.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/study/fwd.h"
#include "antares/study/simulation.h"

using namespace Antares;
using namespace Antares::Data;

static void importShortTermStorages(
  const AreaList& areas,
  std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorageOut)
{
    int clusterGlobalIndex = 0;
    for (uint areaIndex = 0; areaIndex != areas.size(); areaIndex++)
    {
        ShortTermStorageOut[areaIndex].resize(areas[areaIndex]->shortTermStorage.count());
        int storageIndex = 0;
        for (const auto& st: areas[areaIndex]->shortTermStorage.storagesByIndex)
        {
            ::ShortTermStorage::PROPERTIES& toInsert = ShortTermStorageOut[areaIndex][storageIndex];
            toInsert.clusterGlobalIndex = clusterGlobalIndex;

            // Properties
            toInsert.reservoirCapacity = st.properties.reservoirCapacity.value();
            toInsert.injectionEfficiency = st.properties.injectionEfficiency;
            toInsert.withdrawalEfficiency = st.properties.withdrawalEfficiency;
            toInsert.injectionNominalCapacity = st.properties.injectionNominalCapacity.value();
            toInsert.withdrawalNominalCapacity = st.properties.withdrawalNominalCapacity.value();
            toInsert.initialLevel = st.properties.initialLevel;
            toInsert.initialLevelOptim = st.properties.initialLevelOptim;
            toInsert.name = st.properties.name;

            toInsert.series = st.series;

            // TODO add missing properties, or use the same struct
            storageIndex++;
            clusterGlobalIndex++;
        }
    }
}

void SIM_InitialisationProblemeHebdo(Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     unsigned int NombreDePasDeTemps,
                                     uint numspace)
{
    int NombrePaliers;

    auto& parameters = study.parameters;

    problem.Expansion = (parameters.mode == Data::SimulationMode::Expansion);
    problem.firstWeekOfSimulation = false;

    // gp adq : to be removed
    if (parameters.adqPatchParams.enabled)
    {
        problem.adequacyPatchRuntimeData = std::make_shared<AdequacyPatchRuntimeData>(
          study.areas,
          study.runtime.areaLink);
    }

    problem.WaterValueAccurate = (study.parameters.hydroPricing.hpMode
                                  == Antares::Data::HydroPricingMode::hpMILP);

    SIM_AllocationProblemeHebdo(study, problem, NombreDePasDeTemps);

    problem.NombreDePasDeTemps = NombreDePasDeTemps;

    problem.NombreDePasDeTempsDUneJournee = (int)(NombreDePasDeTemps / 7);

    problem.NombreDePays = study.areas.size();

    problem.NombreDInterconnexions = study.runtime.interconnectionsCount();

    problem.NumberOfShortTermStorages = study.runtime.shortTermStorageCount;

    auto activeConstraints = study.bindingConstraints.activeConstraints();
    problem.NombreDeContraintesCouplantes = activeConstraints.size();

    problem.ExportMPS = study.parameters.include.exportMPS;
    problem.ExportStructure = study.parameters.include.exportStructure;
    problem.NamedProblems = study.parameters.namedProblems;
    problem.exportMPSOnError = Data::exportMPS(parameters.include.unfeasibleProblemBehavior);

    problem.OptimisationAvecCoutsDeDemarrage = (study.parameters.unitCommitment.ucMode
                                                != Antares::Data::UnitCommitmentMode::
                                                  ucHeuristicFast);

    problem.OptimisationAvecVariablesEntieres = (study.parameters.unitCommitment.ucMode
                                                 == Antares::Data::UnitCommitmentMode::ucMILP);

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

        problem.CoutDeDefaillanceEnReserve[i] = area.thermal.unsuppliedEnergyCost;

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

        problem.CaracteristiquesHydrauliques[i].SuiviNiveauHoraire
          = area.hydro.reservoirManagement && (problem.OptimisationAuPasHebdomadaire)
            && (!area.hydro.useHeuristicTarget
                || problem.CaracteristiquesHydrauliques[i].PresenceDePompageModulable);

        problem.CaracteristiquesHydrauliques[i].DirectLevelAccess = false;
        problem.CaracteristiquesHydrauliques[i].AccurateWaterValue = false;
        if (problem.WaterValueAccurate && area.hydro.useWaterValue)
        {
            problem.CaracteristiquesHydrauliques[i].AccurateWaterValue = true;
            problem.CaracteristiquesHydrauliques[i].SuiviNiveauHoraire = true;
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

    importShortTermStorages(study.areas, problem.ShortTermStorage);

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
        PtMat.NombreDElementsDansLaContrainteCouplante = bc->linkCount() + bc->clusterCount();
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
    for (uint i = 0; i < study.areas.size(); ++i)
    {
        const auto& area = *(study.areas.byIndex[i]);

        auto& pbPalier = problem.PaliersThermiquesDuPays[i];
        unsigned int clusterCount = area.thermal.list.enabledAndNotMustRunCount();
        pbPalier.NombreDePaliersThermiques = clusterCount;

        for (const auto& cluster: area.thermal.list.each_enabled_and_not_mustrun())
        {
            pbPalier.NumeroDuPalierDansLEnsembleDesPaliersThermiques[cluster->index] = NombrePaliers
                                                                                       + cluster
                                                                                           ->index;
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

        NombrePaliers += clusterCount;
    }

    problem.NombreDePaliersThermiques = NombrePaliers;

    problem.LeProblemeADejaEteInstancie = false;
}

static void prepareBindingConstraint(PROBLEME_HEBDO& problem,
                                     int PasDeTempsDebut,
                                     const BindingConstraintsRepository& bindingConstraints,
                                     const BindingConstraintGroupRepository& bcgroups,
                                     const uint weekFirstDay,
                                     int pasDeTemps)
{
    auto activeConstraints = bindingConstraints.activeConstraints();
    const auto constraintCount = activeConstraints.size();

    for (unsigned constraintIndex = 0; constraintIndex != constraintCount; ++constraintIndex)
    {
        auto bc = activeConstraints[constraintIndex];
        assert(bc->RHSTimeSeries().width && "Invalid constraint data width");

        uint tsIndexForBc = 0;
        auto* group = bcgroups[bc->group()];
        if (group)
        {
            tsIndexForBc = group->timeseriesNumbers[problem.year];
        }

        // If there is only one TS, always select it.
        const auto ts_number = bc->RHSTimeSeries().width == 1 ? 0 : tsIndexForBc;

        auto& timeSeries = bc->RHSTimeSeries();
        const double* column = timeSeries[ts_number];
        switch (bc->type())
        {
        case BindingConstraint::typeHourly:
        {
            problem.MatriceDesContraintesCouplantes[constraintIndex]
              .SecondMembreDeLaContrainteCouplante[pasDeTemps]
              = column[PasDeTempsDebut + pasDeTemps];
            break;
        }
        case BindingConstraint::typeDaily:
        {
            assert(timeSeries.width && "Invalid constraint data width");
            assert(weekFirstDay + 6 < timeSeries.height && "Invalid constraint data height");

            std::vector<double>& sndMember = problem
                                               .MatriceDesContraintesCouplantes[constraintIndex]
                                               .SecondMembreDeLaContrainteCouplante;

            for (unsigned day = 0; day != 7; ++day)
            {
                sndMember[day] = column[weekFirstDay + day];
            }

            break;
        }
        case BindingConstraint::typeWeekly:
        {
            assert(timeSeries.width && "Invalid constraint data width");
            assert(weekFirstDay + 6 < timeSeries.height && "Invalid constraint data height");

            double sum = 0;
            for (unsigned day = 0; day != 7; ++day)
            {
                sum += column[weekFirstDay + day];
            }

            problem.MatriceDesContraintesCouplantes[constraintIndex]
              .SecondMembreDeLaContrainteCouplante[0]
              = sum;
            break;
        }
        case BindingConstraint::typeUnknown:
        case BindingConstraint::typeMax:
        default:
        {
            assert(false && "invalid constraint type");
            logs.error() << "internal error. Please submit a full bug report";
            break;
        }
        }
    }
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

        if (lnk->useLoopFlow)
        {
            problem.CoutDeTransport[k].IntercoGereeAvecLoopFlow = true;
        }
        else
        {
            problem.CoutDeTransport[k].IntercoGereeAvecLoopFlow = false;
        }
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
            problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir
              = problem.previousSimulationFinalLevel[k];

            problem.CaracteristiquesHydrauliques[k].LevelForTimeInterval
              = problem.CaracteristiquesHydrauliques[k]
                  .NiveauInitialReservoir; /*for first 24-hour optim*/
            double nivInit = problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir;
            if (nivInit < 0.)
            {
                std::ostringstream msg;
                msg << "Area " << area.name << ", week " << weekInTheYear + 1
                    << " : initial level < 0";
                throw FatalError(msg.str());
            }

            if (nivInit > area.hydro.reservoirCapacity)
            {
                std::ostringstream msg;
                msg << "Area " << area.name << ", week " << weekInTheYear + 1
                    << " : initial level over capacity";
                throw FatalError(msg.str());
            }

            if (area.hydro.powerToLevel)
            {
                problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation = Antares::Data::
                  getWeeklyModulation(problem.previousSimulationFinalLevel[k] * 100
                                        / area.hydro.reservoirCapacity,
                                      area.hydro.creditModulation,
                                      Data::PartHydro::genMod);

                problem.CaracteristiquesHydrauliques[k].WeeklyPumpingModulation = Antares::Data::
                  getWeeklyModulation(problem.previousSimulationFinalLevel[k] * 100
                                        / area.hydro.reservoirCapacity,
                                      area.hydro.creditModulation,
                                      Data::PartHydro::pumpMod);
            }

            if (area.hydro.useWaterValue)
            {
                problem.CaracteristiquesHydrauliques[k].WeeklyWaterValueStateRegular
                  = getWaterValue(problem.previousSimulationFinalLevel[k] * 100
                                    / area.hydro.reservoirCapacity,
                                  area.hydro.waterValues,
                                  weekFirstDay);
            }

            if (problem.CaracteristiquesHydrauliques[k].PresenceDHydrauliqueModulable)
            {
                if (area.hydro.hardBoundsOnRuleCurves
                    && problem.CaracteristiquesHydrauliques[k].SuiviNiveauHoraire)
                {
                    auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
                    auto& maxLvl = area.hydro.reservoirLevel[Data::PartHydro::maximum];

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
                      = 0.5
                        * (area.hydro.waterValues[layerindex][weekFirstDay + 7]
                           + area.hydro.waterValues[layerindex + 1][weekFirstDay + 7]);
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

    int hourInYear = PasDeTempsDebut;
    for (unsigned hourInWeek = 0; hourInWeek < problem.NombreDePasDeTemps;
         ++hourInWeek, ++hourInYear)
    {
        prepareBindingConstraint(problem,
                                 PasDeTempsDebut,
                                 study.bindingConstraints,
                                 study.bindingConstraintsGroups,
                                 weekFirstDay,
                                 hourInWeek);

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
                                auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
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

                    if (problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir
                        < weekTarget_tmp)
                    {
                        marginGen = problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir;
                    }
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
                                    auto& maxLvl = area.hydro
                                                     .reservoirLevel[Data::PartHydro::maximum];

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
