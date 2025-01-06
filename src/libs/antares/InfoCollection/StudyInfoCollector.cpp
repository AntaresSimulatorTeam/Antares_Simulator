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
//
// Created by marechaljas on 22/08/23.
//

#include "antares/infoCollection/StudyInfoCollector.h"

#include <antares/config/config.h>
#include "antares/benchmarking/DurationCollector.h"
#include "antares/benchmarking/file_content.h"

using namespace Antares::Data;

namespace Benchmarking
{

// Collecting data study
// ---------------------------
void StudyInfoCollector::toFileContent(FileContent& file_content)
{
    areasCountToFileContent(file_content);
    linksCountToFileContent(file_content);
    performedYearsCountToFileContent(file_content);
    enabledThermalClustersCountToFileContent(file_content);
    enabledBindingConstraintsCountToFileContent(file_content);
    unitCommitmentModeToFileContent(file_content);
    maxNbYearsInParallelToFileContent(file_content);
    solverVersionToFileContent(file_content);
    ORToolsSolver(file_content);
}

void StudyInfoCollector::areasCountToFileContent(FileContent& file_content)
{
    file_content.addItemToSection("study", "areas", study_.areas.size());
}

void StudyInfoCollector::linksCountToFileContent(FileContent& file_content)
{
    file_content.addItemToSection("study", "links", study_.areas.areaLinkCount());
}

void StudyInfoCollector::performedYearsCountToFileContent(FileContent& file_content)
{
    // Computing the number of performed years
    unsigned int nbPerformedYears = 0;
    for (uint i = 0; i < study_.parameters.nbYears; i++)
    {
        if (study_.parameters.yearsFilter[i])
        {
            nbPerformedYears++;
        }
    }

    // Adding an item related to number of performed years to the file content
    file_content.addItemToSection("study", "performed years", nbPerformedYears);
}

void StudyInfoCollector::enabledThermalClustersCountToFileContent(FileContent& file_content)
{
    // Computing the number of enabled thermal clusters
    unsigned int nbEnabledThermalClusters = 0;

    auto end = study_.areas.end();
    for (auto i = study_.areas.begin(); i != end; ++i)
    {
        const Area& area = *(i->second);
        nbEnabledThermalClusters += area.thermal.list.enabledAndNotMustRunCount();
    }

    // Adding an item related to number of enabled thermal clusters to the file content
    file_content.addItemToSection("study", "enabled thermal clusters", nbEnabledThermalClusters);
}

void StudyInfoCollector::enabledBindingConstraintsCountToFileContent(FileContent& file_content)
{
    auto activeConstraints = study_.bindingConstraints.activeConstraints();
    auto nbEnabledBC = activeConstraints.size();
    unsigned nbEnabledHourlyBC(0);
    unsigned nbEnabledDailyBC(0);
    unsigned nbEnabledWeeklyBC(0);

    for (const auto& bc: activeConstraints)
    {
        switch (bc->type())
        {
        case BindingConstraint::Type::typeHourly:
            nbEnabledHourlyBC++;
            break;
        case BindingConstraint::Type::typeDaily:
            nbEnabledDailyBC++;
            break;
        case BindingConstraint::Type::typeWeekly:
            nbEnabledWeeklyBC++;
            break;
        default:
            break;
        }
    }

    file_content.addItemToSection("study", "enabled BC", nbEnabledBC);
    file_content.addItemToSection("study", "enabled hourly BC", nbEnabledHourlyBC);
    file_content.addItemToSection("study", "enabled daily BC", nbEnabledDailyBC);
    file_content.addItemToSection("study", "enabled weekly BC", nbEnabledWeeklyBC);
}

void StudyInfoCollector::unitCommitmentModeToFileContent(FileContent& file_content)
{
    const char* unitCommitment = UnitCommitmentModeToCString(
      study_.parameters.unitCommitment.ucMode);
    file_content.addItemToSection("study", "unit commitment", unitCommitment);
}

void StudyInfoCollector::maxNbYearsInParallelToFileContent(FileContent& file_content)
{
    file_content.addItemToSection("study", "max parallel years", study_.maxNbYearsInParallel);
}

void StudyInfoCollector::solverVersionToFileContent(FileContent& file_content)
{
    // Example : 8.3.0 -> 830
    const unsigned int version = 100 * ANTARES_VERSION_HI + 10 * ANTARES_VERSION_LO
                                 + ANTARES_VERSION_BUILD;

    file_content.addItemToSection("study", "antares version", version);
}

void StudyInfoCollector::ORToolsSolver(FileContent& file_content)
{
    std::string solverName = study_.parameters.optOptions.ortoolsSolver;
    file_content.addItemToSection("study", "ortools solver", solverName);
}

// Collecting data optimization problem
// -------------------------------------
void SimulationInfoCollector::toFileContent(FileContent& file_content)
{
    file_content.addItemToSection("optimization problem", "variables", opt_info_.nbVariables);
    file_content.addItemToSection("optimization problem", "constraints", opt_info_.nbConstraints);
}

} // namespace Benchmarking
