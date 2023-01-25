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
    void resetProblem();

    // Variable construction
    void constructVariableENS();
    void constructVariableSpilledEnergy();
    void constructVariableFlows();

    // Variable bounds
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
    void run(uint week, uint year);

    // TODO[FOM] Make these members private
    int triggeredHour;
    double belowThisThresholdSetToZero;
    PROBLEME_HEBDO* problemeHebdo_;
    PROBLEME_ANTARES_A_RESOUDRE problemeAResoudre_;
    explicit HourlyCSRProblem(PROBLEME_HEBDO* p) : problemeHebdo_(p)
    {
        belowThisThresholdSetToZero = p->adqPatchParams->ThresholdCSRVarBoundsRelaxation;
        allocateProblem();
    }

    ~HourlyCSRProblem()
    {
        resetProblem();
    }

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
                logs.warning() << "directVar < 0 detected, this should not happen";
            if (indirectVar < 0)
                logs.warning() << "indirectVar < 0 detected, this should not happen";

            return (directVar >= 0) && (indirectVar >= 0);
        }
        int directVar;
        int indirectVar;
    };
    std::map<int, LinkVariable>
      linkInsideAdqPatch; // links between two areas inside the adq-patch domain
};
