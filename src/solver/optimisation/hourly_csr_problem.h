#pragma once

// forward declaration
namespace Antares::Solver::Variable {
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
    void run(uint week, const Antares::Solver::Variable::State& state);
    
    int hourInWeekTriggeredCsr;
    double belowThisThresholdSetToZero;
    PROBLEME_HEBDO* pWeeklyProblemBelongedTo;
    HOURLY_CSR_PROBLEM(int hourInWeek, PROBLEME_HEBDO* pProblemeHebdo)
    {
        hourInWeekTriggeredCsr = hourInWeek;
        pWeeklyProblemBelongedTo = pProblemeHebdo;
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
    std::set<int> ensSet; // place inside only ENS inside adq-patch
    std::set<int> linkSet; // place inside only links between to zones inside adq-patch
};
