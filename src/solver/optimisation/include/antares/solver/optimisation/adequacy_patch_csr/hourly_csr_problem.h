// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

// TODO[FOM] Remove this, it is only required for PROBLEME_HEBDO
// but this problem has nothing to do with PROBLEME_HEBDO
#include <optional>
#include <set>

#include <antares/logs/logs.h>
#include <antares/optimisation/linear-problem-api/ILinearProblemData.h>
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
        variableManager_(p->CorrespondanceVarNativesVarOptim,
                         p->NumeroDeVariableStockFinal,
                         p->NumeroDeVariableDeTrancheDeStock,
                         p->NombreDePasDeTempsPourUneOptimisation),
        problemeHebdo_(p)
    {
        double temp = pow(10, -adqPatchParams.curtailmentSharing.thresholdVarBoundsRelaxation);
        belowThisThresholdSetToZero = std::min(temp, 0.1);

        gemsUse_ = (problemeHebdo_->modelerData) && (problemeHebdo_->modelerData->system);

        if (gemsUse_ && !problemeHebdo_->optimEntityContainer)
        {
            throw std::runtime_error("optimEntityContainer is null but GEMS data is present");
        }

        allocateProblem();
    }

    HourlyCSRProblem(const HourlyCSRProblem&) = delete;
    HourlyCSRProblem& operator=(const HourlyCSRProblem&) = delete;

    inline void setHour(int hour)
    {
        triggeredHour = hour;
        if (gemsUse_)
        {
            fillContext_.emplace(0,
                                 0,
                                 triggeredHour + problemeHebdo_->HeureDansLAnnee,
                                 triggeredHour + problemeHebdo_->HeureDansLAnnee,
                                 problemeHebdo_->year);
        }
    }

    void run(unsigned int week, unsigned int year);

private:
    // double computeGemsContributionForArea(uint32_t area,const std::string& portFieldName)const;

    double gemsContributionForArea(
      uint32_t area,
      const std::function<std::string(const Antares::ModelerStudy::SystemModel::AreaConnection&)>&
        getFieldId) const;
    double gemsUnsupEnergyForArea(uint32_t area) const;
    double gemsSpilledForArea(uint32_t area) const;
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
    void setBoundsOnENSFromGEMS();
    void setBoundsOnENSFromLegacy();
    void setBoundsOnSpilledEnergy();
    void setBoundsOnFlows();

    // Constraints
    void setRHSvalueOnFlows();
    void setRHSnodeBalanceValue();
    void setRHSfictitiousLoadValue();
    void setRHSMaxEnsLoadValue();
    void setRHSbindingConstraintsValue();

    // CoststriggeredHour
    void setQuadraticCost();
    void setLinearCost();

    const Antares::Optimization::OptimizationOptions& solverOptions_;
    bool gemsUse_ = false;
    std::optional<Antares::LinearProblem::Api::FillContext> fillContext_;

public:
    // TODO [gp] : try to make these members private
    double belowThisThresholdSetToZero;
    std::map<int, int> numberOfConstraintCsrAreaBalance;
    std::set<int> ensVariablesInsideAdqPatch;       // place inside only ENS inside adq-patch
    std::set<int> varToBeSetToZeroIfBelowThreshold; // place inside only ENS and Spillage variable
    int triggeredHour;

    const AdqPatchParams& adqPatchParams_;
    VariableManagement::VariableManager variableManager_;

    PROBLEME_HEBDO* problemeHebdo_;
    PROBLEME_ANTARES_A_RESOUDRE problemeAResoudre_;

    std::map<int, int> numberOfConstraintCsrEns;
    std::map<int, int> numberOfConstraintCsrFlowDissociation;
    std::map<int, int> numberOfConstraintCsrFictitiousLoad;
    std::map<int, int> numberOfConstraintCsrMaxEnsLoad;
    std::map<int, int> numberOfConstraintCsrHourlyBinding; // length is number of binding constraint
                                                           // contains interco 2-2

    std::map<int, double> rhsAreaBalanceValues;

    // links between two areas inside the adq-patch domain
    std::map<int, LinkVariable> linkInsideAdqPatch;
};
