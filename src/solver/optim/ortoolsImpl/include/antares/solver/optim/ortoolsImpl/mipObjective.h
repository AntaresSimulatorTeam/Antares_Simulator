/*
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

#include <ortools/linear_solver/linear_solver.h>

#include <antares/solver/optim/api/mipObjective.h>
#include <antares/solver/optim/api/mipVariable.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

class OrtoolsMipObjective final: public virtual Api::MipObjective
{
public:
    ~OrtoolsMipObjective() = default;

    void setCoefficient(Api::MipVariable* var, double coefficient) override;

    void setMaximization() override;
    void setMinimization() override;

    double getCoefficient(Api::MipVariable* var) override;

    bool getMaximization() override;
    bool getMinimization() override;

private:
    OrtoolsMipObjective(operations_research::MPObjective* objective);

    operations_research::MPObjective* objective_;
};

} // namespace Antares::Solver::Optim::OrtoolsImpl
