#include <string>
#include <vector>

#include "report.h"

#include "ortools/linear_solver/linear_solver.h"

class Problem {
 public:
  Problem();
  void addSlackVariables(const std::string& pattern, bool all);
  void buildObjective();
  operations_research::MPSolver::ResultStatus Solve();
  void sortSlackVariables();
  InfeasibleProblemReport produceReport(double threshold) const;

 protected:
 operations_research::MPSolver mSolver;
 std::vector<operations_research::MPVariable*> mSlackVariables;
};
