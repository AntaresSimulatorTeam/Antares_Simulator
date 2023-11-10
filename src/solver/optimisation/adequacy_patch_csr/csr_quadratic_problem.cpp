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
#include "../constraints/new_constraint_builder_utils.h"

using namespace Antares::Data;

namespace Antares::Solver::Optimization
{

void CsrQuadraticProblem::setConstraintsOnFlows(std::vector<double>& Pi, std::vector<int>& Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    //!\ TODO not associated problemHebdo && probleamAressoudre
    auto builder = NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo_,
                                                                              problemeAResoudre_);
    CsrFlowDissociation csrFlowDissociation(builder);
    CsrFlowDissociationData data = {hourlyCsrProblem_.numberOfConstraintCsrFlowDissociation,
                                    problemeHebdo_->NombreDInterconnexions,
                                    problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
                                    problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
                                    problemeHebdo_->PaysOrigineDeLInterconnexion,
                                    problemeHebdo_->PaysExtremiteDeLInterconnexion};
    auto csrFlowDissociationData = std::make_shared<CsrFlowDissociationData>(data);
    csrFlowDissociation.add(hour, csrFlowDissociationData);
}

void CsrQuadraticProblem::setNodeBalanceConstraints(std::vector<double>& Pi,
                                                    std::vector<int>& Colonne)
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

    auto builder = NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo_,
                                                                              problemeAResoudre_);
    CsrAreaBalance csrAreaBalance(builder);

    CsrAreaBalanceData data{problemeHebdo_->adequacyPatchRuntimeData->areaMode,
                            hour,
                            problemeHebdo_->IndexDebutIntercoOrigine,
                            problemeHebdo_->IndexSuivantIntercoOrigine,
                            problemeHebdo_->IndexDebutIntercoExtremite,
                            problemeHebdo_->IndexSuivantIntercoExtremite,
                            problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
                            problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
                            problemeHebdo_->PaysOrigineDeLInterconnexion,
                            problemeHebdo_->PaysExtremiteDeLInterconnexion,
                            hourlyCsrProblem_.numberOfConstraintCsrAreaBalance,
                            problemeHebdo_->NombreDePays};
    auto csrAreaBalanceData = std::make_shared<CsrAreaBalanceData>(data);
    csrAreaBalance.add(csrAreaBalanceData);
}

void CsrQuadraticProblem::setBindingConstraints(std::vector<double>& Pi, std::vector<int>& Colonne)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    auto builder = NewGetConstraintBuilderFromProblemHebdoAndProblemAResoudre(problemeHebdo_,
                                                                              problemeAResoudre_);
    // Special case of the binding constraints
    for (uint32_t CntCouplante = 0; CntCouplante < problemeHebdo_->NombreDeContraintesCouplantes;
         CntCouplante++)
    {
        CsrBindingConstraintHourData data
          = {problemeHebdo_->MatriceDesContraintesCouplantes[CntCouplante],
             problemeHebdo_->adequacyPatchRuntimeData->originAreaMode,
             problemeHebdo_->adequacyPatchRuntimeData->extremityAreaMode,
             hour,
             hourlyCsrProblem_.numberOfConstraintCsrHourlyBinding};

        CsrBindingConstraintHour csrBindingConstraintHour(builder);
        auto csrBindingConstraintHourData = std::make_shared<CsrBindingConstraintHourData>(data);
        csrBindingConstraintHour.add(CntCouplante, csrBindingConstraintHourData);
    }
}
void CsrQuadraticProblem::buildConstraintMatrix()
{
    logs.debug() << "[CSR] constraint list:";

    std::vector<double> Pi(problemeAResoudre_.NombreDeVariables);
    std::vector<int> Colonne(problemeAResoudre_.NombreDeVariables);

    problemeAResoudre_.NombreDeContraintes = 0;
    problemeAResoudre_.NombreDeTermesDansLaMatriceDesContraintes = 0;

    setConstraintsOnFlows(Pi, Colonne);
    setNodeBalanceConstraints(Pi, Colonne);
    setBindingConstraints(Pi, Colonne);
}

} // namespace Antares::Solver::Optimization
