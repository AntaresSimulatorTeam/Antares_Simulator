/*
** Copyright 2007-2022 RTE
** Authors: RTE-international / Redstork / Antares_Simulator Team
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

#include <vector>
#include "../solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../simulation/adequacy_patch_runtime_data.h"
#include "../solver/optimisation/opt_fonctions.h"
#include "csr_quadratic_problem.h"
#include "hourly_csr_problem.h"
#include "sim_structure_probleme_economique.h"
#include "constraints/CsrFlowDissociation.h"
#include "constraints/CsrAreaBalance.h"
#include "constraints/CsrBindingConstraintHour.h"
#include "../constraints/constraint_builder_utils.h"

using namespace Antares::Data;

namespace Antares::Solver::Optimization
{

void CsrQuadraticProblem::setConstraintsOnFlows(ConstraintBuilder& builder)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    //!\ TODO not associated problemHebdo && probleamAressoudre
    CsrFlowDissociationData csrFlowDissociationData
      = {.numberOfConstraintCsrFlowDissociation
         = hourlyCsrProblem_.numberOfConstraintCsrFlowDissociation,
         .NombreDInterconnexions = problemeHebdo_->NombreDInterconnexions,
         .originAreaMode = problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
         .extremityAreaMode = problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
         .PaysOrigineDeLInterconnexion = problemeHebdo_->PaysOrigineDeLInterconnexion,
         .PaysExtremiteDeLInterconnexion = problemeHebdo_->PaysExtremiteDeLInterconnexion,
         .hour = hour};
    CsrFlowDissociation csrFlowDissociation(builder, csrFlowDissociationData);
    csrFlowDissociation.add();
}

void CsrQuadraticProblem::setNodeBalanceConstraints(ConstraintBuilder& builder)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    const CORRESPONDANCES_DES_VARIABLES& CorrespondanceVarNativesVarOptim
      = problemeHebdo_->CorrespondanceVarNativesVarOptim[hour];

    // constraint:
    // ENS(node A) +
    // - flow (A -> 2) or (+ flow (2 -> A)) there should be only one of them, otherwise double-count
    // - spillage(node A) =
    // ENS_init(node A) + net_position_init(node A) – spillage_init(node A)
    // for all areas inside adequacy patch

    CsrAreaBalanceData csrAreaBalanceData{
      .areaMode = problemeHebdo_->adequacyPatchRuntimeData->areaMode,
      .hour = hour,
      .IndexDebutIntercoOrigine = problemeHebdo_->IndexDebutIntercoOrigine,
      .IndexSuivantIntercoOrigine = problemeHebdo_->IndexSuivantIntercoOrigine,
      .IndexDebutIntercoExtremite = problemeHebdo_->IndexDebutIntercoExtremite,
      .IndexSuivantIntercoExtremite = problemeHebdo_->IndexSuivantIntercoExtremite,
      .originAreaMode = problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
      .extremityAreaMode = problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
      .PaysOrigineDeLInterconnexion = problemeHebdo_->PaysOrigineDeLInterconnexion,
      .PaysExtremiteDeLInterconnexion = problemeHebdo_->PaysExtremiteDeLInterconnexion,
      .numberOfConstraintCsrAreaBalance = hourlyCsrProblem_.numberOfConstraintCsrAreaBalance,
      .NombreDePays = problemeHebdo_->NombreDePays};

    CsrAreaBalance csrAreaBalance(builder, csrAreaBalanceData);
    csrAreaBalance.add();
}

void CsrQuadraticProblem::setBindingConstraints(ConstraintBuilder& builder)
{
    int hour = hourlyCsrProblem_.triggeredHour;

    CsrBindingConstraintHourData csrBindingConstraintHourData = {
      .MatriceDesContraintesCouplantes = problemeHebdo_->MatriceDesContraintesCouplantes,
      .originAreaMode = problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
      .extremityAreaMode = problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
      .hour = hour,
      .numberOfConstraintCsrHourlyBinding = hourlyCsrProblem_.numberOfConstraintCsrHourlyBinding};

    CsrBindingConstraintHour csrBindingConstraintHour(
      builder, csrBindingConstraintHourData); // Special case of the binding constraints
    for (uint32_t CntCouplante = 0; CntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        csrBindingConstraintHour.add(CntCouplante);
    }
}
void CsrQuadraticProblem::buildConstraintMatrix()
{
    logs.debug() << "[CSR] constraint list:";

    std::vector<double> Pi(problemeAResoudre_.NombreDeVariables);
    std::vector<int> Colonne(problemeAResoudre_.NombreDeVariables);

    problemeAResoudre_.NombreDeContraintes = 0;
    problemeAResoudre_.NombreDeTermesDansLaMatriceDesContraintes = 0;
    auto builder_data = NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(
      problemeHebdo_, problemeAResoudre_);

    auto builder = ConstraintBuilder(builder_data);
    setConstraintsOnFlows(builder);
    setNodeBalanceConstraints(builder);
    setBindingConstraints(builder);
}

} // namespace Antares::Solver::Optimization
