
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
#include <filesystem>
#include "antares/study/load-options.h"
#include "antares/solver/misc/options.h"
#include "antares/api/SimulationResults.h"
#include "antares/benchmarking/DurationCollector.h"
#include <antares/writer/i_writer.h>

namespace Antares::API
{

class APIInternal
{
    void prepare(std::filesystem::path study_path);
    Data::StudyLoadOptions options_;
    Settings settings_;
    std::shared_ptr<Data::Study> study_;
    void readDataForTheStudy(Data::StudyLoadOptions& options);

    Benchmarking::DurationCollector durationCollector_;
    uint errorCount_ = 0;
    //! The total muber of warnings which have been generated
    uint warningCount_ = 0;

    Antares::Data::Parameters* parameters_ = nullptr;
    std::shared_ptr<Yuni::Job::QueueService> ioQueueService;
    Solver::IResultWriter::Ptr resultWriter = nullptr;


public:
    SimulationResults run(std::filesystem::path study_path);
    void prepareWriter(const Data::Study& study,
                       Benchmarking::IDurationCollector& duration_collector);
    void writeComment(Data::Study& study);
    void startSimulation();
};

} // namespace API
