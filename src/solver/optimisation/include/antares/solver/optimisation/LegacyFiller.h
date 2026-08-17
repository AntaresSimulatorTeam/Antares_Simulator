// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "opt_structure_probleme_a_resoudre.h"

namespace Antares::Optimization
{
class LegacyFiller final: public LinearProblem::Api::LinearProblemFiller
{
public:
    explicit LegacyFiller(LinearProblem::Api::ILinearProblem& linearProblem,
                          const PROBLEME_HEBDO* problemeHebdo);
    void addVariables(const LinearProblem::Api::FillContext& ctx) override;
    void addConstraints(const LinearProblem::Api::FillContext& ctx) override;
    void addObjectives(const LinearProblem::Api::FillContext& ctx) override;

private:
    const PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre_;

    LinearProblem::Api::ILinearProblem& linearProblem_;
    void CreateVariable(unsigned idxVar) const;
    void CopyVariables() const;
    void UpdateContraints(unsigned idxRow) const;
    void CopyRows() const;
    void CopyMatrix() const;
    std::string GetVariableName(unsigned index) const;
    std::string GetConstraintName(unsigned index) const;
};
} // namespace Antares::Optimization
