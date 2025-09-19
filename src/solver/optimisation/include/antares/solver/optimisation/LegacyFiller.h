#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "opt_structure_probleme_a_resoudre.h"

namespace Antares::Optimization
{
class LegacyFiller final: public Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit LegacyFiller(const PROBLEME_HEBDO* problemeHebdo, bool namedProblems);
    void addVariables(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem& pb,
                        const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjective(Optimisation::LinearProblemApi::ILinearProblem& pb,
                      const Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre_;
    const bool useNamedProblems_;

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
