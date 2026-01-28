// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.h"

#include <vector>

#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrAreaBalance.h"
#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrBindingConstraintHour.h"
#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrFlowDissociation.h"
#include "antares/solver/optimisation/adequacy_patch_csr/hourly_csr_problem.h"
#include "antares/solver/optimisation/constraints/constraint_builder_utils.h"
#include "antares/solver/optimisation/opt_fonctions.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

using namespace Antares::Data;

namespace Antares::Solver::Optimization
{

void CsrQuadraticProblem::setConstraintsOnFlows(ConstraintBuilder& builder)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    //!\ TODO not associated problemHebdo && probleamAressoudre
    CsrFlowDissociationData csrFlowDissociationData = {
      .numberOfConstraintCsrFlowDissociation = hourlyCsrProblem_
                                                 .numberOfConstraintCsrFlowDissociation,
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
      builder,
      csrBindingConstraintHourData); // Special case of the binding constraints
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
      problemeHebdo_,
      problemeAResoudre_);

    auto builder = ConstraintBuilder(builder_data);
    setConstraintsOnFlows(builder);
    setNodeBalanceConstraints(builder);
    setBindingConstraints(builder);
}

} // namespace Antares::Solver::Optimization
