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

#include <string>

namespace operations_research
{
class MPVariable; // forward declaration
}

namespace Antares::Optimisation::LinearProblemMpsolverImpl
{

/**
 * Wrapper class to adapt operations_research::MPVariable to satisfy SolverVariable concept
 */
class OrtoolsVariableWrapper
{
public:
    explicit OrtoolsVariableWrapper(operations_research::MPVariable* mpVar);

    // Methods required by SolverVariable concept
    double lb() const;
    double ub() const;
    std::string& name() const;
    bool integer() const;

    void SetLB(double lb);
    void SetUB(double ub);
    void SetBounds(double lb, double ub);

    // Access to underlying OrTools variable
    operations_research::MPVariable* getMPVariable() const;

private:
    operations_research::MPVariable* mpVar_;
};

} // namespace Antares::Optimisation::LinearProblemMpsolverImpl
