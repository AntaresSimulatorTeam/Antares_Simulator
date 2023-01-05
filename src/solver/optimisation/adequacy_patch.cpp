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

#include "../simulation/simulation.h"
#include "../optimisation/opt_fonctions.h"
#include "adequacy_patch.h"
#include <antares/study/area/scratchpad.h>
#include <cmath>
#include <cassert>

namespace Antares::Data::AdequacyPatch
{
/*!
 * Determines restriction type for transmission links for first step of adequacy patch, when start
 * node is inside adq path (type 2).
 *
 * @param ExtremityNodeAdequacyPatchType uint: The adq type of the node at the end of the link.
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static ntcSetToZeroStatus_AdqPatchStep1 getNTC_OriginNodeInside(AdequacyPatchMode extremityType)
{
    switch (extremityType)
    {
    case physicalAreaInsideAdqPatch:
    case physicalAreaOutsideAdqPatch:
        return setToZero;
    default:
        return leaveLocalValues;
    }
}

/*!
 * Determines restriction type for transmission links for first step of adequacy patch, when start
 * node is outside adq path (type 1).
 *
 * @param extremityType uint: The adq type of the node at the end of the link.
 *
 * @param SetToZero12Links bool: Switch to cut links from nodes outside adq patch
 * (type 1) towards nodes inside adq patch (type 2).
 *
 * @param SetToZero11Links bool: Switch to cut links between nodes outside adq patch
 * (type 1).
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static ntcSetToZeroStatus_AdqPatchStep1 getNTC_OriginNodeOutside(AdequacyPatchMode extremityType,
                                                                 bool SetToZero12Links,
                                                                 bool SetToZero11Links)
{
    switch (extremityType)
    {
    case physicalAreaInsideAdqPatch:
        return SetToZero12Links ? setToZero : setExtremityOriginToZero;
    case physicalAreaOutsideAdqPatch:
        return SetToZero11Links ? setToZero : leaveLocalValues;
    default:
        return leaveLocalValues;
    }
}

/*!
 * Determines restriction type for transmission links for first step of adequacy patch.
 *
 * @param originType uint: The adq type of the node at the start of the link.
 *
 * @param extremityType uint: The adq type of the node at the end of the link.
 *
 * @param SetToZero12Links bool: Switch to cut links from nodes outside adq patch
 * (type 1) towards nodes inside adq patch (type 2).
 *
 * @param SetToZero11Links bool: Switch to cut links between nodes outside adq patch
 * (type 1).
 *
 * @return uint from an enumeration that describes the type of restrictions to put on this link for
 * adq purposes.
 */
static ntcSetToZeroStatus_AdqPatchStep1 getNTCStatus(const PROBLEME_HEBDO* ProblemeHebdo,
                                                     const int Interco)
{
    assert(ProblemeHebdo);
    assert(ProblemeHebdo);

    const AdequacyPatchMode originType
      = ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco];
    const AdequacyPatchMode extremityType
      = ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco];
    const bool SetToZero12Links = ProblemeHebdo->adqPatchParams->SetNTCOutsideToInsideToZero;
    const bool SetToZero11Links = ProblemeHebdo->adqPatchParams->SetNTCOutsideToOutsideToZero;

    switch (originType)
    {
    case physicalAreaInsideAdqPatch:
        return getNTC_OriginNodeInside(extremityType);

    case physicalAreaOutsideAdqPatch:
        return getNTC_OriginNodeOutside(extremityType, SetToZero12Links, SetToZero11Links);
    default:
        return leaveLocalValues;
    }
}

void setBoundsAdqPatch(double& Xmax,
                       double& Xmin,
                       VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                       const int Interco,
                       PROBLEME_HEBDO* ProblemeHebdo)
{
    const auto ntcToZeroStatus = getNTCStatus(ProblemeHebdo, Interco);
    switch (ntcToZeroStatus)
    {
    case setToZero:
    {
        Xmax = 0.;
        Xmin = 0.;
        break;
    }
    case setOrigineExtremityToZero:
    {
        Xmax = 0.;
        Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
        break;
    }
    case setExtremityOriginToZero:
    {
        Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
        Xmin = 0.;
        break;
    }
    default:
    {
        setBoundsNoAdqPatch(Xmax, Xmin, ValeursDeNTC, Interco);
        break;
    }
    }
}

void setBoundsNoAdqPatch(double& Xmax,
                         double& Xmin,
                         VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                         const int Interco)
{
    Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
    Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);
}

double LmrViolationAreaHour(PROBLEME_HEBDO* ProblemeHebdo,
                            double totalNodeBalance,
                            int Area,
                            int hour)
{
    double ensInit
      = ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
    double threshold = ProblemeHebdo->adqPatchParams->ThresholdDisplayLocalMatchingRuleViolations;

    ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 0;
    // check LMR violations
    if ((ensInit > 0.0) && (totalNodeBalance < 0.0)
        && (std::fabs(totalNodeBalance) > ensInit + std::fabs(threshold)))
    {
        ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 1;
        return std::fabs(totalNodeBalance);
    }
    return 0.0;
}

double calculateDensNewAndTotalLmrViolation(PROBLEME_HEBDO* ProblemeHebdo,
                                            const Study& study,
                                            uint numSpace)
{
    double netPositionInit;
    double densNew;
    double totalNodeBalance;
    double totalLmrViolation = 0.0;
    const int numOfHoursInWeek = 168;

    for (int Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (int hour = 0; hour < numOfHoursInWeek; hour++)
            {
                std::tie(netPositionInit, densNew, totalNodeBalance)
                  = calculateAreaFlowBalance(ProblemeHebdo, Area, hour);
                // adjust densNew according to the new specification/request by ELIA
                /* DENS_new (node A) = max [ 0; ENS_init (node A) + net_position_init (node A)
                                        + ∑ flows (node 1 -> node A) - DTG.MRG(node A)] */
                auto& scratchpad = *(study.areas[Area]->scratchpad[numSpace]);
                double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];
                densNew = std::max(0.0, densNew - dtgMrg);
                // write down densNew values for all the hours
                ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDENS[hour] = densNew;
                // copy spilled Energy values into spilled Energy values after CSR
                ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesSpilledEnergyAfterCSR[hour]
                  = ProblemeHebdo->ResultatsHoraires[Area]
                      ->ValeursHorairesDeDefaillanceNegative[hour];
                // check LMR violations
                totalLmrViolation
                  += LmrViolationAreaHour(ProblemeHebdo, totalNodeBalance, Area, hour);
            }
        }
    }
    return totalLmrViolation;
}

std::tuple<double, double, double> calculateAreaFlowBalance(PROBLEME_HEBDO* ProblemeHebdo,
                                                            int Area,
                                                            int hour)
{
    int Interco;
    double netPositionInit = 0;
    double flowsNode1toNodeA = 0;
    double ensInit;
    double densNew;
    bool includeFlowsOutsideAdqPatchToDensNew
      = !ProblemeHebdo->adqPatchParams->SetNTCOutsideToInsideToZero;

    Interco = ProblemeHebdo->IndexDebutIntercoOrigine[Area];
    while (Interco >= 0)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit -= ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
        }
        else if (ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA
              -= std::min(0.0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
        }
        Interco = ProblemeHebdo->IndexSuivantIntercoOrigine[Interco];
    }
    Interco = ProblemeHebdo->IndexDebutIntercoExtremite[Area];
    while (Interco >= 0)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit += ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
        }
        else if (ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA
              += std::max(0.0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
        }
        Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
    }

    ensInit = ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
    if (includeFlowsOutsideAdqPatchToDensNew)
    {
        densNew = std::max(0.0, ensInit + netPositionInit + flowsNode1toNodeA);
        return std::make_tuple(netPositionInit, densNew, netPositionInit + flowsNode1toNodeA);
    }
    else
    {
        densNew = std::max(0.0, ensInit + netPositionInit);
        return std::make_tuple(netPositionInit, densNew, netPositionInit);
    }
}

void addArray(std::vector<double>& A, const double* B)
{
    for (uint i = 0; i < A.size(); ++i)
        A[i] += B[i];
}

void adqPatchPostProcess(const Data::Study& study, PROBLEME_HEBDO& problem, int numSpace)
{
    if (!study.parameters.adqPatch.enabled)
        return;

    const int numOfHoursInWeek = 168;
    for (int Area = 0; Area < problem.NombreDePays; Area++)
    {
        if (problem.adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (int hour = 0; hour < numOfHoursInWeek; hour++)
            {
                // define access to the required variables
                auto& scratchpad = *(study.areas[Area]->scratchpad[numSpace]);
                double dtgMrg = scratchpad.dispatchableGenerationMargin[hour];

                auto& hourlyResults = *(problem.ResultatsHoraires[Area]);
                double& dtgMrgCsr = hourlyResults.ValeursHorairesDtgMrgCsr[hour];
                double& ens = hourlyResults.ValeursHorairesDeDefaillancePositive[hour];
                double& mrgCost = hourlyResults.CoutsMarginauxHoraires[hour];
                // calculate DTG MRG CSR and adjust ENS if neccessary
                if (dtgMrgCsr == -1.0) // area is inside adq-patch and it is CSR triggered hour
                {
                    dtgMrgCsr = std::max(0.0, dtgMrg - ens);
                    ens = std::max(0.0, ens - dtgMrg);
                    // set MRG PRICE to value of unsupplied energy cost, if LOLD=1.0 (ENS>0.5)
                    if (ens > 0.5)
                        mrgCost = -study.areas[Area]->thermal.unsuppliedEnergyCost;
                }
                else
                    dtgMrgCsr = dtgMrg;
            }
        }
    }
}
} // namespace Antares::Data::AdequacyPatch

// TODO[FOM] Place inside namespace Antares::Data::AdequacyPatch
void HOURLY_CSR_PROBLEM::calculateCsrParameters()
{
    double netPositionInit;
    double ensInit;
    double spillageInit;
    int hour = hourInWeekTriggeredCsr;

    for (int Area = 0; Area < pWeeklyProblemBelongedTo->NombreDePays; Area++)
    {
        if (pWeeklyProblemBelongedTo->adequacyPatchRuntimeData.areaMode[Area]
            == physicalAreaInsideAdqPatch)
        {
            // set DTG MRG CSR in all areas inside adq-path for all CSR triggered hours to -1.0
            pWeeklyProblemBelongedTo->ResultatsHoraires[Area]->ValeursHorairesDtgMrgCsr[hour]
              = -1.0;
            // calculate netPositionInit and the RHS of the AreaBalance constraints
            std::tie(netPositionInit, std::ignore, std::ignore)
              = calculateAreaFlowBalance(pWeeklyProblemBelongedTo, Area, hour);

            ensInit = pWeeklyProblemBelongedTo->ResultatsHoraires[Area]
                        ->ValeursHorairesDeDefaillancePositive[hour];
            spillageInit = pWeeklyProblemBelongedTo->ResultatsHoraires[Area]
                             ->ValeursHorairesDeDefaillanceNegative[hour];

            rhsAreaBalanceValues[Area] = ensInit + netPositionInit - spillageInit;
        }
    }
    return;
}

void HOURLY_CSR_PROBLEM::resetProblem()
{
    OPT_LiberationProblemesSimplexe(pWeeklyProblemBelongedTo);
}

void HOURLY_CSR_PROBLEM::buildProblemVariables()
{
    OPT_ConstruireLaListeDesVariablesOptimiseesDuProblemeQuadratique_CSR(pWeeklyProblemBelongedTo,
                                                                         *this);
}

void HOURLY_CSR_PROBLEM::buildProblemConstraintsLHS()
{
    OPT_ConstruireLaMatriceDesContraintesDuProblemeQuadratique_CSR(pWeeklyProblemBelongedTo, *this);
}

void HOURLY_CSR_PROBLEM::setVariableBounds()
{
    OPT_InitialiserLesBornesDesVariablesDuProblemeQuadratique_CSR(pWeeklyProblemBelongedTo, *this);
}

void HOURLY_CSR_PROBLEM::buildProblemConstraintsRHS()
{
    OPT_InitialiserLeSecondMembreDuProblemeQuadratique_CSR(pWeeklyProblemBelongedTo, *this);
}

void HOURLY_CSR_PROBLEM::setProblemCost()
{
    OPT_InitialiserLesCoutsQuadratiques_CSR(pWeeklyProblemBelongedTo, *this);
}

void HOURLY_CSR_PROBLEM::solveProblem(uint week, int year)
{
    ADQ_PATCH_CSR(pWeeklyProblemBelongedTo->ProblemeAResoudre, *this, week, year);
}

void HOURLY_CSR_PROBLEM::run(uint week, const Antares::Solver::Variable::State& state)
{
    resetProblem();
    calculateCsrParameters();
    buildProblemVariables();
    buildProblemConstraintsLHS();
    setVariableBounds();
    buildProblemConstraintsRHS();
    setProblemCost();
    solveProblem(week, state.year);
}
