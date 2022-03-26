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
#include <yuni/core/math.h>
#include <antares/study.h>
#include <antares/study/area/constants.h>
#include <antares/study/area/scratchpad.h>
#include <antares/study/parts/hydro/container.h>

#include "simulation.h"

#include "sim_structure_donnees.h"
#include "sim_structure_probleme_economique.h"
#include "sim_structure_probleme_adequation.h"
#include "sim_extern_variables_globales.h"
#include "../optimisation/opt_fonctions.h"
#include "../optimisation/opt_structure_probleme_a_resoudre.h"

#include <antares/emergency.h>

using namespace Antares;
using namespace Antares::Data;
using namespace Yuni;

void SIM_InitialisationProblemeHebdo(Data::Study& study,
                                     PROBLEME_HEBDO& problem,
                                     int NombreDePasDeTemps,
                                     uint numSpace)
{
    int NombrePaliers;
    CONTRAINTES_COUPLANTES* PtMat;

    auto& parameters = study.parameters;

    problem.Expansion = parameters.expansion ? OUI_ANTARES : NON_ANTARES;
    problem.firstWeekOfSimulation = false;

    problem.hydroHotStart
      = (parameters.initialReservoirLevels.iniLevels == Antares::Data::irlHotStart);

    if (parameters.include.adequacyPatch)
    {
        problem.adqPatch = std::unique_ptr<AdequacyPatchParameters>(new AdequacyPatchParameters());
        // AdequacyFirstStep will be initialized during the economy solve
        // AdqBehaviorMap will be initialized during the economy solve
        problem.adqPatch->LinkCapacityForAdqPatchFirstStepFromAreaOutsideToAreaInsideAdq
          = parameters.setToZero12LinksForAdequacyPatch;
        problem.adqPatch->LinkCapacityForAdqPatchFirstStepBetweenAreaOutsideAdq
          = parameters.setToZero11LinksForAdequacyPatch;
    }

    if (parameters.include.adequacyPatch){
      problem.adequacyPatchRuntimeData.initialize(study);
    }

    problem.WaterValueAccurate
      = (study.parameters.hydroPricing.hpMode == Antares::Data::HydroPricingMode::hpMILP)
          ? OUI_ANTARES
          : NON_ANTARES;

    SIM_AllocationProblemeHebdo(problem, NombreDePasDeTemps);

    problem.NombreDePasDeTemps = NombreDePasDeTemps;

    problem.NombreDePasDeTempsDUneJournee = (int)(NombreDePasDeTemps / 7);

    problem.NombreDePays = study.areas.size();

    problem.NombreDInterconnexions = study.runtime->interconnectionsCount;

    problem.NombreDeContraintesCouplantes = study.runtime->bindingConstraintCount;

    problem.ExportMPS = study.parameters.include.exportMPS;
    problem.ExportStructure = study.parameters.include.exportStructure;
    problem.exportMPSOnError = Data::exportMPS(parameters.include.unfeasibleProblemBehavior);

    problem.OptimisationAvecCoutsDeDemarrage
      = (study.parameters.unitCommitment.ucMode == Antares::Data::UnitCommitmentMode::ucMILP)
          ? OUI_ANTARES
          : NON_ANTARES;

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
          = ((anoOtherDispatchPower & area.nodalOptimization) != 0) ? OUI_ANTARES : NON_ANTARES;
        problem.DefaillanceNegativeUtiliserHydro[i]
          = ((anoDispatchHydroPower & area.nodalOptimization) != 0) ? OUI_ANTARES : NON_ANTARES;
        problem.DefaillanceNegativeUtiliserConsoAbattue[i]
          = ((anoNonDispatchPower & area.nodalOptimization) != 0) ? OUI_ANTARES : NON_ANTARES;

        problem.CaracteristiquesHydrauliques[i]->PresenceDHydrauliqueModulable
          = (area.scratchpad[numSpace]->hydroHasMod ? OUI_ANTARES : NON_ANTARES);

        problem.CaracteristiquesHydrauliques[i]->PresenceDePompageModulable
          = ((area.hydro.reservoirManagement && area.scratchpad[numSpace]->pumpHasMod
              && area.hydro.pumpingEfficiency > 0.
              && problem.CaracteristiquesHydrauliques[i]->PresenceDHydrauliqueModulable)
               ? OUI_ANTARES
               : NON_ANTARES);

        problem.CaracteristiquesHydrauliques[i]->PumpingRatio = area.hydro.pumpingEfficiency;

        problem.CaracteristiquesHydrauliques[i]->TurbinageEntreBornes
          = ((area.hydro.reservoirManagement
              && (!area.hydro.useHeuristicTarget || area.hydro.useLeeway))
               ? OUI_ANTARES
               : NON_ANTARES);

        problem.CaracteristiquesHydrauliques[i]->SuiviNiveauHoraire
          = ((area.hydro.reservoirManagement && !area.hydro.useHeuristicTarget) ? OUI_ANTARES
                                                                                : NON_ANTARES);

        problem.CaracteristiquesHydrauliques[i]->AccurateWaterValue = NON_ANTARES;
        if (problem.WaterValueAccurate == OUI_ANTARES
            && problem.CaracteristiquesHydrauliques[i]->TurbinageEntreBornes == OUI_ANTARES)
            problem.CaracteristiquesHydrauliques[i]->AccurateWaterValue = OUI_ANTARES;

        problem.CaracteristiquesHydrauliques[i]->DirectLevelAccess = NON_ANTARES;
        if (problem.WaterValueAccurate == OUI_ANTARES
            && problem.CaracteristiquesHydrauliques[i]->SuiviNiveauHoraire == OUI_ANTARES)
            problem.CaracteristiquesHydrauliques[i]->DirectLevelAccess = OUI_ANTARES;

        problem.CaracteristiquesHydrauliques[i]->TailleReservoir = area.hydro.reservoirCapacity;

        for (int pdt = 0; pdt < NombreDePasDeTemps; pdt++)
        {
            problem.CaracteristiquesHydrauliques[i]->NiveauHoraireInf[pdt] = 0;
            problem.CaracteristiquesHydrauliques[i]->NiveauHoraireSup[pdt]
              = problem.CaracteristiquesHydrauliques[i]->TailleReservoir;
        }

        problem.previousSimulationFinalLevel[i] = -1.;

        if (problem.previousYearFinalLevels)
            problem.previousYearFinalLevels[i] = -1.;

        problem.CaracteristiquesHydrauliques[i]->WeeklyWaterValueStateRegular = 0.;

        problem.CaracteristiquesHydrauliques[i]->WeeklyGeneratingModulation = 1.;
        problem.CaracteristiquesHydrauliques[i]->WeeklyPumpingModulation = 1.;

        assert(area.hydro.intraDailyModulation >= 1. && "Intra-daily modulation must be >= 1.0");
        problem.CoefficientEcretementPMaxHydraulique[i] = area.hydro.intraDailyModulation;
    }

    for (uint i = 0; i < study.runtime->interconnectionsCount; ++i)
    {
        auto& link = *(study.runtime->areaLink[i]);
        problem.PaysOrigineDeLInterconnexion[i] = link.from->index;
        problem.PaysExtremiteDeLInterconnexion[i] = link.with->index;
    }

    for (uint i = 0; i < study.runtime->bindingConstraintCount; ++i)
    {
        BindingConstraintRTI& bc = study.runtime->bindingConstraint[i];

        PtMat = problem.MatriceDesContraintesCouplantes[i];
        PtMat->NombreDInterconnexionsDansLaContrainteCouplante = bc.linkCount;
        PtMat->NombreDePaliersDispatchDansLaContrainteCouplante = bc.clusterCount;
        PtMat->NombreDElementsDansLaContrainteCouplante = bc.linkCount + bc.clusterCount;
        PtMat->NomDeLaContrainteCouplante = bc.name.c_str();
        switch (bc.type)
        {
        case BindingConstraint::typeHourly:
            PtMat->TypeDeContrainteCouplante = CONTRAINTE_HORAIRE;
            break;
        case BindingConstraint::typeDaily:
            PtMat->TypeDeContrainteCouplante = CONTRAINTE_JOURNALIERE;
            break;
        case BindingConstraint::typeWeekly:
            PtMat->TypeDeContrainteCouplante = CONTRAINTE_HEBDOMADAIRE;
            break;
        case BindingConstraint::typeUnknown:
        case BindingConstraint::typeMax:
            assert(false && "Invalid constraint");
            break;
        }
        PtMat->SensDeLaContrainteCouplante = bc.operatorType;

        for (uint j = 0; j < bc.linkCount; ++j)
        {
            PtMat->NumeroDeLInterconnexion[j] = bc.linkIndex[j];
            PtMat->PoidsDeLInterconnexion[j] = bc.linkWeight[j];

            PtMat->OffsetTemporelSurLInterco[j] = bc.linkOffset[j];
        }

        for (uint j = 0; j < bc.clusterCount; ++j)
        {
            PtMat->NumeroDuPalierDispatch[j] = bc.clusterIndex[j];
            PtMat->PaysDuPalierDispatch[j] = bc.clustersAreaIndex[j];
            PtMat->PoidsDuPalierDispatch[j] = bc.clusterWeight[j];

            PtMat->OffsetTemporelSurLePalierDispatch[j] = bc.clusterOffset[j];
        }
    }

    NombrePaliers = 0;
    for (uint i = 0; i < study.areas.size(); ++i)
    {
        auto& area = *(study.areas.byIndex[i]);

        auto& pbPalier = *(problem.PaliersThermiquesDuPays[i]);
        pbPalier.NombreDePaliersThermiques = area.thermal.list.size();

        for (uint l = 0; l != area.thermal.list.size(); ++l)
        {
            auto& cluster = *(area.thermal.list.byIndex[l]);
            pbPalier.NumeroDuPalierDansLEnsembleDesPaliersThermiques[l] = NombrePaliers + l;
            pbPalier.TailleUnitaireDUnGroupeDuPalierThermique[l]
              = cluster.nominalCapacityWithSpinning;
            pbPalier.PminDuPalierThermiquePendantUneHeure[l] = cluster.minStablePower;
            pbPalier.PminDuPalierThermiquePendantUnJour[l] = 0;
            pbPalier.PminDuPalierThermiquePendantUneSemaine[l] = 0;
            pbPalier.minUpDownTime[l] = cluster.minUpDownTime;

            pbPalier.CoutDeDemarrageDUnGroupeDuPalierThermique[l] = cluster.startupCost;
            pbPalier.CoutDArretDUnGroupeDuPalierThermique[l] = 0;
            pbPalier.CoutFixeDeMarcheDUnGroupeDuPalierThermique[l] = cluster.fixedCost;
            pbPalier.DureeMinimaleDeMarcheDUnGroupeDuPalierThermique[l] = cluster.minUpTime;
            pbPalier.DureeMinimaleDArretDUnGroupeDuPalierThermique[l] = cluster.minDownTime;

            pbPalier.PmaxDUnGroupeDuPalierThermique[l] = cluster.nominalCapacityWithSpinning;
            pbPalier.PminDUnGroupeDuPalierThermique[l]
              = (pbPalier.PmaxDUnGroupeDuPalierThermique[l] < cluster.minStablePower)
                  ? pbPalier.PmaxDUnGroupeDuPalierThermique[l]
                  : cluster.minStablePower;
        }

        NombrePaliers += area.thermal.list.size();
    }

    problem.NombreDePaliersThermiques = NombrePaliers;

    problem.LeProblemeADejaEteInstancie = NON_ANTARES;

    problem.OptimisationAuPasHebdomadaire
      = (parameters.simplexOptimizationRange == Data::sorWeek) ? OUI_ANTARES : NON_ANTARES;
}

void SIM_InitialisationResultats()
{
    auto& study = *Data::Study::Current::Get();
    const size_t sizeOfNbHoursDouble = study.runtime->nbHoursPerYear * sizeof(double);
    const size_t sizeOfNbHoursLong = study.runtime->nbHoursPerYear * sizeof(int);

    for (uint i = 0; i < study.runtime->interconnectionsCount; ++i)
    {
        auto& interconnexion = *ResultatsParInterconnexion[i];
        memset(interconnexion.VariablesDualesMoyennes, 0, sizeOfNbHoursDouble);
        memset(interconnexion.TransitMoyen, 0, sizeOfNbHoursDouble);
        memset(interconnexion.TransitStdDev, 0, sizeOfNbHoursDouble);
        memset(interconnexion.TransitMinimumNo, 0, sizeOfNbHoursLong);
        memset(interconnexion.TransitMaximumNo, 0, sizeOfNbHoursLong);
        memset(interconnexion.RenteHoraire, 0, sizeOfNbHoursDouble);
        memset(interconnexion.TransitAnnuel, 0, study.runtime->nbYears * sizeof(double));
        for (uint j = 0; j < study.runtime->nbHoursPerYear; j++)
        {
            interconnexion.TransitMinimum[j] = (double)LINFINI_ENTIER;
            interconnexion.TransitMaximum[j] = (double)-LINFINI_ENTIER;
        }
    }

    for (uint i = 0; i < study.runtime->bindingConstraintCount; i++)
    {
        memset(ResultatsParContrainteCouplante[i]->VariablesDualesMoyennes, 0, sizeOfNbHoursDouble);
    }
}

void SIM_RenseignementProblemeHebdo(PROBLEME_HEBDO& problem,
                                    Antares::Solver::Variable::State& state,
                                    uint numSpace,
                                    const int PasDeTempsDebut)
{
    auto& study = *Data::Study::Current::Get();
    const auto& parameters = study.parameters;
    auto& studyruntime = *study.runtime;
    const uint nbPays = study.areas.size();
    const size_t pasDeTempsSizeDouble = problem.NombreDePasDeTemps * sizeof(double);
    const size_t sizeOfIntercoDouble = sizeof(double) * studyruntime.interconnectionsCount;

    const uint weekFirstDay = study.calendar.hours[PasDeTempsDebut].dayYear;

    int indx = PasDeTempsDebut;

    for (int opt = 0; opt < 7; opt++)
    {
        problem.numeroOptimisation[opt] = 0;
        problem.coutOptimalSolution1[opt] = 0.;
        problem.coutOptimalSolution2[opt] = 0.;
    }

    for (uint k = 0; k < studyruntime.interconnectionsCount; ++k)
    {
        auto* lnk = studyruntime.areaLink[k];

        if (lnk->useHurdlesCost)
        {
            COUTS_DE_TRANSPORT* couts = problem.CoutDeTransport[k];
            couts->IntercoGereeAvecDesCouts = OUI_ANTARES;
            const double* direct
              = ((const double*)((void*)lnk->parameters[fhlHurdlesCostDirect])) + PasDeTempsDebut;
            const double* indirect
              = ((const double*)((void*)lnk->parameters[fhlHurdlesCostIndirect])) + PasDeTempsDebut;
            memcpy(couts->CoutDeTransportOrigineVersExtremite, direct, pasDeTempsSizeDouble);
            memcpy(couts->CoutDeTransportOrigineVersExtremiteRef, direct, pasDeTempsSizeDouble);
            memcpy(couts->CoutDeTransportExtremiteVersOrigine, indirect, pasDeTempsSizeDouble);
            memcpy(couts->CoutDeTransportExtremiteVersOrigineRef, indirect, pasDeTempsSizeDouble);
        }
        else
            problem.CoutDeTransport[k]->IntercoGereeAvecDesCouts = NON_ANTARES;

        if (lnk->useLoopFlow)
        {
            problem.CoutDeTransport[k]->IntercoGereeAvecLoopFlow = OUI_ANTARES;
        }
        else
            problem.CoutDeTransport[k]->IntercoGereeAvecLoopFlow = NON_ANTARES;

        lnk->flush();
    }

    if (studyruntime.bindingConstraintCount)
    {
        for (uint k = 0; k != studyruntime.bindingConstraintCount; ++k)
        {
            auto& bc = studyruntime.bindingConstraint[k];
            switch (bc.type)
            {
            case BindingConstraint::typeHourly:
            {
                break;
            }
            case BindingConstraint::typeDaily:
            {
                assert(bc.bounds.width && "Invalid constraint data width");
                assert(weekFirstDay + 6 < bc.bounds.height && "Invalid constraint data height");
                auto& column = bc.bounds[0];
                double* sndMember
                  = problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante;
                double* sndMemberRef = problem.MatriceDesContraintesCouplantes[k]
                                         ->SecondMembreDeLaContrainteCouplanteRef;
                for (uint d = 0; d != 7; ++d)
                {
                    sndMember[d] = column[weekFirstDay + d];
                    sndMemberRef[d] = sndMember[d];
                }
                break;
            }
            case BindingConstraint::typeWeekly:
            {
                assert(bc.bounds.width && "Invalid constraint data width");
                assert(weekFirstDay + 6 < bc.bounds.height && "Invalid constraint data height");
                const Matrix<>::ColumnType& column = bc.bounds[0];
                double sum = 0;
                for (uint d = 0; d != 7; ++d)
                    sum += column[weekFirstDay + d];

                problem.MatriceDesContraintesCouplantes[k]->SecondMembreDeLaContrainteCouplante[0]
                  = sum;
                problem.MatriceDesContraintesCouplantes[k]
                  ->SecondMembreDeLaContrainteCouplanteRef[0]
                  = sum;
                break;
            }
            case BindingConstraint::typeUnknown:
            case BindingConstraint::typeMax:
            {
                assert(false && "invalid constraint type");
                logs.error() << "internal error. Please submit a full bug report";
                break;
            }
            }
            bc.bounds.flush();
        }
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
            problem.CaracteristiquesHydrauliques[k]->NiveauInitialReservoir
              = problem.previousSimulationFinalLevel[k];

            problem.CaracteristiquesHydrauliques[k]->LevelForTimeInterval
              = problem.CaracteristiquesHydrauliques[k]
                  ->NiveauInitialReservoir; /*for first 24-hour optim*/
            double nivInit = problem.CaracteristiquesHydrauliques[k]->NiveauInitialReservoir;
            if (nivInit < 0.)
            {
                logs.fatal() << "Area " << area.name << ", week " << state.weekInTheYear + 1
                             << " : initial level < 0";
                AntaresSolverEmergencyShutdown();
            }

            if (nivInit > area.hydro.reservoirCapacity)
            {
                logs.fatal() << "Area " << area.name << ", week " << state.weekInTheYear + 1
                             << " : initial level over capacity";
                AntaresSolverEmergencyShutdown();
            }

            if (area.hydro.powerToLevel)
            {
                problem.CaracteristiquesHydrauliques[k]->WeeklyGeneratingModulation
                  = Antares::Data::getWeeklyModulation(
                    problem.previousSimulationFinalLevel[k] * 100 / area.hydro.reservoirCapacity,
                    area.hydro.creditModulation,
                    Data::PartHydro::genMod);

                problem.CaracteristiquesHydrauliques[k]->WeeklyPumpingModulation
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
                  state.h2oValueWorkVars,
                  problem.CaracteristiquesHydrauliques[k]->WeeklyWaterValueStateRegular);
            }

            if (problem.CaracteristiquesHydrauliques[k]->PresenceDHydrauliqueModulable > 0)
            {
                if (area.hydro.hardBoundsOnRuleCurves
                    && problem.CaracteristiquesHydrauliques[k]->SuiviNiveauHoraire == OUI_ANTARES)
                {
                    auto& minLvl = area.hydro.reservoirLevel[Data::PartHydro::minimum];
                    auto& maxLvl = area.hydro.reservoirLevel[Data::PartHydro::maximum];

                    for (int day = 0; day < 7; day++)
                    {
                        levelInterpolBeg
                          = minLvl[weekDayIndex[day]]
                            * problem.CaracteristiquesHydrauliques[k]->TailleReservoir;
                        levelInterpolEnd
                          = minLvl[weekDayIndex[day + 1]]
                            * problem.CaracteristiquesHydrauliques[k]->TailleReservoir;
                        delta = (levelInterpolEnd - levelInterpolBeg) / 24.;

                        for (int hour = 0; hour < 24; hour++)
                            problem.CaracteristiquesHydrauliques[k]
                              ->NiveauHoraireInf[24 * day + hour]
                              = levelInterpolBeg + hour * delta;

                        levelInterpolBeg
                          = maxLvl[weekDayIndex[day]]
                            * problem.CaracteristiquesHydrauliques[k]->TailleReservoir;
                        levelInterpolEnd
                          = maxLvl[weekDayIndex[day + 1]]
                            * problem.CaracteristiquesHydrauliques[k]->TailleReservoir;
                        delta = (levelInterpolEnd - levelInterpolBeg) / 24.;

                        for (int hour = 0; hour < 24; hour++)
                            problem.CaracteristiquesHydrauliques[k]
                              ->NiveauHoraireSup[24 * day + hour]
                              = levelInterpolBeg + hour * delta;
                    }
                }
            }
            if (problem.CaracteristiquesHydrauliques[k]->AccurateWaterValue == OUI_ANTARES)
            {
                for (uint layerindex = 0; layerindex < 100; layerindex++)
                {
                    problem.CaracteristiquesHydrauliques[k]->WaterLayerValues[layerindex]
                      = 0.5
                        * (area.hydro.waterValues[layerindex][weekFirstDay + 7]
                           + area.hydro.waterValues[layerindex + 1][weekFirstDay + 7]);
                }
            }
        }
    }

    for (int j = 0; j < problem.NombreDePasDeTemps; ++j, ++indx)
    {
        VALEURS_DE_NTC_ET_RESISTANCES* ntc = problem.ValeursDeNTC[j];
        assert(NULL != ntc);

        {
            uint linkCount = studyruntime.interconnectionsCount;
            for (uint k = 0; k != linkCount; ++k)
            {
                auto& lnk = *(studyruntime.areaLink[k]);
                const int tsIndex
                  = NumeroChroniquesTireesParInterconnexion[numSpace][k].TransmissionCapacities;

                assert((uint)indx < lnk.directCapacities.height);
                assert((uint)tsIndex < lnk.directCapacities.width);
                assert((uint)tsIndex < lnk.indirectCapacities.width);

                ntc->ValeurDeNTCOrigineVersExtremite[k] = lnk.directCapacities[tsIndex][indx];
                ntc->ValeurDeNTCExtremiteVersOrigine[k] = lnk.indirectCapacities[tsIndex][indx];
                ntc->ValeurDeLoopFlowOrigineVersExtremite[k] = lnk.parameters[fhlLoopFlow][indx];
            }
        }

        memcpy((char*)problem.ValeursDeNTCRef[j]->ValeurDeNTCOrigineVersExtremite,
               (char*)ntc->ValeurDeNTCOrigineVersExtremite,
               sizeOfIntercoDouble);
        memcpy((char*)problem.ValeursDeNTCRef[j]->ValeurDeNTCExtremiteVersOrigine,
               (char*)ntc->ValeurDeNTCExtremiteVersOrigine,
               sizeOfIntercoDouble);
        memcpy((char*)problem.ValeursDeNTCRef[j]->ValeurDeLoopFlowOrigineVersExtremite,
               (char*)ntc->ValeurDeLoopFlowOrigineVersExtremite,
               sizeOfIntercoDouble);

        {
            const uint constraintCount = studyruntime.bindingConstraintCount;
            for (uint k = 0; k != constraintCount; ++k)
            {
                auto& bc = studyruntime.bindingConstraint[k];

                if (bc.type == BindingConstraint::typeHourly)
                {
                    auto& column = bc.bounds[0];
                    problem.MatriceDesContraintesCouplantes[k]
                      ->SecondMembreDeLaContrainteCouplante[j]
                      = column[PasDeTempsDebut + j];
                    problem.MatriceDesContraintesCouplantes[k]
                      ->SecondMembreDeLaContrainteCouplanteRef[j]
                      = problem.MatriceDesContraintesCouplantes[k]
                          ->SecondMembreDeLaContrainteCouplante[j];
                }
            }
        }

        const uint dayInTheYear = study.calendar.hours[indx].dayYear;

        for (uint k = 0; k < nbPays; ++k)
        {
            auto& tsIndex = *NumeroChroniquesTireesParPays[numSpace][k];
            auto& area = *(study.areas.byIndex[k]);
            auto& scratchpad = *(area.scratchpad[numSpace]);
            auto& ror = area.hydro.series->ror;

            assert(&scratchpad);
            assert((uint)indx < scratchpad.ts.load.height);
            assert((uint)tsIndex.Consommation < scratchpad.ts.load.width);
            if (parameters.renewableGeneration.isAggregated())
            {
                assert((uint)indx < scratchpad.ts.solar.height);
                assert((uint)indx < scratchpad.ts.wind.height);
                assert((uint)tsIndex.Eolien < scratchpad.ts.wind.width);
                assert((uint)tsIndex.Solar < scratchpad.ts.solar.width);
            }

            uint tsFatalIndex = (uint)tsIndex.Hydraulique < ror.width ? tsIndex.Hydraulique : 0;
            problem.AllMustRunGeneration[j]->AllMustRunGenerationOfArea[k]
              = scratchpad.miscGenSum[indx] + ror[tsFatalIndex][indx] + scratchpad.mustrunSum[indx];

            if (parameters.renewableGeneration.isAggregated())
            {
                problem.AllMustRunGeneration[j]->AllMustRunGenerationOfArea[k]
                  += scratchpad.ts.wind[tsIndex.Eolien][indx]
                     + scratchpad.ts.solar[tsIndex.Solar][indx];
            }

            // Renewable
            if (parameters.renewableGeneration.isClusters())
            {
                area.renewable.list.each([&](const RenewableCluster& cluster) {
                    assert(cluster.series->series.jit == NULL && "No JIT data from the solver");
                    problem.AllMustRunGeneration[j]->AllMustRunGenerationOfArea[k]
                      += cluster.valueAtTimeStep(
                        tsIndex.RenouvelableParPalier[cluster.areaWideIndex], (uint)indx);
                });
            }

            assert(
              !Math::NaN(problem.AllMustRunGeneration[j]->AllMustRunGenerationOfArea[k])
              && "NaN detected for 'AllMustRunGeneration', probably from miscGenSum/mustrunSum");

            problem.ConsommationsAbattues[j]->ConsommationAbattueDuPays[k]
              = +scratchpad.ts.load[tsIndex.Consommation][indx]
                - problem.AllMustRunGeneration[j]->AllMustRunGenerationOfArea[k];

            area.thermal.list.each([&](const Data::ThermalCluster& cluster) {
                assert((uint)tsIndex.ThermiqueParPalier[cluster.areaWideIndex]
                       < cluster.series->series.width);
                assert((uint)indx < cluster.series->series.height);
                assert(cluster.series->series.jit == NULL && "No JIT data from the solver");

                auto& Pt
                  = *problem.PaliersThermiquesDuPays[k]->PuissanceDisponibleEtCout[cluster.index];
                auto& PtValGen = *ValeursGenereesParPays[numSpace][k];

                Pt.PuissanceDisponibleDuPalierThermique[j]
                  = cluster.series->series[tsIndex.ThermiqueParPalier[cluster.areaWideIndex]][indx];

#ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
                if (Pt.PuissanceDisponibleDuPalierThermique[j] == 0.)
                {
                    Pt.CoutHoraireDeProductionDuPalierThermique[j] = studyruntime.globalMaximumCost;
                }
                else
                    Pt.CoutHoraireDeProductionDuPalierThermique[j]
                      = cluster.marketBidCost * cluster.modulation[thermalModulationMarketBid][indx]
                        + PtValGen.AleaCoutDeProductionParPalier[cluster.areaWideIndex];
#else
                Pt.CoutHoraireDeProductionDuPalierThermique[j]
                  = cluster.marketBidCost * cluster.modulation[thermalModulationMarketBid][indx]
                    + PtValGen.AleaCoutDeProductionParPalier[cluster.areaWideIndex];
#endif

                Pt.PuissanceMinDuPalierThermique[j]
                  = (Pt.PuissanceDisponibleDuPalierThermique[j] < cluster.PthetaInf[indx])
                      ? Pt.PuissanceDisponibleDuPalierThermique[j]
                      : cluster.PthetaInf[indx];
            });

            if (problem.CaracteristiquesHydrauliques[k]->PresenceDHydrauliqueModulable > 0)
            {
                problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire[j]
                  = scratchpad.optimalMaxPower[dayInTheYear]
                    * problem.CaracteristiquesHydrauliques[k]->WeeklyGeneratingModulation;
            }

            if (problem.CaracteristiquesHydrauliques[k]->PresenceDePompageModulable)
            {
                problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxPompageHoraire[j]
                  = scratchpad.pumpingMaxPower[dayInTheYear]
                    * problem.CaracteristiquesHydrauliques[k]->WeeklyPumpingModulation;
            }

            problem.ReserveJMoins1[k]->ReserveHoraireJMoins1[j]
              = area.reserves[fhrDayBefore][PasDeTempsDebut + j];
        }
    }

    {
#ifdef ANTARES_USE_GLOBAL_MAXIMUM_COST
        double sum;
        for (uint k = 0; k < nbPays; ++k)
        {
            studyruntime.hydroCostByAreaShouldBeInfinite[k] = false;
            if (problem.CaracteristiquesHydrauliques[k]->PresenceDHydrauliqueModulable > 0)
            {
                sum = 0;
                for (int j = 0; j < 7; ++j)
                {
                    uint dayYear = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                    problem.CaracteristiquesHydrauliques[k]->ContrainteDEnergieHydrauliqueParJour[j]
                      = ValeursGenereesParPays[numSpace][k]->HydrauliqueModulableQuotidien[dayYear];
                    sum += problem.CaracteristiquesHydrauliques[k]
                             ->ContrainteDEnergieHydrauliqueParJour[j];
                }
                if (sum < DBL_EPSILON)
                    studyruntime.hydroCostByAreaShouldBeInfinite[k] = true;
            }
        }
#else
        for (uint k = 0; k < nbPays; ++k)
        {
            if (problem.CaracteristiquesHydrauliques[k]->PresenceDHydrauliqueModulable > 0)
            {
                auto& area = *study.areas.byIndex[k];
                uint tsIndex = (*NumeroChroniquesTireesParPays[numSpace][k]).Hydraulique;
                auto& inflowsmatrix = area.hydro.series->storage;
                auto const& srcinflows = inflowsmatrix[tsIndex < inflowsmatrix.width ? tsIndex : 0];

                if (area.hydro.reservoirManagement)
                {
                    if (not area.hydro.useHeuristicTarget)
                    {
                        for (uint j = 0; j < 7; ++j)
                        {
                            uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                            problem.CaracteristiquesHydrauliques[k]
                              ->MinEnergieHydrauParIntervalleOptimise[j]
                              = 0.;
                            problem.CaracteristiquesHydrauliques[k]
                              ->MaxEnergieHydrauParIntervalleOptimise[j]
                              = area.hydro.maxPower[area.hydro.genMaxP][day]
                                * area.hydro.maxPower[area.hydro.genMaxE][day]
                                * problem.CaracteristiquesHydrauliques[k]
                                    ->WeeklyGeneratingModulation;
                        }
                    }

                    if (area.hydro.useHeuristicTarget && area.hydro.useLeeway)
                    {
                        double* DGU = problem.CaracteristiquesHydrauliques[k]
                                        ->MaxEnergieHydrauParIntervalleOptimise;

                        double* DGL = problem.CaracteristiquesHydrauliques[k]
                                        ->MinEnergieHydrauParIntervalleOptimise;

                        double* DNT
                          = ValeursGenereesParPays[numSpace][k]->HydrauliqueModulableQuotidien;

                        double WSL
                          = problem.CaracteristiquesHydrauliques[k]->NiveauInitialReservoir;

                        double LUB = area.hydro.leewayUpperBound;

                        double LLB = area.hydro.leewayLowerBound;

                        double DGM
                          = problem.CaracteristiquesHydrauliques[k]->WeeklyGeneratingModulation;

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

                            double DGC = area.hydro.maxPower[area.hydro.genMaxP][day]
                                         * area.hydro.maxPower[area.hydro.genMaxE][day];

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
                        weekTarget_tmp += ValeursGenereesParPays[numSpace][k]
                                            ->HydrauliqueModulableQuotidien[day];
                    }

                    if (weekTarget_tmp != 0.)
                        weekGenerationTarget = weekTarget_tmp;

                    marginGen = weekGenerationTarget;

                    if (problem.CaracteristiquesHydrauliques[k]->NiveauInitialReservoir
                        < weekTarget_tmp)
                        marginGen = problem.CaracteristiquesHydrauliques[k]->NiveauInitialReservoir;
                }

                if (not problem.CaracteristiquesHydrauliques[k]->TurbinageEntreBornes)
                {
                    for (uint j = 0; j < 7; ++j)
                    {
                        uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                        problem.CaracteristiquesHydrauliques[k]
                          ->CntEnergieH2OParIntervalleOptimise[j]
                          = ValeursGenereesParPays[numSpace][k]->HydrauliqueModulableQuotidien[day]
                            * problem.CaracteristiquesHydrauliques[k]->WeeklyGeneratingModulation
                            * marginGen / weekGenerationTarget;
                    }
                }

                for (uint j = 0; j < 7; ++j)
                {
                    uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                    problem.CaracteristiquesHydrauliques[k]->InflowForTimeInterval[j]
                      = srcinflows[day];
                    for (int h = 0; h < 24; h++)
                    {
                        problem.CaracteristiquesHydrauliques[k]->ApportNaturelHoraire[j * 24 + h]
                          = srcinflows[day] / 24;
                    }
                }

                if (problem.CaracteristiquesHydrauliques[k]->PresenceDePompageModulable
                    == OUI_ANTARES)
                {
                    if (area.hydro.reservoirManagement) /* No need to include the condition "use
                                                           water value" */
                    {
                        if (not area.hydro.useHeuristicTarget)
                        {
                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                                problem.CaracteristiquesHydrauliques[k]
                                  ->MaxEnergiePompageParIntervalleOptimise[j]
                                  = area.hydro.maxPower[area.hydro.pumpMaxP][day]
                                    * area.hydro.maxPower[area.hydro.pumpMaxE][day]
                                    * problem.CaracteristiquesHydrauliques[k]
                                        ->WeeklyPumpingModulation;
                            }
                        }

                        if (area.hydro.useHeuristicTarget)
                        {
                            double WNI = 0.;
                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                                WNI += srcinflows[day];
                            }

                            double* DPU = problem.CaracteristiquesHydrauliques[k]
                                            ->MaxEnergiePompageParIntervalleOptimise;

                            double WSL
                              = problem.CaracteristiquesHydrauliques[k]->NiveauInitialReservoir;

                            double DPM
                              = problem.CaracteristiquesHydrauliques[k]->WeeklyPumpingModulation;

                            double pumping_ratio = area.hydro.pumpingEfficiency;

                            double WPU = 0.;

                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;

                                double DPC = area.hydro.maxPower[area.hydro.pumpMaxP][day]
                                             * area.hydro.maxPower[area.hydro.pumpMaxE][day];

                                WPU += DPC;
                            }

                            double U = WPU * DPM * pumping_ratio;

                            for (uint j = 0; j < 7; ++j)
                            {
                                uint day = study.calendar.hours[PasDeTempsDebut + j * 24].dayYear;
                                double DPC = area.hydro.maxPower[area.hydro.pumpMaxP][day]
                                             * area.hydro.maxPower[area.hydro.pumpMaxE][day];
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
#endif
    }

    for (uint k = 0; k < nbPays; ++k)
    {
        auto& area = *study.areas.byIndex[k];

        for (uint l = 0; l != area.thermal.list.size(); ++l)
        {
            memcpy((char*)problem.PaliersThermiquesDuPays[k]
                     ->PuissanceDisponibleEtCout[l]
                     ->PuissanceDisponibleDuPalierThermiqueRef,
                   (char*)problem.PaliersThermiquesDuPays[k]
                     ->PuissanceDisponibleEtCout[l]
                     ->PuissanceDisponibleDuPalierThermique,
                   pasDeTempsSizeDouble);
            memcpy((char*)problem.PaliersThermiquesDuPays[k]
                     ->PuissanceDisponibleEtCout[l]
                     ->CoutHoraireDeProductionDuPalierThermiqueRef,
                   (char*)problem.PaliersThermiquesDuPays[k]
                     ->PuissanceDisponibleEtCout[l]
                     ->CoutHoraireDeProductionDuPalierThermique,
                   pasDeTempsSizeDouble);
        }

        memcpy(
          (char*)problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimiseRef,
          (char*)problem.CaracteristiquesHydrauliques[k]->CntEnergieH2OParIntervalleOptimise,
          7 * sizeof(double));
        memcpy(
          (char*)problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraireRef,
          (char*)problem.CaracteristiquesHydrauliques[k]->ContrainteDePmaxHydrauliqueHoraire,
          pasDeTempsSizeDouble);

        memcpy((char*)problem.ReserveJMoins1[k]->ReserveHoraireJMoins1Ref,
               (char*)problem.ReserveJMoins1[k]->ReserveHoraireJMoins1,
               pasDeTempsSizeDouble);
    }
    for (int j = 0; j < problem.NombreDePasDeTemps; ++j)
    {
        memcpy((char*)problem.ConsommationsAbattuesRef[j]->ConsommationAbattueDuPays,
               (char*)problem.ConsommationsAbattues[j]->ConsommationAbattueDuPays,
               nbPays * sizeof(double));
    }

#ifdef ANTARES_SWAP_SUPPORT

    Antares::memory.flushAll();
#endif
}
