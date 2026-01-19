// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 22/08/23.
//

#pragma once

#include "antares/study/study.h"

namespace Benchmarking
{
class FileContent;

class StudyInfoCollector final
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

    void ORToolsSolver(FileContent& file_content);

    // Member data
    const Antares::Data::Study& study_;
};

struct OptimizationInfo
{
    unsigned int nbVariables = 0;
    unsigned int nbConstraints = 0;
};

class SimulationInfoCollector final
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
