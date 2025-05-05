#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/utils/named_problem.h"

namespace Antares::Optimization
{
class LegacyFiller: public Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit LegacyFiller(const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
    void addVariables(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                        Optimisation::LinearProblemApi::ILinearProblemData& data,
                        Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjective(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;

    void CreateVariable(unsigned idxVar, Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void CopyVariables(Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void UpdateContraints(unsigned idxRow,
                          Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void CopyRows(Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void CopyMatrix(Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    std::string GetVariableName(unsigned index) const;
    std::string GetConstraintName(unsigned index) const;
};
} // namespace Antares::Optimization
