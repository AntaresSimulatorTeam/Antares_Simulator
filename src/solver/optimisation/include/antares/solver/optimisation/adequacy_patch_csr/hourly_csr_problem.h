// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

// TODO[FOM] Remove this, it is only required for PROBLEME_HEBDO
// but this problem has nothing to do with PROBLEME_HEBDO
#include <optional>
#include <set>
#include <vector>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/solver/optimisation/adequacy_patch_csr/gems-part.h>
#include <antares/study/parameters/adq-patch-params.h>
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include "../variables/VariableManagerUtils.h"

struct LinkVariable
{
    LinkVariable():
        directVar(-1),
        indirectVar(-1)
    {
    }

    LinkVariable(int direct, int indirect):
        directVar(direct),
        indirectVar(indirect)
    {
    }

    inline bool check() const
    {
        if (directVar < 0)
        {
            Antares::logs.warning() << "directVar < 0 detected, this should not happen";
        }
        if (indirectVar < 0)
        {
            Antares::logs.warning() << "indirectVar < 0 detected, this should not happen";
        }

        return (directVar >= 0) && (indirectVar >= 0);
    }

    int directVar;
    int indirectVar;
};

struct PROBLEME_HEBDO;
class ConstraintBuilder;

// GEMS contribution for hybrid studies

class HourlyCSRProblem final
{
    using AdqPatchParams = AdequacyPatch::AdqPatchParams;

public:
    explicit HourlyCSRProblem(const AdqPatchParams& adqPatchParams,
                              PROBLEME_HEBDO* p,
                              const Antares::Optimization::OptimizationOptions& solverOptions):
        solverOptions_(solverOptions),
        adqPatchParams_(adqPatchParams),
        // The adq patch problem only covers a single hour. So we store variable mapping for this
        // hour
        correspondence_(csrTimeSteps_),
        variableManager_(correspondence_, unusedStockFinal_, unusedStockTranche_, csrTimeSteps_),
        problemeHebdo_(p)
    {
        // This HourlyCSRProblem's own correspondence table, entirely separate from
        // problemeHebdo_->CorrespondanceVarNativesVarOptim: it repoints UnsuppliedEnergy /
        // Spillage / DirectFlow / PositiveDirectFlow / PositiveIndirectFlow (the only
        // accessors it ever calls) at indices in its own short-lived per-hour problem, and
        // sharing the main correspondence table for that would leave every hour it touches
        // permanently pointing at this problem's numbering instead of the main weekly
        // problem's once this HourlyCSRProblem is done with it -- which is what any later
        // reader (e.g. the simulation table dump) would then resolve against. Sized for
        // every hour up front since HourlyCSRProblem is constructed once per week and reused
        // across every triggered hour.

        {
            auto& entry = correspondence_.back();
            entry.NumeroDeVariableDefaillancePositive.assign(p->NombreDePays, -1);
            entry.NumeroDeVariableDefaillanceNegative.assign(p->NombreDePays, -1);
            entry.NumeroDeVariableDuFluxDirect.assign(p->NombreDInterconnexions, -1);
            entry.NumeroDeVariableDuFluxDirectPositif.assign(p->NombreDInterconnexions, -1);
            entry.NumeroDeVariableDuFluxIndirectPositif.assign(p->NombreDInterconnexions, -1);
        }

        double temp = pow(10, -adqPatchParams.curtailmentSharing.thresholdVarBoundsRelaxation);
        belowThisThresholdSetToZero = std::min(temp, 0.1);

        allocateProblem();
        gemsPart_ = makeGemsPart(problemeHebdo_,
                                 problemeAResoudre_,
                                 variableManager_,
                                 numberOfConstraintCsrFictitiousLoad,
                                 numberOfConstraintCsrMaxEnsLoad);
    }

    HourlyCSRProblem(const HourlyCSRProblem&) = delete;
    HourlyCSRProblem& operator=(const HourlyCSRProblem&) = delete;

    inline void setHour(int hour)
    {
        triggeredHour = hour;
        gemsPart_->setHour(hour);
    }

    void run(unsigned int week, unsigned int year);

    // A ConstraintBuilder that resolves variables against this problem's own
    // correspondence_, not problemeHebdo_->CorrespondanceVarNativesVarOptim -- so
    // constraint-building agrees with the indices constructVariableENS/SpilledEnergy/Flows
    // assigned. See the constructor's comment on correspondence_.
    ConstraintBuilder makeConstraintBuilder();

private:
    void calculateCsrParameters();

    void buildProblemVariables();
    void setVariableBounds();
    void buildProblemConstraintsLHS();
    void buildProblemConstraintsRHS();
    void setProblemCost();
    void solveProblem(unsigned int week,
                      int year,
                      const Antares::Optimization::OptimizationOptions& options);
    void allocateProblem();

    // variable construction
    void constructVariableENS();
    void constructVariableSpilledEnergy();
    void constructVariableFlows();

    // variable bounds
    void setBoundsOnENS();
    void setBoundsOnSpilledEnergy();
    void setBoundsOnFlows();

    // Constraints
    void setRHSvalueOnFlows();
    void setRHSnodeBalanceValue();
    void setRHSMaxEnsLoadValue();
    void setRHSbindingConstraintsValue();
    void setRHSfictitiousLoadValue();

    // CoststriggeredHour
    void setQuadraticCost();
    void setLinearCost();

    const Antares::Optimization::OptimizationOptions& solverOptions_;
    std::unique_ptr<IGemsPart> gemsPart_;

public:
    double belowThisThresholdSetToZero;

    std::set<int> ensVariablesInsideAdqPatch;       // place inside only ENS inside adq-patch
    std::set<int> varToBeSetToZeroIfBelowThreshold; // place inside only ENS and Spillage variable
    int triggeredHour;
    // links between two areas inside the adq-patch domain

    std::map<int, LinkVariable> linkInsideAdqPatch;
    std::map<int, int> numberOfConstraintCsrAreaBalance;

    std::map<int, int> numberOfConstraintCsrEns;
    std::map<int, int> numberOfConstraintCsrFlowDissociation;
    std::map<int, int> numberOfConstraintCsrFictitiousLoad;
    std::map<int, int> numberOfConstraintCsrMaxEnsLoad;
    std::map<int, int> numberOfConstraintCsrHourlyBinding; // length is number of binding constraint
                                                           // contains interco 2-2

    std::map<int, double> rhsAreaBalanceValues;

private:
    const AdqPatchParams& adqPatchParams_;

    // See the constructor body for why this exists instead of reusing
    // problemeHebdo_->CorrespondanceVarNativesVarOptim. unusedStockFinal_ /
    // unusedStockTranche_ back VariableManager accessors (hydro layer storage) this
    // problem never calls; they only need to exist for the reference to bind to.
    std::vector<CORRESPONDANCES_DES_VARIABLES> correspondence_;
    std::vector<int> unusedStockFinal_;
    std::vector<std::vector<int>> unusedStockTranche_;
    // only one hour in the correspondance table
    static constexpr int32_t csrTimeSteps_ = 1;
    VariableManagement::VariableManager variableManager_;
    PROBLEME_HEBDO* problemeHebdo_;
    PROBLEME_ANTARES_A_RESOUDRE problemeAResoudre_;
};
