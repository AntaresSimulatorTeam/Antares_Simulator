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

#include <antares/solver/optim/api/mipConstraint.h>

namespace Antares::Solver::Optim::OrtoolsImpl
{

class OrtoolsMipConstraint: virtual public Api::MipConstraint
{
public:
    void setLb(double lb) override;
    void setUb(double ub) override;

    void setBounds(double lb, double ub) override;
    void setCoefficient(Api::MipVariable* var, double coefficient) override;

    double getLb() override;
    double getUb() override;

    double getCoefficient(Api::MipVariable* var) override;

    ~OrtoolsMipConstraint() = default;

private:
    OrtoolsMipConstraint(operations_research::MPConstraint* mpConstraint);

    operations_research::MPConstraint* mpConstraint_;
};

} // namespace Antares::Solver::Optim::OrtoolsImpl
