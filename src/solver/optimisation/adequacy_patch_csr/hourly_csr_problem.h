/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
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

#pragma once

// TODO[FOM] Remove this, it is only required for PROBLEME_HEBDO
// but this problem has nothing to do with PROBLEME_HEBDO
#include <set>
#include <logs/logs.h>
#include <antares/study/parameters/adq-patch-params.h>
#include "../opt_structure_probleme_a_resoudre.h"

struct PROBLEME_HEBDO;
class HourlyCSRProblem
{
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

private:
    using AdqPatchParams = Antares::Data::AdequacyPatch::AdqPatchParams;
    const AdqPatchParams& adqPatchParams_;

public:
    void run(uint week, uint year);

    // TODO[FOM] Make these members private
    int triggeredHour;
    double belowThisThresholdSetToZero;
    PROBLEME_HEBDO* problemeHebdo_;
    PROBLEME_ANTARES_A_RESOUDRE problemeAResoudre_;

    explicit HourlyCSRProblem(const AdqPatchParams& adqPatchParams,PROBLEME_HEBDO* p) :
        adqPatchParams_(adqPatchParams),
        problemeHebdo_(p)
    {
        double temp = pow(10, -adqPatchParams.curtailmentSharing.thresholdVarBoundsRelaxation);
        belowThisThresholdSetToZero = std::min(temp, 0.1);

        allocateProblem();
    }

    ~HourlyCSRProblem() = default;

    HourlyCSRProblem(const HourlyCSRProblem&) = delete;
    HourlyCSRProblem& operator=(const HourlyCSRProblem&) = delete;

    inline void setHour(int hour)
    {
        triggeredHour = hour;
    }

    std::map<int, int> numberOfConstraintCsrEns;
    std::map<int, int> numberOfConstraintCsrAreaBalance;
    std::map<int, int> numberOfConstraintCsrFlowDissociation;
    std::map<int, int> numberOfConstraintCsrHourlyBinding; // length is number of binding constraint
                                                           // contains interco 2-2

    std::map<int, double> rhsAreaBalanceValues;
    std::set<int> varToBeSetToZeroIfBelowThreshold; // place inside only ENS and Spillage variable
    std::set<int> ensVariablesInsideAdqPatch;       // place inside only ENS inside adq-patch

    struct LinkVariable
    {
        LinkVariable() : directVar(-1), indirectVar(-1)
        {
        }
        LinkVariable(int direct, int indirect) : directVar(direct), indirectVar(indirect)
        {
        }
        inline bool check() const
        {
            if (directVar < 0)
                Antares::logs.warning() << "directVar < 0 detected, this should not happen";
            if (indirectVar < 0)
                Antares::logs.warning() << "indirectVar < 0 detected, this should not happen";

            return (directVar >= 0) && (indirectVar >= 0);
        }
        int directVar;
        int indirectVar;
    };

    // links between two areas inside the adq-patch domain
    std::map<int, LinkVariable> linkInsideAdqPatch;
};
