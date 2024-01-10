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
#include <fstream>
#include <algorithm>

#include "unfeasible-pb-analyzer.h"
#include "variables-bounds-consistency.h"
#include "constraint-slack-analysis.h"
#include <antares/logs/logs.h>


using namespace operations_research;

namespace Antares::Optimization
{

std::unique_ptr<UnfeasiblePbAnalyzer> makeUnfeasiblePbAnalyzer()
{
    std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList;
    analysisList.push_back(std::make_unique<VariablesBoundsConsistency>());
    analysisList.push_back(std::make_unique<ConstraintSlackAnalysis>());

    return std::make_unique<UnfeasiblePbAnalyzer>(std::move(analysisList));
}

UnfeasiblePbAnalyzer::UnfeasiblePbAnalyzer(std::vector<std::unique_ptr<UnfeasibilityAnalysis>> analysisList)
          : analysisList_(std::move(analysisList))
{}

void UnfeasiblePbAnalyzer::run(MPSolver* problem)
{
    logs.info();
    logs.info() << "Solver: Starting unfeasibility analysis...";

    for (auto& analysis : analysisList_)
    {
        logs.info();
        logs.info() << analysis->title() << " : running...";
        analysis->run(problem);
        if (analysis->hasDetectedInfeasibilityCause())
            return;

        logs.notice() << analysis->title() << " : nothing detected.";
    }
}

void UnfeasiblePbAnalyzer::printReport() const
{
    for (auto& analysis : analysisList_)
    {
        if (analysis->hasDetectedInfeasibilityCause())
        {
            logs.info() << analysis->title() << " : printing report";
            analysis->printReport();
            return;
        }
    }
    logs.notice() << "Solver: unfeasibility analysis : could not find the cause of unfeasibility.";
}
} // namespace Antares::Optimization