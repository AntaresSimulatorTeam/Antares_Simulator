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

#pragma once

#include "antares/study/study.h"

namespace Benchmarking
{
class FileContent;

class StudyInfoCollector
{
public:
    StudyInfoCollector(const Antares::Data::Study& study):
        study_(study)
    {
    }

    void toFileContent(FileContent& file_content);

private:
    // Methods
    void areasCountToFileContent(FileContent& file_content);
    void linksCountToFileContent(FileContent& file_content);
    void performedYearsCountToFileContent(FileContent& file_content);
    void enabledThermalClustersCountToFileContent(FileContent& file_content);
    void enabledBindingConstraintsCountToFileContent(FileContent& file_content);
    void unitCommitmentModeToFileContent(FileContent& file_content);
    void maxNbYearsInParallelToFileContent(FileContent& file_content);
    void solverVersionToFileContent(FileContent& file_content);

    void ORToolsUsed(FileContent& file_content);
    void ORToolsSolver(FileContent& file_content);

    // Member data
    const Antares::Data::Study& study_;
};

struct OptimizationInfo
{
    unsigned int nbVariables = 0;
    unsigned int nbConstraints = 0;
};

class SimulationInfoCollector
{
public:
    SimulationInfoCollector(const OptimizationInfo& optInfo):
        opt_info_(optInfo)
    {
    }

    void toFileContent(FileContent& file_content);

private:
    const OptimizationInfo& opt_info_;
};
} // namespace Benchmarking
