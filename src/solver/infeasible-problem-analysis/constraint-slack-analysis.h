/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
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

/*!
 * That particular analysis relaxes all constraints by
 * adding slack variables for each one.
 */
class ConstraintSlackAnalysis : public UnfeasibilityAnalysis
{
public:
    ConstraintSlackAnalysis() = default;
    ~ConstraintSlackAnalysis() override = default;

    void run(operations_research::MPSolver* problem) override;
    void printReport() const override;
    std::string title() const override { return "Slack variables analysis"; }

private:
    void buildObjective(operations_research::MPSolver* problem) const;
    void addSlackVariables(operations_research::MPSolver* problem);

    std::vector<const operations_research::MPVariable*> slackVariables_;
    const std::string constraint_name_pattern = "^AreaHydroLevel::|::hourly::|::daily::|::weekly::|^FictiveLoads::|^Level::";
};

} // namespace Antares::Optimization
