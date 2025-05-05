/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include <antares/optimisation/linear-problem-api/mipConstraint.h>

namespace operations_research
{
class MPConstraint; // forward declaration
}

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

class OrtoolsMipConstraint final: public LinearProblemApi::IMipConstraint
{
public:
    void setLb(double lb) override;
    void setUb(double ub) override;

    void setBounds(double lb, double ub) override;
    void setCoefficient(LinearProblemApi::IMipVariable* var, double coefficient) override;

    double getLb() const override;
    double getUb() const override;

    double getCoefficient(LinearProblemApi::IMipVariable* var) override;

    const std::string& getName() const override;

    ~OrtoolsMipConstraint() override = default;

    explicit OrtoolsMipConstraint(operations_research::MPConstraint* mpConstraint);

private:
    operations_research::MPConstraint* mpConstraint_;
};

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
