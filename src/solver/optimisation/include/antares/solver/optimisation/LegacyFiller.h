/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
 * See AUTHORS.txt
 * SPDX-License-Identifier: MPL-2.0
 * This file is part of Antares-Simulator,
 * Adequacy and Performance assessment for interconnected energy networks.
 *
 * Antares_Simulator is free software: you can redistribute it and/or modify
 * it under the terms of the Mozilla Public Licence 2.0 as published by
 * the Mozilla Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Antares_Simulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * Mozilla Public Licence 2.0 for more details.
 *
 * You should have received a copy of the Mozilla Public Licence 2.0
 * along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
 */

#pragma once

#include "antares/optimisation/linear-problem-api/linearProblemFiller.h"
#include "antares/solver/simulation/sim_structure_probleme_economique.h"

#include "opt_structure_probleme_a_resoudre.h"

namespace Antares::Optimization
{
template<Optimisation::LinearProblemApi::SolverTag SolverTagType>
class LegacyFiller: public Optimisation::LinearProblemApi::LinearProblemFiller<SolverTagType>
{
public:
    explicit LegacyFiller(const PROBLEME_HEBDO* problemeHebdo, bool namedProblems);
    void addVariables(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addConstraints(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                        Optimisation::LinearProblemApi::ILinearProblemData& data,
                        Optimisation::LinearProblemApi::FillContext& ctx) override;
    void addObjective(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb,
                      Optimisation::LinearProblemApi::ILinearProblemData& data,
                      Optimisation::LinearProblemApi::FillContext& ctx) override;

private:
    const PROBLEME_ANTARES_A_RESOUDRE* problemeAResoudre_;
    const bool useNamedProblems_;

    void CreateVariable(unsigned idxVar,
                        Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const;
    void CopyVariables(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const;
    void UpdateContraints(unsigned idxRow,
                          Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const;
    void CopyRows(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const;
    void CopyMatrix(Optimisation::LinearProblemApi::ILinearProblem<SolverTagType>& pb) const;
    std::string GetVariableName(unsigned index) const;
    std::string GetConstraintName(unsigned index) const;
};
} // namespace Antares::Optimization
