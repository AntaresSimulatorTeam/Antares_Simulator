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

#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"

#include <cmath>

#include "antares/solver/optimisation/adequacy_patch_csr/count_constraints_variables.h"
#include "antares/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

#include "solve_problem.h"

using namespace Yuni;

namespace Antares::Data::AdequacyPatch
{
double LmrViolationAreaHour(PROBLEME_HEBDO* problemeHebdo,
                            double totalNodeBalance,
                            double threshold,
                            int Area,
                            int hour)
{
    const double ensInit = problemeHebdo->ResultatsHoraires[Area]
                             .ValeursHorairesDeDefaillancePositive[hour];

    problemeHebdo->ResultatsHoraires[Area].ValeursHorairesLmrViolations[hour] = 0;
    // check LMR violations
    if ((ensInit > 0.0) && (totalNodeBalance < 0.0)
        && (std::fabs(totalNodeBalance) > ensInit + std::fabs(threshold)))
    {
        problemeHebdo->ResultatsHoraires[Area].ValeursHorairesLmrViolations[hour] = 1;
        return std::fabs(totalNodeBalance);
    }
    return 0.0;
}

std::tuple<double, double, double> calculateAreaFlowBalance(PROBLEME_HEBDO* problemeHebdo,
                                                            bool setNTCOutsideToInsideToZero,
                                                            int Area,
                                                            int hour)
{
    double netPositionInit = 0;
    double flowsNode1toNodeA = 0;
    double densNew;

    int Interco = problemeHebdo->IndexDebutIntercoOrigine[Area];
    while (Interco >= 0)
    {
        if (problemeHebdo->adequacyPatchRuntimeData->extremityAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit -= problemeHebdo->ValeursDeNTC[hour].ValeurDuFlux[Interco];
        }
        else if (problemeHebdo->adequacyPatchRuntimeData->extremityAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA -= std::min(0.0,
                                          problemeHebdo->ValeursDeNTC[hour].ValeurDuFlux[Interco]);
        }
        Interco = problemeHebdo->IndexSuivantIntercoOrigine[Interco];
    }
    Interco = problemeHebdo->IndexDebutIntercoExtremite[Area];
    while (Interco >= 0)
    {
        if (problemeHebdo->adequacyPatchRuntimeData->originAreaMode[Interco]
            == physicalAreaInsideAdqPatch)
        {
            netPositionInit += problemeHebdo->ValeursDeNTC[hour].ValeurDuFlux[Interco];
        }
        else if (problemeHebdo->adequacyPatchRuntimeData->originAreaMode[Interco]
                 == physicalAreaOutsideAdqPatch)
        {
            flowsNode1toNodeA += std::max(0.0,
                                          problemeHebdo->ValeursDeNTC[hour].ValeurDuFlux[Interco]);
        }
        Interco = problemeHebdo->IndexSuivantIntercoExtremite[Interco];
    }

    double ensInit = problemeHebdo->ResultatsHoraires[Area]
                       .ValeursHorairesDeDefaillancePositive[hour];
    if (!setNTCOutsideToInsideToZero)
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
    using namespace Antares::Data::AdequacyPatch;
    double netPositionInit;
    int hour = triggeredHour;

    for (uint32_t Area = 0; Area < problemeHebdo_->NombreDePays; Area++)
    {
        if (problemeHebdo_->adequacyPatchRuntimeData->areaMode[Area] == physicalAreaInsideAdqPatch)
        {
            problemeHebdo_->adequacyPatchRuntimeData->addCSRTriggeredAtAreaHour(Area, hour);

            // calculate netPositionInit and the RHS of the AreaBalance constraints
            std::tie(netPositionInit, std::ignore, std::ignore) = calculateAreaFlowBalance(
              problemeHebdo_,
              adqPatchParams_.setToZeroOutsideInsideLinks,
              Area,
              hour);
            double ensInit = problemeHebdo_->ResultatsHoraires[Area]
                               .ValeursHorairesDeDefaillancePositive[hour];
            double spillageInit = problemeHebdo_->ResultatsHoraires[Area]
                                    .ValeursHorairesDeDefaillanceNegative[hour];

            rhsAreaBalanceValues[Area] = ensInit + netPositionInit - spillageInit;
        }
    }
}

void HourlyCSRProblem::allocateProblem()
{
    using namespace Antares::Data::AdequacyPatch;
    problemeAResoudre_.NombreDeVariables = countVariables(problemeHebdo_);
    problemeAResoudre_.NombreDeContraintes = countConstraints(problemeHebdo_);
    OPT_AllocateFromNumberOfVariableConstraints(&problemeAResoudre_);
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
    Antares::Solver::Optimization::CsrQuadraticProblem csrProb(problemeHebdo_,
                                                               problemeAResoudre_,
                                                               *this);
    csrProb.buildConstraintMatrix();
}

void HourlyCSRProblem::setVariableBounds()
{
    for (int var = 0; var < problemeAResoudre_.NombreDeVariables; var++)
    {
        problemeAResoudre_.AdresseOuPlacerLaValeurDesVariablesOptimisees[var] = nullptr;
    }

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
    problemeAResoudre_.CoutLineaire.assign(problemeAResoudre_.NombreDeVariables, 0.);
    problemeAResoudre_.CoutQuadratique.assign(problemeAResoudre_.NombreDeVariables, 0.);

    setQuadraticCost();
    if (adqPatchParams_.curtailmentSharing.includeHurdleCost)
    {
        setLinearCost();
    }
}

void HourlyCSRProblem::solveProblem(uint week, int year)
{
    ADQ_PATCH_CSR(problemeAResoudre_, *this, adqPatchParams_, week, year);
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
