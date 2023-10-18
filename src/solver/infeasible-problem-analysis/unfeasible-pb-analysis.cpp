#include "unfeasible-pb-analysis.h"
#include <antares/logs/logs.h>

#include <fstream>
#include <regex>
#include <algorithm>

using namespace operations_research;

namespace Antares::Optimization
{

InfeasibilityDiagnostic::InfeasibilityDiagnostic(std::shared_ptr<operations_research::MPSolver> problem)
    : problem_(problem)
{}

// ============================
// Slack variables analysis
// ============================

void ConstraintSlackDiagnostic::run()
{
    addSlackVariables();
    if (slackVariables_.empty())
    {
        logs.error() << title() << " : no constraints have been selected";
        return;
    }

    buildObjective();

    const MPSolver::ResultStatus status = Solve();
    if ((status != MPSolver::OPTIMAL) && (status != MPSolver::FEASIBLE))
    {
        logs.error() << title() << " : modified linear problem could not be solved";
        return;
    }

    hasDetectedInfeasibilityCause_ = true;
}

void ConstraintSlackDiagnostic::addSlackVariables()
{
    /* Optimization:
       We assess that less than 1 every 3 constraint will match
       the regex. If more, push_back may force the copy of memory blocks.
       This should not happen in most cases.
    */
    const unsigned int selectedConstraintsInverseRatio = 3;
    slackVariables_.reserve(problem_->NumConstraints() / selectedConstraintsInverseRatio);
    std::regex rgx(constraint_name_pattern);
    const double infinity = MPSolver::infinity();
    for (MPConstraint* constraint : problem_->constraints())
    {
        if (std::regex_search(constraint->name(), rgx))
        {
            if (constraint->lb() != -infinity)
            {
                const MPVariable* slack
                  = problem_->MakeNumVar(0, infinity, constraint->name() + "::low");
                constraint->SetCoefficient(slack, 1.);
                slackVariables_.push_back(slack);
            }

            if (constraint->ub() != infinity)
            {
                const MPVariable* slack
                  = problem_->MakeNumVar(0, infinity, constraint->name() + "::up");
                constraint->SetCoefficient(slack, -1.);
                slackVariables_.push_back(slack);
            }
        }
    }
}

void ConstraintSlackDiagnostic::buildObjective() const
{
    MPObjective* objective = problem_->MutableObjective();
    // Reset objective function
    objective->Clear();
    // Only slack variables have a non-zero cost
    for (const MPVariable* slack : slackVariables_)
    {
        objective->SetCoefficient(slack, 1.);
    }
    objective->SetMinimization();
}

MPSolver::ResultStatus ConstraintSlackDiagnostic::Solve() const
{
    return problem_->Solve();
}

void ConstraintSlackDiagnostic::printReport()
{
    InfeasibleProblemReport report(slackVariables_);
    report.prettyPrint();
}

// ============================
// Variables bounds analysis
// ============================

void VariablesBoundsConsistency::run()
{
    for (auto& var : problem_->variables())
    {
        double lowBound = var->lb();
        double upBound = var->ub();
        std::string name = var->name();
        if (lowBound > upBound)
        {
            storeIncorrectVariable(name, lowBound, upBound);
        }
    }

    if (foundIncorrectVariables())
        hasDetectedInfeasibilityCause_ = true;
}

void VariablesBoundsConsistency::storeIncorrectVariable(std::string name, double lowBound, double upBound)
{
    incorrectVars_.push_back(VariableBounds(name, lowBound, upBound));
}

bool VariablesBoundsConsistency::foundIncorrectVariables()
{
    return !incorrectVars_.empty();
}

void VariablesBoundsConsistency::printReport()
{
    for (auto& var : incorrectVars_)
    {
        logs.notice() << var.name << " : low bound = " << var.lowBound << ", up bound = " << var.upBound;
    }
}


// ===============================
// Unfeasibility analyzer 
// ===============================

// gp : this class should be renamed into UnfeasibilityAnalyzer

UnfeasiblePbAnalysis::UnfeasiblePbAnalysis(const std::string& solverName, const PROBLEME_SIMPLEXE_NOMME* ProbSpx)
{
    // gp : Here we have a dependency on PROBLEME_SIMPLEXE_NOMME and MPSolver.
    // gp : We should Convert() the PROBLEME_SIMPLEXE_NOMME into a MPSolver outside (and before) this constructor.
    // gp : And so we should have a MPSolver* passed here.
    // gp : It would be easier to test this class in isolation.
    problem_ = std::shared_ptr<MPSolver>(ProblemSimplexeNommeConverter(solverName, ProbSpx).Convert());

    analysisList_.push_back(std::make_unique<VariablesBoundsConsistency>(problem_));
    analysisList_.push_back(std::make_unique<ConstraintSlackDiagnostic>(problem_));
}

void UnfeasiblePbAnalysis::run()
{
    logs.info();
    logs.info() << "Solver: Starting unfeasibility analysis...";

    for (auto& analysis : analysisList_)
    {
        logs.info();
        logs.info() << analysis->title() << " : running...";
        analysis->run();
        if (analysis->hasDetectedInfeasibilityCause())
            return;

        logs.notice() << analysis->title() << " : nothing detected.";
    }
}

void UnfeasiblePbAnalysis::printReport()
{
    for (auto& analysis : analysisList_)
    {
        if (analysis->hasDetectedInfeasibilityCause())
        {
            logs.info() << analysis->title() << " : printing report";
            analysis->printReport();
            return;
        }
    }
    logs.notice() << "Solver: unfeasibility analysis : could not find the cause of unfeasibility.";
}
} // namespace Antares::Optimization