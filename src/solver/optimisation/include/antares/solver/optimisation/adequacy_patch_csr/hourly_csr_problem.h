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

#pragma once

// TODO[FOM] Remove this, it is only required for PROBLEME_HEBDO
// but this problem has nothing to do with PROBLEME_HEBDO
#include <set>

#include <antares/logs/logs.h>
#include <antares/study/parameters/adq-patch-params.h>
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

class HourlyCSRProblem
{
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;

public:
    explicit HourlyCSRProblem(const AdqPatchParams& adqPatchParams, PROBLEME_HEBDO* p):
        adqPatchParams_(adqPatchParams),
        variableManager_(p->CorrespondanceVarNativesVarOptim,
                         p->NumeroDeVariableStockFinal,
                         p->NumeroDeVariableDeTrancheDeStock,
                         p->NombreDePasDeTempsPourUneOptimisation),
        problemeHebdo_(p)
    {
        double temp = pow(10, -adqPatchParams.curtailmentSharing.thresholdVarBoundsRelaxation);
        belowThisThresholdSetToZero = std::min(temp, 0.1);

        allocateProblem();
    }

    HourlyCSRProblem(const HourlyCSRProblem&) = delete;
    HourlyCSRProblem& operator=(const HourlyCSRProblem&) = delete;

    inline void setHour(int hour)
    {
        triggeredHour = hour;
    }

    void run(uint week, uint year);

private:
    void calculateCsrParameters();

    void buildProblemVariables();
    void setVariableBounds();
    void buildProblemConstraintsLHS();
    void buildProblemConstraintsRHS();
    void setProblemCost();
    void solveProblem(uint week, int year);
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
    void setRHSbindingConstraintsValue();

    // Costs
    void setQuadraticCost();
    void setLinearCost();

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
    std::map<int, int> numberOfConstraintCsrHourlyBinding; // length is number of binding constraint
                                                           // contains interco 2-2

    std::map<int, double> rhsAreaBalanceValues;

    // links between two areas inside the adq-patch domain
    std::map<int, LinkVariable> linkInsideAdqPatch;
};
