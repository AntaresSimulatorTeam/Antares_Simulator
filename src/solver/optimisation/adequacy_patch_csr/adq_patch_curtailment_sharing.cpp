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

#include "adq_patch_curtailment_sharing.h"
#include "../opt_fonctions.h"
#include "csr_quadratic_problem.h"
#include "count_constraints_variables.h"
#include "../simulation/adequacy_patch_runtime_data.h"

#include <cmath>
#include "../study/area/scratchpad.h"

using namespace Yuni;

namespace Antares::Data::AdequacyPatch
{
double LmrViolationAreaHour(const PROBLEME_HEBDO* problemeHebdo,
                            double totalNodeBalance,
                            int Area,
                            int hour)
{
    double ensInit
      = problemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
    double threshold = problemeHebdo->adqPatchParams->ThresholdDisplayLocalMatchingRuleViolations;

    problemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 0;
    // check LMR violations
    if ((ensInit > 0.0) && (totalNodeBalance < 0.0)
        && (std::fabs(totalNodeBalance) > ensInit + std::fabs(threshold)))
    {
        problemeHebdo->ResultatsHoraires[Area]->ValeursHorairesLmrViolations[hour] = 1;
        return std::fabs(totalNodeBalance);
    }
    return 0.0;
}

std::tuple<double, double, double> calculateAreaFlowBalance(PROBLEME_HEBDO* problemeHebdo,
                                                            int Area,
                                                            int hour)
{
    double netPositionInit = 0;
    double flowsNode1toNodeA = 0;
    double densNew;
    bool includeFlowsOutsideAdqPatchToDensNew
      = !problemeHebdo->adqPatchParams->SetNTCOutsideToInsideToZero;

    int Interco = problemeHebdo->IndexDebutIntercoOrigine[Area];
    while (Interco >= 0)
    {
        if (problemeHebdo->adequacyPatchRuntimeData->extremityAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit -= problemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
        }
        else if (problemeHebdo->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA
              -= std::min(0.0, problemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
        }
        Interco = problemeHebdo->IndexSuivantIntercoOrigine[Interco];
    }
    Interco = problemeHebdo->IndexDebutIntercoExtremite[Area];
    while (Interco >= 0)
    {
        if (problemeHebdo->adequacyPatchRuntimeData->originAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit += problemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco];
        }
        else if (problemeHebdo->adequacyPatchRuntimeData->originAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA
              += std::max(0.0, problemeHebdo->ValeursDeNTC[hour]->ValeurDuFlux[Interco]);
        }
        Interco = problemeHebdo->IndexSuivantIntercoExtremite[Interco];
    }

    double ensInit
      = problemeHebdo->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
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

} // namespace Antares::Data::AdequacyPatch

void HourlyCSRProblem::calculateCsrParameters()
{
    double netPositionInit;
    int hour = triggeredHour;

    for (int Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area]
            == Antares::Data::AdequacyPatch::physicalAreaInsideAdqPatch)
        {
            problemeHebdo_->adequacyPatchRuntimeData->addCSRTriggeredAtAreaHour(Area, hour);

            // calculate netPositionInit and the RHS of the AreaBalance constraints
            std::tie(netPositionInit, std::ignore, std::ignore)
              = Antares::Data::AdequacyPatch::calculateAreaFlowBalance(problemeHebdo_, Area, hour);

            double ensInit
              = problemeHebdo_->ResultatsHoraires[Area]->ValeursHorairesDeDefaillancePositive[hour];
            double spillageInit
              = problemeHebdo_->ResultatsHoraires[Area]->ValeursHorairesDeDefaillanceNegative[hour];

            rhsAreaBalanceValues[Area] = ensInit + netPositionInit - spillageInit;
        }
    }
}

void HourlyCSRProblem::resetProblem()
{
    OPT_FreeOptimizationData(&problemeAResoudre_);
}

void HourlyCSRProblem::allocateProblem()
{
    using namespace Antares::Data::AdequacyPatch;
    int nbConst;

    problemeAResoudre_.NombreDeVariables = countVariables(problemeHebdo_);
    nbConst = problemeAResoudre_.NombreDeContraintes = countConstraints(problemeHebdo_);
    int nbTerms
      = 3 * nbConst; // This is a rough estimate, reallocations may happen later if it's too low
    OPT_AllocateFromNumberOfVariableConstraints(&problemeAResoudre_, nbTerms);
}

void HourlyCSRProblem::buildProblemVariables()
{
    logs.debug() << "[CSR] variable list:";

    constructVariableENS();
    constructVariableSpilledEnergy();
    constructVariableFlows();
}

void HourlyCSRProblem::buildProblemConstraintsLHS()
{
    Antares::Solver::Optimization::CsrQuadraticProblem csrProb(
      problemeHebdo_, problemeAResoudre_, *this);
    csrProb.buildConstraintMatrix();
}

void HourlyCSRProblem::setVariableBounds()
{
    for (int var = 0; var < problemeAResoudre_.NombreDeVariables; var++)
        problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;

    logs.debug() << "[CSR] bounds";
    setBoundsOnENS();
    setBoundsOnSpilledEnergy();
    setBoundsOnFlows();
}

void HourlyCSRProblem::buildProblemConstraintsRHS()
{
    logs.debug() << "[CSR] RHS: ";
    setRHSvalueOnFlows();
    setRHSnodeBalanceValue();
    setRHSbindingConstraintsValue();
}

void HourlyCSRProblem::setProblemCost()
{
    logs.debug() << "[CSR] cost";
    std::fill_n(problemeAResoudre_.CoutLineaire, problemeAResoudre_.NombreDeVariables, 0.);
    std::fill_n(problemeAResoudre_.CoutQuadratique, problemeAResoudre_.NombreDeVariables, 0.);

    setQuadraticCost();
    if (problemeHebdo_->adqPatchParams->IncludeHurdleCostCsr)
        setLinearCost();
}

void HourlyCSRProblem::solveProblem(uint week, int year)
{
    ADQ_PATCH_CSR(problemeAResoudre_, *this, week, year);
}

void HourlyCSRProblem::run(uint week, uint year)
{
    calculateCsrParameters();
    buildProblemVariables();
    buildProblemConstraintsLHS();
    setVariableBounds();
    buildProblemConstraintsRHS();
    setProblemCost();
    solveProblem(week, year);
}
