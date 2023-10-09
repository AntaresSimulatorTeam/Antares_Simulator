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
    virtual bool run() = 0;

protected:
    std::shared_ptr<operations_research::MPSolver> problem_;
};

class SlackVariablesAnalysis : public SingleAnalysis
{
    using SingleAnalysis::SingleAnalysis;
public:
    bool run() override;
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
    bool run();
    InfeasibleProblemReport produceReport();

private:
    void buildObjective() const;
    void addSlackVariables();
    operations_research::MPSolver::ResultStatus Solve() const;

    std::shared_ptr<operations_research::MPSolver> problem_;
    std::vector<const operations_research::MPVariable*> slackVariables_;
    const std::string constraint_name_pattern = "^AreaHydroLevel::|::hourly::|::daily::|::weekly::|^FictiveLoads::";                              
};
} // namespace Optimization
} // namespace Antares
