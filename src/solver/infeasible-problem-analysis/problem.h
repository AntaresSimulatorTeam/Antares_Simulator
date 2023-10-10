#pragma once

#include <string>
#include <vector>
#include <memory>

#include "../utils/ortools_utils.h"
#include "report.h"

namespace Antares
{
namespace Optimization
{


class SingleAnalysis
{
public:
    SingleAnalysis(std::shared_ptr<operations_research::MPSolver> problem_);
    virtual void run() = 0;
    virtual void printReport() = 0;
    virtual std::string title() = 0;
    bool hasDetectedInfeasibilityCause() { return hasDetectedInfeasibilityCause_; }

protected:
    std::shared_ptr<operations_research::MPSolver> problem_;
    bool hasDetectedInfeasibilityCause_ = false;
};


class SlackVariablesAnalysis : public SingleAnalysis
{
    using SingleAnalysis::SingleAnalysis;
public:
    void run() override;
    void printReport() override;
    std::string title() override { return "Slack variables analysis"; }

private:
    void buildObjective() const;
    void addSlackVariables();
    operations_research::MPSolver::ResultStatus Solve() const;

    std::vector<const operations_research::MPVariable*> slackVariables_;
    const std::string constraint_name_pattern = "^AreaHydroLevel::|::hourly::|::daily::|::weekly::|^FictiveLoads::";
};

struct VariableBounds
{
    std::string name;
    double lowBound;
    double upBound;
};

class VariablesBoundsAnalysis : public SingleAnalysis
{
    using SingleAnalysis::SingleAnalysis;
public:
    void run() override;
    void printReport() override;
    std::string title() override { return "Variables analysis"; }

private:
    void storeIncorrectVariable(std::string name, double lowBound, double upBound);
    bool foundIncorrectVariables();

    std::vector<VariableBounds> incorrectVars_;
};

class InfeasibleProblemAnalysis
{
public:
    InfeasibleProblemAnalysis() = delete;
    explicit InfeasibleProblemAnalysis(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx);
    void run();
    void printReport();

private:
    std::shared_ptr<operations_research::MPSolver> problem_;
    std::vector<std::unique_ptr<SingleAnalysis>> analysisList_;
};
} // namespace Optimization
} // namespace Antares
