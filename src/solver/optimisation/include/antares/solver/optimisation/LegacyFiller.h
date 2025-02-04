#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/utils/named_problem.h"

namespace Antares::Optimization
{
class LegacyFiller: public Antares::Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit LegacyFiller(const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe);
    void addVariables(Antares::Optimisation::LinearProblemApi::ILinearProblem& pb,
                      Antares::Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Antares::Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(Antares::Optimisation::LinearProblemApi::ILinearProblem& pb,
                        Antares::Optimisation::LinearProblemApi::ILinearProblemData& data,
                        Antares::Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjective(Antares::Optimisation::LinearProblemApi::ILinearProblem& pb,
                      Antares::Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Antares::Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const Antares::Optimization::PROBLEME_SIMPLEXE_NOMME* problemeSimplexe_;

    void CreateVariable(unsigned idxVar, Antares::Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void CopyVariables(Antares::Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void UpdateContraints(unsigned idxRow, Antares::Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void CopyRows(Antares::Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    void CopyMatrix(Antares::Optimisation::LinearProblemApi::ILinearProblem& pb) const;
    std::string GetVariableName(unsigned index) const;
    std::string GetConstraintName(unsigned index) const;
};
} // namespace Antares::Optimization
