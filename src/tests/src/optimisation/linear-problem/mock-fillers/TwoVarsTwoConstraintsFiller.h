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

namespace Antares::Optimisation::LinearProblemApi
{

template<SolverTag SolverTagType>
class TwoVarsTwoConstraintsFiller: public LinearProblemFiller<SolverTagType>
{
public:
    explicit TwoVarsTwoConstraintsFiller() = default;
    void addVariables(ILinearProblem<SolverTagType>& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem<SolverTagType>& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem<SolverTagType>& pb, ILinearProblemData& data, FillContext& ctx) override;
};

template<SolverTag SolverTagType>
void TwoVarsTwoConstraintsFiller<SolverTagType>::addVariables(ILinearProblem<SolverTagType>& pb,
                                               [[maybe_unused]] ILinearProblemData& data,
                                               [[maybe_unused]] FillContext& ctx)
{
    pb.addNumVariable(0, 1, "var-1-by-TwoVarsTwoConstraintsFiller");
    pb.addNumVariable(0, 3, "var-2-by-TwoVarsTwoConstraintsFiller");
}

template<SolverTag SolverTagType>
void TwoVarsTwoConstraintsFiller<SolverTagType>::addConstraints(ILinearProblem<SolverTagType>& pb,
                                                 [[maybe_unused]] ILinearProblemData& data,
                                                 [[maybe_unused]] FillContext& ctx)
{
    pb.addConstraint(1, 2, "constr-1-by-TwoVarsTwoConstraintsFiller");
    pb.addConstraint(1, 3, "constr-2-by-TwoVarsTwoConstraintsFiller");
}

template<SolverTag SolverTagType>
void TwoVarsTwoConstraintsFiller<SolverTagType>::addObjective([[maybe_unused]] ILinearProblem<SolverTagType>& pb,
                                               [[maybe_unused]] ILinearProblemData& data,
                                               [[maybe_unused]] FillContext& ctx)
{
}

} // namespace Antares::Optimisation::LinearProblemApi
