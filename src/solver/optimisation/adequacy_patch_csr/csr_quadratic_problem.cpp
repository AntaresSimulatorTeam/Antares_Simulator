// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.h"

#include <vector>

#include "antares/solver/adequacy-patch/gems-csr-adapter.h"
#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrAreaBalance.h"
#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrBindingConstraintHour.h"
#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrFictitiousLoad.h"
#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrFlowDissociation.h"
#include "antares/solver/optimisation/adequacy_patch_csr/constraints/CsrMaxEnsLoad.h"
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

    const auto* rtdBalance = problemeHebdo_->adequacyPatchRuntimeData.get();
    const std::vector<Antares::AdequacyPatch::AreaFlowContribution>* gemsContribs = nullptr;
    if (rtdBalance && rtdBalance->useGemsFbConstraints && rtdBalance->gemsCsrAdapter)
    {
        gemsContribs = &rtdBalance->gemsCsrAdapter->areaFlowContributions();
    }

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
      .NombreDePays = problemeHebdo_->NombreDePays,
      .gemsAreaFlowContribs = gemsContribs};

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

void CsrQuadraticProblem::setFictitiousLoadConstraints(ConstraintBuilder& builder)
{
    int hour = hourlyCsrProblem_.triggeredHour;

    CsrFictitiousLoadData csrFictitiousLoadData{
      .areaMode = problemeHebdo_->adequacyPatchRuntimeData->areaMode,
      .hour = hour,
      .PaliersThermiquesDuPays = problemeHebdo_->PaliersThermiquesDuPays,
      .DefaillanceNegativeUtiliserHydro = problemeHebdo_->DefaillanceNegativeUtiliserHydro,
      .DefaillanceNegativeUtiliserConsoAbattue = problemeHebdo_
                                                   ->DefaillanceNegativeUtiliserConsoAbattue,
      .DefaillanceNegativeUtiliserPMinThermique = problemeHebdo_
                                                    ->DefaillanceNegativeUtiliserPMinThermique,
      .numberOfConstraintCsrFictitiousLoad = hourlyCsrProblem_.numberOfConstraintCsrFictitiousLoad,
      .NombreDePays = problemeHebdo_->NombreDePays};

    CsrFictitiousLoad csrFictitiousLoad(builder, csrFictitiousLoadData);
    csrFictitiousLoad.add();
}

void CsrQuadraticProblem::setMaxEnsLoadConstraints(ConstraintBuilder& builder)
{
    int hour = hourlyCsrProblem_.triggeredHour;
    CsrMaxEnsLoadData csrMaxEnsLoadData{
      .areaMode = problemeHebdo_->adequacyPatchRuntimeData->areaMode,
      .hour = hour,
      .NombreDePays = problemeHebdo_->NombreDePays,
      .numberOfConstraintCsrMaxEnsLoad = hourlyCsrProblem_.numberOfConstraintCsrMaxEnsLoad};

    CsrMaxEnsLoad csrMaxEnsLoad(builder, csrMaxEnsLoadData);
    csrMaxEnsLoad.add();
}

void CsrQuadraticProblem::setFlowBasedConstraints(ConstraintBuilder& builder)
{
    const auto* rtd = problemeHebdo_->adequacyPatchRuntimeData.get();
    if (!rtd || !rtd->useGemsFbConstraints || !rtd->gemsCsrAdapter)
    {
        return;
    }

    const int hour = hourlyCsrProblem_.triggeredHour;
    const int mcYear = hourlyCsrProblem_.mcYear_;

    auto& rowIndices = hourlyCsrProblem_.gemsFbConstraintRows_;
    rowIndices.clear();

    const auto rows = rtd->gemsCsrAdapter->rowsForHour(hour, mcYear);
    for (const auto& row : rows)
    {
        builder.updateHourWithinWeek(static_cast<unsigned>(hour));
        for (const auto& term : row.terms)
        {
            builder.rawTerm(term.column, term.coefficient);
        }

        if (builder.NumberOfVariables() > 0)
        {
            const int csrRow = builder.data.nombreDeContraintes;
            rowIndices.push_back(csrRow);
            builder.data.NomDesContraintes[csrRow] = "gems_" + row.constraintId;
            switch (row.sense)
            {
            case Antares::AdequacyPatch::CsrRowSense::LE:
                builder.lessThan();
                break;
            case Antares::AdequacyPatch::CsrRowSense::GE:
                builder.greaterThan();
                break;
            case Antares::AdequacyPatch::CsrRowSense::EQ:
                builder.equalTo();
                break;
            }
            builder.build();
        }
        else
        {
            rowIndices.push_back(-1);
        }
    }
}

void CsrQuadraticProblem::setGemsConservationConstraint(ConstraintBuilder& builder)
{
    const auto* rtd = problemeHebdo_->adequacyPatchRuntimeData.get();
    if (!rtd || !rtd->useGemsFbConstraints || !rtd->gemsCsrAdapter)
    {
        return;
    }

    const auto& contribs = rtd->gemsCsrAdapter->areaFlowContributions();
    if (contribs.empty())
    {
        return;
    }

    builder.updateHourWithinWeek(static_cast<unsigned>(hourlyCsrProblem_.triggeredHour));
    for (const auto& contrib : contribs)
    {
        // Conservation: Σ ccr_exchange_A = 0 (each raw exchange variable with coeff +1)
        builder.rawTerm(contrib.csrColumn, 1.0);
    }

    const int csrRow = builder.data.nombreDeContraintes;
    builder.data.NomDesContraintes[csrRow] = "gems_conservation_ccr_exchange";
    builder.equalTo();
    builder.build();
    logs.debug() << "[CSR] GEMS conservation constraint at row " << csrRow;
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
    setFictitiousLoadConstraints(builder);
    setMaxEnsLoadConstraints(builder);
    setBindingConstraints(builder);
    setFlowBasedConstraints(builder);
    setGemsConservationConstraint(builder);
}

} // namespace Antares::Solver::Optimization
