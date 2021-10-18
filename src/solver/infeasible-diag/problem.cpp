#include "problem.h"

#include <fstream>
#include <regex>
#include <algorithm>

using namespace operations_research;

// TODO : use solver provided by the user ?
Problem::Problem() : mSolver("some or-tools problem", MPSolver::OptimizationProblemType::CLP_LINEAR_PROGRAMMING)
{
}

// TODO : remove all flag
void Problem::addSlackVariables(const std::string& pattern, bool all) {
  std::regex rgx(pattern);
  const double infinity = MPSolver::infinity();
  for (auto constraint : mSolver.constraints()) {
    if (all || std::regex_match(constraint->name(), rgx)) {
      if (constraint->lb() != -infinity) {
        MPVariable* slack;
        slack = mSolver.MakeNumVar(0, infinity, constraint->name() + "_low");
        constraint->SetCoefficient(slack, 1.);
        mSlackVariables.push_back(slack);
      }

      if (constraint->ub() != infinity) {
        MPVariable* slack;
        slack = mSolver.MakeNumVar(0, infinity, constraint->name() + "_up");
        constraint->SetCoefficient(slack, -1.);
        mSlackVariables.push_back(slack);
      }
    }
  }
}

void Problem::buildObjective() {
  MPObjective* objective = mSolver.MutableObjective();
  for (MPVariable* variable : mSolver.variables()) {
    objective->SetCoefficient(variable, 0.);
  }
  for (MPVariable* slack : mSlackVariables) {
    objective->SetCoefficient(slack, 1.);
  }
  objective->SetMinimization();
}

MPSolver::ResultStatus Problem::Solve() {
  return mSolver.Solve();
}

InfeasibleProblemReport Problem::produceReport(double threshold) const {
  InfeasibleProblemReport r;
  for (MPVariable* slack : mSlackVariables) {
    const double v = slack->solution_value();
    if (v > threshold)
      r.append(slack->name(), v);
  }
  return r;
}
