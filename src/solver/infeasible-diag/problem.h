#include <string>
#include <vector>

#include "../utils/ortools_utils.h"

#include "report.h"

class Problem {
 public:
  Problem(PROBLEME_SIMPLEXE* ProbSpx);
  void addSlackVariables(const std::string& pattern, bool all);
  void buildObjective();
  operations_research::MPSolver::ResultStatus Solve();
  void sortSlackVariables();
  InfeasibleProblemReport produceReport(double threshold) const;
  ~Problem();
 protected:
 operations_research::MPSolver* mSolver;
 std::vector<operations_research::MPVariable*> mSlackVariables;
};
