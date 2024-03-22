
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

#include "API.h"
#include "antares/solver/run_mode/economy.h"
#include "antares/solver/run_mode/adequacy.h"
#include "antares/solver/misc/options.h"
#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/benchmarking/DurationCollector.h"
#include <antares/writer/writer_factory.h>
#include <SimulationObserver.h>

namespace Antares::API
{
SimulationResults APIInternal::run(IStudyLoader* study_loader)
{
    study_ = study_loader->load();
    return execute();
}

SimulationResults APIInternal::execute() const
{
    // study_ == nullptr e.g when the -h flag is given
    if (!study_)
        return {
          .simulationPath = "",
          .antares_problems = {},
          .resultStatus = SimulationResults::ResultStatus::FAIL
        };

    study_->computePThetaInfForThermalClusters();

    // Only those two fields are used un simulation
    Settings settings;
    settings.tsGeneratorsOnly = false;
    settings.noOutput = false;

    Benchmarking::DurationCollector durationCollector;
    Benchmarking::OptimizationInfo optimizationInfo;
    auto ioQueueService = std::make_shared<Yuni::Job::QueueService>();
    ioQueueService->maximumThreadCount(1);
    ioQueueService->start();
    auto resultWriter = Solver::resultWriterFactory(
      study_->parameters.resultFormat, study_->folderOutput, ioQueueService, durationCollector);
    auto simulationObserver = std::make_shared<SimulationObserver>();
    // Run the simulation
    switch (study_->runtime->mode)
    {
    case Data::SimulationMode::Economy:
    case Data::SimulationMode::Expansion:
        Solver::runSimulationInEconomicMode(*study_,
                                            settings,
                                            durationCollector,
                                            *resultWriter,
                                            optimizationInfo,
                                            simulationObserver);
        break;
    case Data::SimulationMode::Adequacy:
        Solver::runSimulationInAdequacyMode(*study_,
                                            settings,
                                            durationCollector,
                                            *resultWriter,
                                            optimizationInfo,
                                            simulationObserver);
        break;
    default:
        break;
    }

    // Importing Time-Series if asked
    study_->importTimeseriesIntoInput();

    // Stop the display of the progression
    study_->progression.stop();
    return
    {
        .simulationPath = study_->folderOutput.c_str(),
        .antares_problems = simulationObserver->acquireLps(),
        .resultStatus = SimulationResults::ResultStatus::OK
    };
}
} // namespace Antares::API