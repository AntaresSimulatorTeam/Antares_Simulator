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
class OneVarFiller: public LinearProblemFiller<SolverTagType>
{
public:
    explicit OneVarFiller() = default;
    void addVariables(ILinearProblem<SolverTagType>& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addConstraints(ILinearProblem<SolverTagType>& pb, ILinearProblemData& data, FillContext& ctx) override;
    void addObjective(ILinearProblem<SolverTagType>& pb, ILinearProblemData& data, FillContext& ctx) override;

private:
    std::string added_var_name_ = "var-by-OneVarFiller";
};

template<SolverTag SolverTagType>
void OneVarFiller<SolverTagType>::addVariables(ILinearProblem<SolverTagType>& pb,
                                [[maybe_unused]] ILinearProblemData& data,
                                [[maybe_unused]] FillContext& ctx)
{
    pb.addNumVariable(0, 1, added_var_name_);
}

template<SolverTag SolverTagType>
void OneVarFiller<SolverTagType>::addConstraints([[maybe_unused]] ILinearProblem<SolverTagType>& pb,
                                  [[maybe_unused]] ILinearProblemData& data,
                                  [[maybe_unused]] FillContext& ctx)
{
}

template<SolverTag SolverTagType>
void OneVarFiller<SolverTagType>::addObjective(ILinearProblem<SolverTagType>& pb,
                                [[maybe_unused]] ILinearProblemData& data,
                                [[maybe_unused]] FillContext& ctx)
{
    auto* var = pb.lookupVariable(added_var_name_);
    pb.setObjectiveCoefficient(var, 1);
}

} // namespace Antares::Optimisation::LinearProblemApi
