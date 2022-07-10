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
#include "opt_fonctions.h"
#include "adequacy_patch.h"
#include <math.h>
#include "../simulation/sim_structure_probleme_economique.h"

using namespace Yuni;

namespace Antares
{
namespace Data
{
namespace AdequacyPatch
{
ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatus(PROBLEME_HEBDO* ProblemeHebdo, int Interco)
{
    AdequacyPatchMode OriginNodeAdequacyPatchType
      = ProblemeHebdo->adequacyPatchRuntimeData.originAreaMode[Interco];
    AdequacyPatchMode ExtremityNodeAdequacyPatchType
      = ProblemeHebdo->adequacyPatchRuntimeData.extremityAreaMode[Interco];
    bool setToZeroNTCfromOutToIn_AdqPatch
      = ProblemeHebdo->adqPatchParams->SetNTCOutsideToInsideToZero;
    bool setToZeroNTCfromOutToOut_AdqPatch
      = ProblemeHebdo->adqPatchParams->SetNTCOutsideToOutsideToZero;

    switch (OriginNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return getNTCtoZeroStatusOriginNodeInsideAdq(ExtremityNodeAdequacyPatchType,
                                                     setToZeroNTCfromOutToIn_AdqPatch);
    case physicalAreaOutsideAdqPatch:
        return getNTCtoZeroStatusOriginNodeOutsideAdq(ExtremityNodeAdequacyPatchType,
                                                      setToZeroNTCfromOutToIn_AdqPatch,
                                                      setToZeroNTCfromOutToOut_AdqPatch);
    default:
        return leaveLocalValues;
    }
}

ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatusOriginNodeInsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
    case physicalAreaOutsideAdqPatch:
        return setToZero;
    default:
        return leaveLocalValues;
    }
}

ntcSetToZeroStatus_AdqPatchStep1 getNTCtoZeroStatusOriginNodeOutsideAdq(
  AdequacyPatchMode ExtremityNodeAdequacyPatchType,
  bool setToZeroNTCfromOutToIn_AdqPatch,
  bool setToZeroNTCfromOutToOut_AdqPatch)
{
    switch (ExtremityNodeAdequacyPatchType)
    {
    case physicalAreaInsideAdqPatch:
        return (setToZeroNTCfromOutToIn_AdqPatch) ? setToZero : setExtremityOrigineToZero;
    case physicalAreaOutsideAdqPatch:
        return (setToZeroNTCfromOutToOut_AdqPatch) ? setToZero : leaveLocalValues;
    default:
        return leaveLocalValues;
    }
}

void setNTCbounds(double& Xmax,
                  double& Xmin,
                  VALEURS_DE_NTC_ET_RESISTANCES* ValeursDeNTC,
                  const int Interco,
                  PROBLEME_HEBDO* ProblemeHebdo)
{
    ntcSetToZeroStatus_AdqPatchStep1 ntcToZeroStatusForAdqPatch;

    // set as default values
    Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
    Xmin = -(ValeursDeNTC->ValeurDeNTCExtremiteVersOrigine[Interco]);

    // set for adq patch first step
    if (ProblemeHebdo->adqPatchParams && ProblemeHebdo->adqPatchParams->AdequacyFirstStep)
    {
        ntcToZeroStatusForAdqPatch = getNTCtoZeroStatus(ProblemeHebdo, Interco);

        switch (ntcToZeroStatusForAdqPatch)
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
        case setExtremityOrigineToZero:
        {
            Xmax = ValeursDeNTC->ValeurDeNTCOrigineVersExtremite[Interco];
            Xmin = 0.;
            break;
        }
        }
    }
}

void checkLocalMatchingRuleViolations(PROBLEME_HEBDO* ProblemeHebdo, uint weekNb)
{
    float threshold = ProblemeHebdo->adqPatchParams->ThresholdDisplayLocalMatchingRuleViolations;
    double netPositionInit;
    double densNew;
    double ensInit;
    const int numOfHoursInWeek = 168;
    double totalLmrViolation = 0;

    for (int Area = 0; Area < ProblemeHebdo->NombreDePays; Area++)
    {
        if (ProblemeHebdo->adequacyPatchRuntimeData.areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            for (int hour = 0; hour < numOfHoursInWeek; hour++)
            {
                std::tie(netPositionInit, densNew)
                  = calculateAreaFlowBalance(ProblemeHebdo, Area, hour);

                ensInit = ProblemeHebdo->ResultatsHoraires[Area]
                            ->ValeursHorairesDeDefaillancePositive[hour];

                // check LMR violations
                ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 0;
                if ((densNew < ensInit) && (ensInit - densNew > Math::Abs(threshold)))
                {
                    ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 1;
                    totalLmrViolation += (ensInit - densNew);
                }
            }
        }
    }
    logs.debug() << "Week:" << weekNb + 1 << ". Total LMR violation:" << totalLmrViolation;
    return;
}

std::pair<double, double> calculateAreaFlowBalance(PROBLEME_HEBDO* ProblemeHebdo,
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
              -= Math::Min(0.0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
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
              += Math::Max(0.0, ProblemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
        }
        Interco = ProblemeHebdo->IndexSuivantIntercoExtremite[Interco];
    }

    ensInit = ProblemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
    if (includeFlowsOutsideAdqPatchToDensNew)
        densNew = Math::Max(0.0, ensInit + netPositionInit + flowsNode1toNodeA);
    else
        densNew = Math::Max(0.0, ensInit + netPositionInit);

    return std::make_pair(netPositionInit, densNew);
}

void addArray(std::vector<double>& A, double* B, int num)
{
    for (uint i = 0; i < num; ++i)
        A[i] += B[i];
}

} // end namespace Antares
} // end namespace Data
} // end namespace AdequacyPatch

void HOURLY_CSR_PROBLEM::calculateCsrParameters()
{
    double netPositionInit;
    double densNew;
    double ensInit;
    double spillageInit;
    int hour = hourInWeekTriggeredCsr;

    for (int Area = 0; Area < pWeeklyProblemBelongedTo->NombreDePays; Area++)
    {
        if (pWeeklyProblemBelongedTo->adequacyPatchRuntimeData.areaMode[Area]
            == physicalAreaInsideAdqPatch)
        {
            std::tie(netPositionInit, densNew)
              = calculateAreaFlowBalance(pWeeklyProblemBelongedTo, Area, hour);

            ensInit = pWeeklyProblemBelongedTo->ResultatsHoraires[Area]
                        ->ValeursHorairesDeDefaillancePositive[hour];
            spillageInit = pWeeklyProblemBelongedTo->ResultatsHoraires[Area]
                             ->ValeursHorairesDeDefaillanceNegative[hour];

            densNewValues[Area] = densNew;
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

void HOURLY_CSR_PROBLEM::solveProblem()
{
    OPT_AppelDuSolveurQuadratique_CSR(pWeeklyProblemBelongedTo->ProblemeAResoudre, *this);
}

void HOURLY_CSR_PROBLEM::run()
{
    resetProblem();
    calculateCsrParameters();
    buildProblemVariables();
    buildProblemConstraintsLHS();
    setVariableBounds();
    buildProblemConstraintsRHS();
    setProblemCost();
    solveProblem();
}