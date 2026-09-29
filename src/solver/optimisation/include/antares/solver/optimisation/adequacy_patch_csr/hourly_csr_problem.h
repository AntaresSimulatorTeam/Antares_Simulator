// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

// TODO[FOM] Remove this, it is only required for PROBLEME_HEBDO
// but this problem has nothing to do with PROBLEME_HEBDO
#include <optional>
#include <set>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
#include <antares/optimization-options/options.h>
#include <antares/solver/optimisation/adequacy_patch_csr/csr_variable_indices.h>
#include <antares/solver/optimisation/adequacy_patch_csr/gems-part.h>
#include <antares/study/parameters/adq-patch-params.h>
#include "antares/solver/modeler/ModelerData.h"
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

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
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

public:
    explicit HourlyCSRProblem(const AdqPatchParams& adqPatchParams,
                              PROBLEME_HEBDO* p,
                              const Antares::Optimization::OptimizationOptions& solverOptions):
        solverOptions_(solverOptions),
        adqPatchParams_(adqPatchParams),
        problemeHebdo_(p)
    {
        // This problem numbers its own variables (see constructVariableENS & co): keep the
        // indices in its own small table instead of
        // problemeHebdo->CorrespondanceVarNativesVarOptim. The weekly problem's correspondence
        // table is then never overwritten by the CSR machinery, so later readers (e.g. the
        // simulation table dump) still resolve the weekly variables against the weekly
        // problem's own numbering.
        constexpr int noVariable = -1; // skipped by ConstraintBuilder::AddVariable
        variableIndices_.unsuppliedEnergy.assign(p->NombreDePays, noVariable);
        variableIndices_.spillage.assign(p->NombreDePays, noVariable);
        variableIndices_.directFlow.assign(p->NombreDInterconnexions, noVariable);
        variableIndices_.positiveDirectFlow.assign(p->NombreDInterconnexions, noVariable);
        variableIndices_.positiveIndirectFlow.assign(p->NombreDInterconnexions, noVariable);

        double temp = pow(10, -adqPatchParams.curtailmentSharing.thresholdVarBoundsRelaxation);
        belowThisThresholdSetToZero = std::min(temp, 0.1);

        allocateProblem();
        gemsPart_ = makeGemsPart(problemeHebdo_,
                                 problemeAResoudre_,
                                 variableIndices_,
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

    /// \brief A ConstraintBuilder resolving variables against this problem's own variableIndices_
    ///
    /// Constraint building must agree with the indices assigned by constructVariableENS /
    /// constructVariableSpilledEnergy / constructVariableFlows, without reading nor writing
    /// problemeHebdo_->CorrespondanceVarNativesVarOptim.
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

    // This problem's own variable indices, see the constructor body. Re-written for every
    // triggered hour by constructVariableENS & co, read-only for everyone else (constraint
    // building, bounds, costs, GEMS part).
    CsrVariableIndices variableIndices_;

    PROBLEME_HEBDO* problemeHebdo_;
    PROBLEME_ANTARES_A_RESOUDRE problemeAResoudre_;
};
