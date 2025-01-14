#pragma once

#include "antares/solver/modeler/api/linearProblemFiller.h"
#include "antares/solver/utils/named_problem.h"

namespace Antares::Optimization
{
class LegacyFiller: public Antares::Solver::Modeler::Api::LinearProblemFiller
{
public:
    explicit LegacyFiller(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
    void addVariables(Antares::Solver::Modeler::Api::ILinearProblem& pb,
                      Antares::Solver::Modeler::Api::ILinearProblemData& data,
                      Antares::Solver::Modeler::Api::FillContext& ctx) override;
    void addConstraints(Antares::Solver::Modeler::Api::ILinearProblem& pb,
                        Antares::Solver::Modeler::Api::ILinearProblemData& data,
                        Antares::Solver::Modeler::Api::FillContext& ctx) override;
    void addObjective(Antares::Solver::Modeler::Api::ILinearProblem& pb,
                      Antares::Solver::Modeler::Api::ILinearProblemData& data,
                      Antares::Solver::Modeler::Api::FillContext& ctx) override;

private:
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;

    void CreateVariable(unsigned idxVar, Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void CopyVariables(Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void UpdateContraints(unsigned idxRow, Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void CopyRows(Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    void CopyMatrix(Antares::Solver::Modeler::Api::ILinearProblem& pb) const;
    std::string GetVariableName(unsigned index) const;
    std::string GetConstraintName(unsigned index) const;
};
} // namespace Antares::Optimization
