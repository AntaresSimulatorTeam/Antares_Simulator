/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include "unfeasibility-analysis.h"

namespace Antares::Optimization
{

struct VariableBounds
{
    VariableBounds(const std::string& var_name, double low_bound, double up_bound)
        : name(var_name), lowBound(low_bound), upBound(up_bound)
    {}

    std::string name;
    double lowBound;
    double upBound;
};

/*!
 * That particular analysis simply checks that all variables
 * are within their minimum and maximum bounds.
 */
class VariablesBoundsConsistency : public UnfeasibilityAnalysis
{
public:
    VariablesBoundsConsistency() = default;
    ~VariablesBoundsConsistency() override = default;

    void run(operations_research::MPSolver* problem) override;
    void printReport() const override;
    std::string title() const override { return "Variables bounds consistency check"; }

    const std::vector<VariableBounds>& incorrectVars() const { return incorrectVars_; }

private:
    void storeIncorrectVariable(std::string name, double lowBound, double upBound);
    bool foundIncorrectVariables();

    std::vector<VariableBounds> incorrectVars_;
};
}
