#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"
#include "antares/solver/utils/named_problem.h"
#include "antares/solver/utils/ortools_utils.h"

#include "ortools/linear_solver/linear_solver.h"

using namespace Antares::Solver::Modeler::Api;

namespace Antares::Optimization
{
class LegacyOrtoolsFiller: public LinearProblemFiller
{
public:
    explicit LegacyOrtoolsFiller(
      const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
    void addVariables(ILinearProblem& pb, LinearProblemData& data) override;
    void addConstraints(ILinearProblem& pb, LinearProblemData& data) override;
    void addObjective(ILinearProblem& pb, LinearProblemData& data) override;

private:
    operations_research::MPSolver* mpSolver_ = nullptr;
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;
    Nomenclature variableNameManager_ = Nomenclature('x');
    Nomenclature constraintNameManager_ = Nomenclature('c');

    void CreateVariable(unsigned idxVar, MPSolver* solver, MPObjective* const objective) const;
    void CopyVariables(MPSolver* solver) const;
    void UpdateContraints(unsigned idxRow, MPSolver* solver) const;
    void CopyRows(MPSolver* solver) const;
    void CopyMatrix(const MPSolver* solver) const;
};
} // namespace Antares::Optimization
