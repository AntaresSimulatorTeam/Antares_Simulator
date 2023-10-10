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
    bool wasRun() { return wasRun_; }
    bool hasDetectedInfeasibilityCause() { return hasDetectedInfeasibilityCause_; }

protected:
    std::shared_ptr<operations_research::MPSolver> problem_;
    bool wasRun_ = false;
    bool hasDetectedInfeasibilityCause_ = false;
};

class SlackVariablesAnalysis : public SingleAnalysis
{
    using SingleAnalysis::SingleAnalysis;
public:
    void run() override;
    void printReport() override;

private:
    void buildObjective() const;
    void addSlackVariables();
    operations_research::MPSolver::ResultStatus Solve() const;

    std::vector<const operations_research::MPVariable*> slackVariables_;
    const std::string constraint_name_pattern = "^AreaHydroLevel::|::hourly::|::daily::|::weekly::|^FictiveLoads::";
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
