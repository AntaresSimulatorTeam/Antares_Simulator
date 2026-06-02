// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "opt_structure_probleme_a_resoudre.h"

namespace Antares::Optimization
{
class LegacyFiller final: public Optimisation::LinearProblemApi::LinearProblemFiller
{
public:
    explicit LegacyFiller(Optimisation::LinearProblemApi::ILinearProblem& linearProblem,
                          const PROBLEME_HEBDO* problemeHebdo,
                          bool namedProblems);
    void addVariables(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(const Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjectives(const Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre_;
    const bool useNamedProblems_;

    Optimisation::LinearProblemApi::ILinearProblem& linearProblem_;
    void CreateVariable(unsigned idxVar) const;
    void CopyVariables() const;
    void UpdateContraints(unsigned idxRow) const;
    void CopyRows() const;
    void CopyMatrix() const;
    std::string GetVariableName(unsigned index) const;
    std::string GetConstraintName(unsigned index) const;
};
} // namespace Antares::Optimization
