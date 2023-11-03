#include <regex>
#include "constraint-slack-analysis.h"
#include <antares/logs/logs.h>
#include "report.h"

using namespace operations_research;

namespace Antares::Optimization
{

void ConstraintSlackAnalysis::run(MPSolver* problem)
{
    addSlackVariables(problem);
    if (slackVariables_.empty())
    {
        logs.error() << title() << " : no constraints have been selected";
        return;
    }

    buildObjective(problem);

    const MPSolver::ResultStatus status = problem->Solve();
    if ((status != MPSolver::OPTIMAL) && (status != MPSolver::FEASIBLE))
    {
        logs.error() << title() << " : modified linear problem could not be solved";
        return;
    }

    hasDetectedInfeasibilityCause_ = true;
}

void ConstraintSlackAnalysis::addSlackVariables(MPSolver* problem)
{
    /* Optimization:
        We assess that less than 1 every 3 constraint will match
        the regex. If more, push_back may force the copy of memory blocks.
        This should not happen in most cases.
    */
    const unsigned int selectedConstraintsInverseRatio = 3;
    slackVariables_.reserve(problem->NumConstraints() / selectedConstraintsInverseRatio);
    std::regex rgx(constraint_name_pattern);
    const double infinity = MPSolver::infinity();
    for (MPConstraint* constraint : problem->constraints())
    {
        if (std::regex_search(constraint->name(), rgx))
        {
            if (constraint->lb() != -infinity)
            {
                const MPVariable* slack
                    = problem->MakeNumVar(0, infinity, constraint->name() + "::low");
                constraint->SetCoefficient(slack, 1.);
                slackVariables_.push_back(slack);
            }

            if (constraint->ub() != infinity)
            {
                const MPVariable* slack
                    = problem->MakeNumVar(0, infinity, constraint->name() + "::up");
                constraint->SetCoefficient(slack, -1.);
                slackVariables_.push_back(slack);
            }
        }
    }
}

void ConstraintSlackAnalysis::buildObjective(MPSolver* problem) const
{
    MPObjective* objective = problem->MutableObjective();
    // Reset objective function
    objective->Clear();
    // Only slack variables have a non-zero cost
    for (const MPVariable* slack : slackVariables_)
    {
        objective->SetCoefficient(slack, 1.);
    }
    objective->SetMinimization();
}

void ConstraintSlackAnalysis::printReport() const
{
    InfeasibleProblemReport report(slackVariables_);
    report.prettyPrint();
}

} // namespace Antares::Optimization