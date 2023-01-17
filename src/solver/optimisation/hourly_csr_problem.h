#pragma once

// forward declaration
namespace Antares::Solver::Variable
{
class State;
}

class HOURLY_CSR_PROBLEM
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
    HOURLY_CSR_PROBLEM(int hourInWeek, PROBLEME_HEBDO* pProblemeHebdo)
    {
        hourInWeekTriggeredCsr = hourInWeek;
        problemeHebdo = pProblemeHebdo;
        belowThisThresholdSetToZero
          = pProblemeHebdo->adqPatchParams->ThresholdCSRVarBoundsRelaxation;
    };
    std::map<int, int> numberOfConstraintCsrEns;
    std::map<int, int> numberOfConstraintCsrAreaBalance;
    std::map<int, int> numberOfConstraintCsrFlowDissociation;
    std::map<int, int> numberOfConstraintCsrHourlyBinding; // length is number of binding constraint
                                                           // contains interco 2-2

    std::map<int, double> rhsAreaBalanceValues;
    std::set<int> varToBeSetToZeroIfBelowThreshold; // place inside only ENS and Spillage variable
    std::set<int> ensSet;                           // place inside only ENS inside adq-patch
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
      linkSet; // place inside only links between to zones inside adq-patch
};
