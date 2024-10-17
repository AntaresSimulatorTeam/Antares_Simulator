#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"
#include "antares/solver/utils/named_problem.h"
#include "antares/solver/utils/ortools_utils.h"

#include "ortools/linear_solver/linear_solver.h"

namespace Antares::Optimization
{
class LegacyFiller: public Antares::Solver::Modeler::Api::LinearProblemFiller
{
public:
    explicit LegacyFiller(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
    void addVariables(Antares::Solver::Modeler::Api::ILinearProblem& pb,
                      Antares::Solver::Modeler::Api::LinearProblemData& data,
                      Antares::Solver::Modeler::Api::FillContext& ctx) override;
    void addConstraints(Antares::Solver::Modeler::Api::ILinearProblem& pb,
                        Antares::Solver::Modeler::Api::LinearProblemData& data,
                        Antares::Solver::Modeler::Api::FillContext& ctx) override;
    void addObjective(Antares::Solver::Modeler::Api::ILinearProblem& pb,
                      Antares::Solver::Modeler::Api::LinearProblemData& data,
                      Antares::Solver::Modeler::Api::FillContext& ctx) override;

private:
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;
    Nomenclature variableNameManager_ = Nomenclature('x');
    Nomenclature constraintNameManager_ = Nomenclature('c');

    void CreateVariable(unsigned idxVar, Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void CopyVariables(Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void UpdateContraints(unsigned idxRow, Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void CopyRows(Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void CopyMatrix(Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
};
} // namespace Antares::Optimization
