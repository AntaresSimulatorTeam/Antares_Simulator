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
    void resetProblem();
    void buildProblemVariables();
    void setVariableBounds();
    void buildProblemConstraintsLHS();
    void buildProblemConstraintsRHS();
    void setProblemCost();
    void solveProblem(uint week, int year);

public:
    void run(uint week, uint year);

    int hourInWeekTriggeredCsr;
    double belowThisThresholdSetToZero;
    PROBLEME_HEBDO* problemeHebdo;
    HourlyCSRProblem(PROBLEME_HEBDO* pProblemeHebdo) : problemeHebdo(pProblemeHebdo)
    {
        belowThisThresholdSetToZero
          = pProblemeHebdo->adqPatchParams->ThresholdCSRVarBoundsRelaxation;
    };

    inline void setHour(int hour)
    {
        hourInWeekTriggeredCsr = hour;
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
        int directVar;
        int indirectVar;
    };
    std::map<int, LinkVariable>
      linkInsideAdqPatch; // links between two areas inside the adq-patch domain
};
