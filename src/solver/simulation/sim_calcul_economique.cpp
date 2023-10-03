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

#include <sstream>

#include <antares/study/study.h>
#include <antares/study/area/constants.h>
#include <antares/study/area/scratchpad.h>

#include "antares/study/fwd.h"
#include "simulation.h"
#include "sim_structure_probleme_economique.h"
#include "sim_extern_variables_globales.h"
#include "adequacy_patch_runtime_data.h"
#include <antares/fatal-error.h>

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

static void importShortTermStorages(
  const AreaList& areas,
  std::vector<::ShortTermStorage::AREA_INPUT>& ShortTermStorageOut)
{
    int clusterGlobalIndex = 0;
    for (uint areaIndex = 0; areaIndex != areas.size(); areaIndex++)
    {
        ShortTermStorageOut[areaIndex].resize(areas[areaIndex]->shortTermStorage.count());
        int storageIndex = 0;
        for (auto st : areas[areaIndex]->shortTermStorage.storagesByIndex)
        {
            ::ShortTermStorage::PROPERTIES& toInsert = ShortTermStorageOut[areaIndex][storageIndex];
            toInsert.clusterGlobalIndex = clusterGlobalIndex;

            // Properties
            toInsert.reservoirCapacity = st->properties.reservoirCapacity.value();
            toInsert.efficiency = st->properties.efficiencyFactor;
            toInsert.injectionNominalCapacity = st->properties.injectionNominalCapacity.value();
            toInsert.withdrawalNominalCapacity = st->properties.withdrawalNominalCapacity.value();
            toInsert.initialLevel = st->properties.initialLevel;
            toInsert.name = st->properties.name;

            toInsert.series = st->series;

            // TODO add missing properties, or use the same struct
            storageIndex++;
            clusterGlobalIndex++;
        }
    }
}

void SIM_InitialisationProblemeHebdo(Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     int NombreDePasDeTemps,
                                     uint numSpace)
{
    int NombrePaliers;

    auto& parameters = study.parameters;

    problem.Expansion = parameters.expansion;
    problem.firstWeekOfSimulation = false;

    problem.hydroHotStart
      = (parameters.initialReservoirLevels.iniLevels == Antares::Data::irlHotStart);

    // gp adq : to be removed
    if (parameters.adqPatchParams.enabled)
    {
        problem.adequacyPatchRuntimeData
          = std::make_shared<AdequacyPatchRuntimeData>(study.areas, study.runtime->areaLink);
    }

    problem.WaterValueAccurate
      = (study.parameters.hydroPricing.hpMode == Antares::Data::HydroPricingMode::hpMILP);

    SIM_AllocationProblemeHebdo(study, problem, NombreDePasDeTemps);

    problem.NombreDePasDeTemps = NombreDePasDeTemps;

    problem.NombreDePasDeTempsDUneJournee = (int)(NombreDePasDeTemps / 7);

    problem.NombreDePays = study.areas.size();

    problem.NombreDInterconnexions = study.runtime->interconnectionsCount();

    problem.NumberOfShortTermStorages = study.runtime->shortTermStorageCount;

    auto activeContraints = study.bindingConstraints.activeContraints();
    problem.NombreDeContraintesCouplantes = activeContraints.size();

    problem.ExportMPS = study.parameters.include.exportMPS;
    problem.ExportStructure = study.parameters.include.exportStructure;
    problem.NamedProblems = study.parameters.namedProblems;
    problem.exportMPSOnError = Data::exportMPS(parameters.include.unfeasibleProblemBehavior);

    problem.OptimisationAvecCoutsDeDemarrage
      = (study.parameters.unitCommitment.ucMode == Antares::Data::UnitCommitmentMode::ucMILP);

    problem.OptimisationAuPasHebdomadaire
      = (parameters.simplexOptimizationRange == Data::sorWeek);

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

    for (uint i = 0; i != study.areas.size(); i++)
    {
        auto& area = *(study.areas[i]);

        problem.NomsDesPays[i] = area.id.c_str();

        problem.CoutDeDefaillancePositive[i] = area.thermal.unsuppliedEnergyCost;

        problem.CoutDeDefaillanceNegative[i] = area.thermal.spilledEnergyCost;

        problem.CoutDeDefaillanceEnReserve[i] = area.thermal.unsuppliedEnergyCost;

        problem.DefaillanceNegativeUtiliserPMinThermique[i]
          = (anoOtherDispatchPower & area.nodalOptimization) != 0;
        problem.DefaillanceNegativeUtiliserHydro[i]
          = (anoDispatchHydroPower & area.nodalOptimization) != 0;
        problem.DefaillanceNegativeUtiliserConsoAbattue[i]
          = (anoNonDispatchPower & area.nodalOptimization) != 0;

        problem.CaracteristiquesHydrauliques[i].PresenceDHydrauliqueModulable
          = area.scratchpad[numSpace].hydroHasMod;

        problem.CaracteristiquesHydrauliques[i].PresenceDePompageModulable
          = area.hydro.reservoirManagement && area.scratchpad[numSpace].pumpHasMod
              && area.hydro.pumpingEfficiency > 0.
              && problem.CaracteristiquesHydrauliques[i].PresenceDHydrauliqueModulable;

        problem.CaracteristiquesHydrauliques[i].PumpingRatio = area.hydro.pumpingEfficiency;

        problem.CaracteristiquesHydrauliques[i].SansHeuristique
          = area.hydro.reservoirManagement && !area.hydro.useHeuristicTarget;

        problem.CaracteristiquesHydrauliques[i].TurbinageEntreBornes
          = area.hydro.reservoirManagement
            && (!area.hydro.useHeuristicTarget || area.hydro.useLeeway);


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

        for (int pdt = 0; pdt < NombreDePasDeTemps; pdt++)
        {
            problem.CaracteristiquesHydrauliques[i].NiveauHoraireInf[pdt] = 0;
            problem.CaracteristiquesHydrauliques[i].NiveauHoraireSup[pdt]
              = problem.CaracteristiquesHydrauliques[i].TailleReservoir;
        }

        problem.previousSimulationFinalLevel[i] = -1.;

        if (!problem.previousYearFinalLevels.empty())
            problem.previousYearFinalLevels[i] = -1.;

        problem.CaracteristiquesHydrauliques[i].WeeklyWaterValueStateRegular = 0.;

        problem.CaracteristiquesHydrauliques[i].WeeklyGeneratingModulation = 1.;
        problem.CaracteristiquesHydrauliques[i].WeeklyPumpingModulation = 1.;

        assert(area.hydro.intraDailyModulation >= 1. && "Intra-daily modulation must be >= 1.0");
        problem.CoefficientEcretementPMaxHydraulique[i] = area.hydro.intraDailyModulation;
    }

    importShortTermStorages(study.areas, problem.ShortTermStorage);

    for (uint i = 0; i < study.runtime->interconnectionsCount(); ++i)
    {
        auto& link = *(study.runtime->areaLink[i]);
        problem.PaysOrigineDeLInterconnexion[i] = link.from->index;
        problem.PaysExtremiteDeLInterconnexion[i] = link.with->index;
    }

    for (uint i = 0; i < activeContraints.size(); ++i)
    {
        auto bc = activeContraints[i];
        CONTRAINTES_COUPLANTES& PtMat = problem.MatriceDesContraintesCouplantes[i];
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
        PtMat.SensDeLaContrainteCouplante = *Antares::Data::BindingConstraint::MathOperatorToCString(bc->operatorType());

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

            PtMat.OffsetTemporelSurLePalierDispatch[j] = bindingConstraintStructures.clusterOffset[j];
        }
    }

    NombrePaliers = 0;
    for (uint i = 0; i < study.areas.size(); ++i)
    {
        auto& area = *(study.areas.byIndex[i]);

        auto& pbPalier = problem.PaliersThermiquesDuPays[i];
        pbPalier.NombreDePaliersThermiques = area.thermal.list.size();

        for (uint clusterIndex = 0; clusterIndex != area.thermal.list.size(); ++clusterIndex)
        {
            auto& cluster = *(area.thermal.list.byIndex[clusterIndex]);
            pbPalier.NumeroDuPalierDansLEnsembleDesPaliersThermiques[clusterIndex]
              = NombrePaliers + clusterIndex;
            pbPalier.TailleUnitaireDUnGroupeDuPalierThermique[clusterIndex]
              = cluster.nominalCapacityWithSpinning;
            pbPalier.PminDuPalierThermiquePendantUneHeure[clusterIndex] = cluster.minStablePower;
            pbPalier.PminDuPalierThermiquePendantUnJour[clusterIndex] = 0;
            pbPalier.minUpDownTime[clusterIndex] = cluster.minUpDownTime;

            pbPalier.CoutDeDemarrageDUnGroupeDuPalierThermique[clusterIndex] = cluster.startupCost;
            pbPalier.CoutDArretDUnGroupeDuPalierThermique[clusterIndex] = 0;
            pbPalier.CoutFixeDeMarcheDUnGroupeDuPalierThermique[clusterIndex] = cluster.fixedCost;
            pbPalier.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[clusterIndex]
              = cluster.minUpTime;
            pbPalier.DureeMinimaleDArretDUnGroupeDuPalierThermique[clusterIndex]
              = cluster.minDownTime;

            pbPalier.PmaxDUnGroupeDuPalierThermique[clusterIndex]
              = cluster.nominalCapacityWithSpinning;
            pbPalier.pminDUnGroupeDuPalierThermique[clusterIndex]
              = (pbPalier.PmaxDUnGroupeDuPalierThermique[clusterIndex] < cluster.minStablePower)
                  ? pbPalier.PmaxDUnGroupeDuPalierThermique[clusterIndex]
                  : cluster.minStablePower;
            pbPalier.NomsDesPaliersThermiques[clusterIndex] = cluster.name().c_str();
        }

        NombrePaliers += area.thermal.list.size();
    }

    problem.NombreDePaliersThermiques = NombrePaliers;

    problem.LeProblemeADejaEteInstancie = false;
}

void preparerBindingConstraint(const PROBLEME_HEBDO &problem, uint numSpace, int PasDeTempsDebut,
                               const BindingConstraintsRepository &bindingConstraints, const uint weekFirstDay, int pasDeTemps) 
{
    auto activeContraints = bindingConstraints.activeContraints();
    const auto constraintCount = activeContraints.size();
    for (unsigned constraintIndex = 0; constraintIndex != constraintCount; ++constraintIndex)
    {
        auto bc = activeContraints[constraintIndex];
        assert(bc->RHSTimeSeries().width && "Invalid constraint data width");
        //If there is only one TS, always select it.
        const auto ts_number = bc->RHSTimeSeries().width == 1 ? 0 : NumeroChroniquesTireesParGroup[numSpace][bc->group()];
        auto& timeSeries = bc->RHSTimeSeries();
        double const* column = timeSeries[ts_number];
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

                std::vector<double>& sndMember
                    = problem.MatriceDesContraintesCouplantes[constraintIndex]
                        .SecondMembreDeLaContrainteCouplante;

                for (unsigned day = 0; day != 7; ++day)
                    sndMember[day] = column[weekFirstDay + day];

                break;
            }
            case BindingConstraint::typeWeekly:
            {
                assert(timeSeries.width && "Invalid constraint data width");
                assert(weekFirstDay + 6 < timeSeries.height && "Invalid constraint data height");

                double sum = 0;
                for (unsigned day = 0; day != 7; ++day)
                    sum += column[weekFirstDay + day];

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
                                    uint numSpace,
                                    const int PasDeTempsDebut,
                                    const ALL_HYDRO_VENTILATION_RESULTS& hydroVentilationResults)
{
    const auto& parameters = study.parameters;
    auto& studyruntime = *study.runtime;
    const uint nbPays = study.areas.size();
    const size_t pasDeTempsSizeDouble = problem.NombreDePasDeTemps * sizeof(double);

    const uint weekFirstDay = study.calendar.hours[PasDeTempsDebut].dayYear;

    for (int opt = 0; opt < 7; opt++)
    {
        problem.coutOptimalSolution1[opt] = 0.;
        problem.coutOptimalSolution2[opt] = 0.;
        problem.tempsResolution1[opt] = 0.;
        problem.tempsResolution2[opt] = 0.;
    }

    for (uint k = 0; k < studyruntime.interconnectionsCount(); ++k)
    {
        auto* lnk = studyruntime.areaLink[k];

        if (lnk->useHurdlesCost)
        {
            COUTS_DE_TRANSPORT& couts = problem.CoutDeTransport[k];
            couts.IntercoGereeAvecDesCouts = true;
            const double* direct
              = ((const double*)((void*)lnk->parameters[fhlHurdlesCostDirect])) + PasDeTempsDebut;
            const double* indirect
              = ((const double*)((void*)lnk->parameters[fhlHurdlesCostIndirect])) + PasDeTempsDebut;
            memcpy(&couts.CoutDeTransportOrigineVersExtremite[0], direct, pasDeTempsSizeDouble);
            memcpy(&couts.CoutDeTransportOrigineVersExtremiteRef[0], direct, pasDeTempsSizeDouble);
            memcpy(&couts.CoutDeTransportExtremiteVersOrigine[0], indirect, pasDeTempsSizeDouble);
            memcpy(&couts.CoutDeTransportExtremiteVersOrigineRef[0], indirect, pasDeTempsSizeDouble);
        }
        else
            problem.CoutDeTransport[k].IntercoGereeAvecDesCouts = false;

        if (lnk->useLoopFlow)
        {
            problem.CoutDeTransport[k].IntercoGereeAvecLoopFlow = true;
        }
        else
            problem.CoutDeTransport[k].IntercoGereeAvecLoopFlow = false;
    }

    int weekDayIndex[8];
    for (int day = 0; day < 8; day++)
        weekDayIndex[day] = study.calendar.hours[PasDeTempsDebut + day * 24].dayYear;

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
                problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation
                  = Antares::Data::getWeeklyModulation(
                    problem.previousSimulationFinalLevel[k] * 100 / area.hydro.reservoirCapacity,
                    area.hydro.creditModulation,
                    Data::PartHydro::genMod);

                problem.CaracteristiquesHydrauliques[k].WeeklyPumpingModulation
                  = Antares::Data::getWeeklyModulation(
                    problem.previousSimulationFinalLevel[k] * 100 / area.hydro.reservoirCapacity,
                    area.hydro.creditModulation,
                    Data::PartHydro::pumpMod);
            }

            if (area.hydro.useWaterValue)
            {
                Antares::Data::getWaterValue(
                  problem.previousSimulationFinalLevel[k] * 100 / area.hydro.reservoirCapacity,
                  area.hydro.waterValues,
                  weekFirstDay,
                  problem.CaracteristiquesHydrauliques[k].WeeklyWaterValueStateRegular);
            }

            if (problem.CaracteristiquesHydrauliques[k].PresenceDHydrauliqueModulable > 0)
            {
                if (area.hydro.hardBoundsOnRuleCurves
                    && problem.CaracteristiquesHydrauliques[k].SuiviNiveauHoraire)
                {
                    auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
                    auto& maxLvl = area.hydro.reservoirLevel[Data::PartHydro::maximum];

                    for (int day = 0; day < 7; day++)
                    {
                        levelInterpolBeg
                          = minLvl[weekDayIndex[day]]
                            * problem.CaracteristiquesHydrauliques[k].TailleReservoir;
                        levelInterpolEnd
                          = minLvl[weekDayIndex[day + 1]]
                            * problem.CaracteristiquesHydrauliques[k].TailleReservoir;
                        delta = (levelInterpolEnd - levelInterpolBeg) / 24.;

                        for (int hour = 0; hour < 24; hour++)
                            problem.CaracteristiquesHydrauliques[k]
                              .NiveauHoraireInf[24 * day + hour]
                              = levelInterpolBeg + hour * delta;

                        levelInterpolBeg
                          = maxLvl[weekDayIndex[day]]
                            * problem.CaracteristiquesHydrauliques[k].TailleReservoir;
                        levelInterpolEnd
                          = maxLvl[weekDayIndex[day + 1]]
                            * problem.CaracteristiquesHydrauliques[k].TailleReservoir;
                        delta = (levelInterpolEnd - levelInterpolBeg) / 24.;

                        for (int hour = 0; hour < 24; hour++)
                            problem.CaracteristiquesHydrauliques[k]
                              .NiveauHoraireSup[24 * day + hour]
                              = levelInterpolBeg + hour * delta;
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

    int hourInYear = PasDeTempsDebut;

    for (unsigned hourInWeek = 0; hourInWeek < problem.NombreDePasDeTemps; ++hourInWeek, ++hourInYear)
    {
        VALEURS_DE_NTC_ET_RESISTANCES& ntc = problem.ValeursDeNTC[hourInWeek];
        {
            uint linkCount = studyruntime.interconnectionsCount();
            for (uint k = 0; k != linkCount; ++k)
            {
                auto& lnk = *(studyruntime.areaLink[k]);
                const int tsIndex
                  = NumeroChroniquesTireesParInterconnexion[numSpace][k].TransmissionCapacities;

                assert((uint)hourInYear < lnk.directCapacities.height);
                assert((uint)tsIndex < lnk.directCapacities.width);
                assert((uint)tsIndex < lnk.indirectCapacities.width);

                ntc.ValeurDeNTCOrigineVersExtremite[k] = lnk.directCapacities[tsIndex][hourInYear];
                ntc.ValeurDeNTCExtremiteVersOrigine[k] = lnk.indirectCapacities[tsIndex][hourInYear];
                ntc.ValeurDeLoopFlowOrigineVersExtremite[k] = lnk.parameters[fhlLoopFlow][hourInYear];
            }
        }
        preparerBindingConstraint(
          problem, numSpace, PasDeTempsDebut, study.bindingConstraints, weekFirstDay, hourInWeek);

        for (uint k = 0; k < nbPays; ++k)
        {
            auto& tsIndex = NumeroChroniquesTireesParPays[numSpace][k];
            uint tsIndexMaxPower = tsIndex.HydroMaxPower;
            auto& area = *(study.areas.byIndex[k]);
            auto& scratchpad = area.scratchpad[numSpace];
            auto& ror = area.hydro.series->ror;

            auto& maxHourlyGenPowerMatrix = area.hydro.series->maxHourlyGenPower;
            auto const& maxHourlyGenPowerTS
              = maxHourlyGenPowerMatrix[tsIndexMaxPower];
            auto const& ContrainteDePmaxHydrauliqueHoraire = maxHourlyGenPowerTS[PasDeTempsDebut + hourInWeek];

            auto& maxHourlyPumpPowerMatrix = area.hydro.series->maxHourlyPumpPower;
            auto const& maxHourlyPumpPowerTS
              = maxHourlyPumpPowerMatrix[tsIndexMaxPower];
            auto const& ContrainteDePmaxPompageHoraire = maxHourlyPumpPowerTS[PasDeTempsDebut + hourInWeek];

            assert(&scratchpad);
            assert((uint)hourInYear < scratchpad.ts.load.height);
            assert((uint)tsIndex.Consommation < scratchpad.ts.load.width);
            if (parameters.renewableGeneration.isAggregated())
            {
                assert((uint)hourInYear < scratchpad.ts.solar.height);
                assert((uint)hourInYear < scratchpad.ts.wind.height);
                assert((uint)tsIndex.Eolien < scratchpad.ts.wind.width);
                assert((uint)tsIndex.Solar < scratchpad.ts.solar.width);
            }

            uint tsFatalIndex = (uint)tsIndex.Hydraulique < ror.width ? tsIndex.Hydraulique : 0;
            double& mustRunGen = problem.AllMustRunGeneration[hourInWeek].AllMustRunGenerationOfArea[k];
            if (parameters.renewableGeneration.isAggregated())
            {
                mustRunGen = scratchpad.ts.wind[tsIndex.Eolien][hourInYear]
                             + scratchpad.ts.solar[tsIndex.Solar][hourInYear]
                             + scratchpad.miscGenSum[hourInYear] + ror[tsFatalIndex][hourInYear]
                             + scratchpad.mustrunSum[hourInYear];
            }

            // Renewable
            if (parameters.renewableGeneration.isClusters())
            {
                mustRunGen = scratchpad.miscGenSum[hourInYear] + ror[tsFatalIndex][hourInYear]
                             + scratchpad.mustrunSum[hourInYear];

                area.renewable.list.each([&](const RenewableCluster& cluster) {
                    assert(cluster.series->timeSeries.jit == NULL && "No JIT data from the solver");
                    mustRunGen += cluster.valueAtTimeStep(
                      tsIndex.RenouvelableParPalier[cluster.areaWideIndex], (uint)hourInYear);
                });
            }

            assert(
              !Math::NaN(problem.AllMustRunGeneration[hourInWeek].AllMustRunGenerationOfArea[k])
              && "NaN detected for 'AllMustRunGeneration', probably from miscGenSum/mustrunSum");

            problem.ConsommationsAbattues[hourInWeek].ConsommationAbattueDuPays[k]
              = +scratchpad.ts.load[tsIndex.Consommation][hourInYear]
                - problem.AllMustRunGeneration[hourInWeek].AllMustRunGenerationOfArea[k];

            if (problem.CaracteristiquesHydrauliques[k].PresenceDHydrauliqueModulable > 0)
            {
                problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxHydrauliqueHoraire[hourInWeek]
                  = ContrainteDePmaxHydrauliqueHoraire
                    * problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation;
            }

            if (problem.CaracteristiquesHydrauliques[k].PresenceDePompageModulable)
            {
                problem.CaracteristiquesHydrauliques[k].ContrainteDePmaxPompageHoraire[hourInWeek]
                  = ContrainteDePmaxPompageHoraire
                    * problem.CaracteristiquesHydrauliques[k].WeeklyPumpingModulation;
            }

            problem.ReserveJMoins1[k].ReserveHoraireJMoins1[hourInWeek]
              = area.reserves[fhrDayBefore][PasDeTempsDebut + hourInWeek];
        }
    }

    {
        for (uint k = 0; k < nbPays; ++k)
        {
            if (problem.CaracteristiquesHydrauliques[k].PresenceDHydrauliqueModulable > 0)
            {
                auto& area = *study.areas.byIndex[k];
                uint tsIndex = (NumeroChroniquesTireesParPays[numSpace][k]).Hydraulique;
                auto& inflowsmatrix = area.hydro.series->storage;
                auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];
                uint tsIndexMaxPower = (NumeroChroniquesTireesParPays[numSpace][k]).HydroMaxPower;
                auto& scratchpad = area.scratchpad[numSpace];
                auto& dailyMeanMaxGenPower = scratchpad.meanMaxDailyGenPower[tsIndexMaxPower];
                auto& dailyMeanMaxPumpPower = scratchpad.meanMaxDailyPumpPower[tsIndexMaxPower];
                auto& mingenmatrix = area.hydro.series->mingen;
                auto const& srcmingen = mingenmatrix[tsIndex < mingenmatrix.width ? tsIndex : 0];
                for (uint j = 0; j < problem.NombreDePasDeTemps; ++j)
                {
                    problem.CaracteristiquesHydrauliques[k].MingenHoraire[j]
                        = srcmingen[PasDeTempsDebut + j];
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
                              = dailyMeanMaxGenPower[day] * area.hydro.maxDailyGenEnergy[0][day]
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

                        const std::vector<double>& DNT
                          = hydroVentilationResults[numSpace][k].HydrauliqueModulableQuotidien;

                        double WSL
                          = problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir;

                        double LUB = area.hydro.leewayUpperBound;
                        if (!area.hydro.useLeeway)
                            LUB = 1;
                        double LLB = area.hydro.leewayLowerBound;
                        if (!area.hydro.useLeeway)
                            LLB = 1;
                        double DGM
                          = problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation;

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

                            double DGC
                              = dailyMeanMaxGenPower[day] * area.hydro.maxDailyGenEnergy[0][day];

                            DGU_tmp[j] = DNT[day] * LUB;
                            DGL_tmp[j] = DNT[day] * LLB;
                            double DGCxDGM = DGC * DGM;

                            if (DGCxDGM < DGL_tmp[j])
                            {
                                DGU_tmp[j] = DGCxDGM;
                                DGL_tmp[j] = DGCxDGM;
                            }

                            if (DGCxDGM > DGL_tmp[j] && DGCxDGM < DGU_tmp[j])
                                DGU_tmp[j] = DGCxDGM;

                            WGU += DGU_tmp[j];
                        }

                        for (uint j = 0; j < 7; ++j)
                        {
                            if (not area.hydro.hardBoundsOnRuleCurves)
                            {
                                if (Math::Zero(WGU))
                                    DGU[j] = 0.;
                                else
                                    DGU[j] = DGU_tmp[j] * Math::Min(WGU, WSL + WNI) / WGU;
                            }

                            else
                            {
                                const uint nextWeekFirstDay
                                  = study.calendar.hours[PasDeTempsDebut + 7 * 24].dayYear;
                                auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
                                double V = Math::Max(0., WSL - minLvl[nextWeekFirstDay] * rc + WNI);

                                if (Math::Zero(WGU))
                                    DGU[j] = 0.;
                                else
                                    DGU[j] = DGU_tmp[j] * Math::Min(WGU, V) / WGU;
                            }

                            DGL[j] = Math::Min(DGU[j], DGL_tmp[j]);
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
                        weekTarget_tmp += hydroVentilationResults[numSpace][k]
                                            .HydrauliqueModulableQuotidien[day];
                    }

                    if (weekTarget_tmp != 0.)
                        weekGenerationTarget = weekTarget_tmp;

                    marginGen = weekGenerationTarget;

                    if (problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir
                        < weekTarget_tmp)
                        marginGen = problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir;
                }

                if (not problem.CaracteristiquesHydrauliques[k].TurbinageEntreBornes)
                {
                    for (uint j = 0; j < 7; ++j)
                    {
                        uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                        problem.CaracteristiquesHydrauliques[k]
                          .CntEnergieH2OParIntervalleOptimise[j]
                          = hydroVentilationResults[numSpace][k].HydrauliqueModulableQuotidien[day]
                            * problem.CaracteristiquesHydrauliques[k].WeeklyGeneratingModulation
                            * marginGen / weekGenerationTarget;
                    }
                }

                for (uint j = 0; j < 7; ++j)
                {
                    uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                    problem.CaracteristiquesHydrauliques[k].InflowForTimeInterval[j]
                      = srcinflows[day];
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
                                    * area.hydro.maxDailyPumpEnergy[0][day]
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

                            double WSL
                              = problem.CaracteristiquesHydrauliques[k].NiveauInitialReservoir;

                            double DPM
                              = problem.CaracteristiquesHydrauliques[k].WeeklyPumpingModulation;

                            double pumping_ratio = area.hydro.pumpingEfficiency;

                            double WPU = 0.;

                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                                double DPC = dailyMeanMaxPumpPower[day]
                                             * area.hydro.maxDailyPumpEnergy[0][day];

                                WPU += DPC;
                            }

                            double U = WPU * DPM * pumping_ratio;

                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                                double DPC = dailyMeanMaxPumpPower[day]
                                             * area.hydro.maxDailyPumpEnergy[0][day];
                                double rc = area.hydro.reservoirCapacity;

                                if (not area.hydro.hardBoundsOnRuleCurves)
                                {
                                    double V = Math::Max(0., rc - (WNI + WSL));

                                    if (Math::Zero(U))
                                        DPU[j] = 0.;
                                    else
                                        DPU[j] = DPC * DPM * Math::Min(U, V) / U;
                                }

                                else
                                {
                                    const uint nextWeekFirstDay
                                      = study.calendar.hours[PasDeTempsDebut + 7 * 24].dayYear;
                                    auto& maxLvl
                                      = area.hydro.reservoirLevel[Data::PartHydro::maximum];

                                    double V
                                      = Math::Max(0., maxLvl[nextWeekFirstDay] * rc - (WNI + WSL));

                                    if (Math::Zero(U))
                                        DPU[j] = 0.;
                                    else
                                        DPU[j] = DPC * DPM * Math::Min(U, V) / U;
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
