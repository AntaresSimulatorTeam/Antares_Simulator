#include <string>
#include <vector>

#include "../utils/ortools_utils.h"

#include "report.h"

class InfeasibleProblemDiag {
 public:
  InfeasibleProblemDiag(PROBLEME_SIMPLEXE* ProbSpx, const std::string& pattern);
  InfeasibleProblemReport produceReport();
  ~InfeasibleProblemDiag();
 protected:
 operations_research::MPSolver* mSolver;
 std::vector<operations_research::MPVariable*> mSlackVariables;
private:
  void buildObjective();
  operations_research::MPSolver::ResultStatus Solve();
  void addSlackVariables();
  const std::string mPattern;
};
