// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/optimisation/adequacy_patch_csr/adq_patch_curtailment_sharing.h"

#include <cmath>
#include <spx_constantes_externes.h>

#include "antares/solver/adequacy-patch/gems-csr-adapter.h"
#include "antares/solver/optimisation/adequacy_patch_csr/count_constraints_variables.h"
#include "antares/solver/optimisation/adequacy_patch_csr/csr_quadratic_problem.h"
#include "antares/solver/simulation/adequacy_patch_runtime_data.h"

#include "solve_problem.h"

using namespace Yuni;
namespace
{
// Thin CsrProblemBuilder implementation that assigns consecutive column indices
// starting at startIdx_, storing bounds directly in PROBLEME_ANTARES_A_RESOUDRE.
class CsrColumnAllocator final : public Antares::AdequacyPatch::CsrProblemBuilder
{
public:
    CsrColumnAllocator(PROBLEME_ANTARES_A_RESOUDRE& pa, int startIdx):
        pa_(pa),
        nextIdx_(startIdx)
    {
    }

    int allocateColumn(const std::string& /*name*/, double lb, double ub) override
    {
        int col = nextIdx_++;
        pa_.Xmin[col] = lb;
        pa_.Xmax[col] = ub;
        pa_.TypeDeVariable[col] = VARIABLE_NON_BORNEE;
        return col;
    }

private:
    PROBLEME_ANTARES_A_RESOUDRE& pa_;
    int nextIdx_;
};
} // anonymous namespace

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

    if (problemeHebdo->modelerData)
    {
        netPositionInit += problemeHebdo->ResultatsHoraires[Area]
                             .ValeursHorairesNetechangeModeler[hour];
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

    const auto* rtd = problemeHebdo_->adequacyPatchRuntimeData.get();
    if (rtd && rtd->useGemsFbConstraints && rtd->gemsCsrAdapter)
    {
        // NombreDeVariables now equals the legacy count — use it as the start index
        // for extra GEMS columns.
        CsrColumnAllocator allocator(problemeAResoudre_,
                                     problemeAResoudre_.NombreDeVariables);
        rtd->gemsCsrAdapter->registerExtraVariables(allocator);
        // constructVariableENS() reset NombreDeVariables to 0 and rebuilt it to
        // the legacy count; advance it past the GEMS extra columns now.
        problemeAResoudre_.NombreDeVariables += rtd->gemsCsrAdapter->countExtraVariables();
    }
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
    setBoundsOnGemsFbExtraVars();
}

void HourlyCSRProblem::buildProblemConstraintsRHS()
{
    logs.debug() << "[CSR] RHS: ";
    setRHSvalueOnFlows();
    setRHSnodeBalanceValue();
    setRHSfictitiousLoadValue();
    setRHSMaxEnsLoadValue();
    setRHSbindingConstraintsValue();
    setRHSgemsFbConstraintsValue();
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

void HourlyCSRProblem::solveProblem(uint week, int year, const OptimizationOptions& options)
{
    ADQ_PATCH_CSR(options.quadraticOptimOptions,
                  problemeAResoudre_,
                  *this,
                  adqPatchParams_,
                  week,
                  year);
}

void HourlyCSRProblem::run(uint week, uint year)
{   
    mcYear_ = static_cast<int>(year);
    calculateCsrParameters();
    buildProblemVariables();
    buildProblemConstraintsLHS();
    setVariableBounds();
    buildProblemConstraintsRHS();
    setProblemCost();
    solveProblem(week, year, solverOptions_);
}

void HourlyCSRProblem::setBoundsOnGemsFbExtraVars()
{
    const auto* rtd = problemeHebdo_->adequacyPatchRuntimeData.get();
    if (!rtd || !rtd->useGemsFbConstraints || !rtd->gemsCsrAdapter)
    {
        return;
    }
    // Extra GEMS columns sit above the legacy variable range.
    // Their bounds were written by CsrColumnAllocator during buildProblemVariables();
    // re-apply them here so they survive the AdresseOuPlacerLaValeurDesVariablesOptimisees
    // reset loop at the top of setVariableBounds().
    constexpr double kInf = 1e20;
    const int legacyEnd = problemeAResoudre_.NombreDeVariables
                          - rtd->gemsCsrAdapter->countExtraVariables();
    for (int col = legacyEnd; col < problemeAResoudre_.NombreDeVariables; ++col)
    {
        problemeAResoudre_.Xmin[col] = -kInf;
        problemeAResoudre_.Xmax[col] = kInf;
        problemeAResoudre_.TypeDeVariable[col] = VARIABLE_NON_BORNEE;
    }
}

void HourlyCSRProblem::setRHSgemsFbConstraintsValue()
{
    const auto* rtd = problemeHebdo_->adequacyPatchRuntimeData.get();
    if (!rtd || !rtd->useGemsFbConstraints || !rtd->gemsCsrAdapter)
    {
        return;
    }

    const auto rows = rtd->gemsCsrAdapter->rowsForHour(triggeredHour, mcYear_);
    for (size_t i = 0; i < rows.size() && i < gemsFbConstraintRows_.size(); ++i)
    {
        const int csrRow = gemsFbConstraintRows_[i];
        if (csrRow >= 0)
        {
            problemeAResoudre_.SecondMembre[csrRow] = rows[i].rhs;
            logs.debug() << csrRow << ": GEMS FB: RHS[" << csrRow
                         << "] = " << rows[i].rhs;
        }
    }
}