
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
#include "antares/solver/simulation/economy_mode.h"
#include "antares/solver/simulation/adequacy_mode.h"
#include "antares/solver/misc/options.h"
#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/benchmarking/DurationCollector.h"
#include "antares/exception/LoadingError.hpp"
#include <antares/writer/writer_factory.h>
#include <SimulationObserver.h>

namespace Antares::API
{
SimulationResults APIInternal::run(IStudyLoader* study_loader)
{
    try {
        study_ = study_loader->load();
    } catch (const ::Antares::Error::StudyFolderDoesNotExist& e) {
        Antares::API::Error err{.reason = e.what()};
        return {
          .simulationPath = "",
          .antares_problems = {},
          .error = err
        };
    }
    study_ = study_loader->load();
    return execute();
}

/**
 * @brief The execute method is used to execute the simulation.
 * @return SimulationResults object which contains the results of the simulation.
 *
 * This method is a copy of Application::execute
 */
SimulationResults APIInternal::execute() const
{
    // study_ == nullptr e.g when the -h flag is given
    if (!study_)
    {
        using namespace std::string_literals;
        Antares::API::Error err{.reason = "Couldn't create study"s};
        return {.simulationPath{}, .antares_problems{}, .error = err};
    }

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
    auto simulationObserver = std::make_unique<SimulationObserver>();
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
                                            *simulationObserver);
        break;
    case Data::SimulationMode::Adequacy:
        Solver::runSimulationInAdequacyMode(*study_,
                                            settings,
                                            durationCollector,
                                            *resultWriter,
                                            optimizationInfo,
                                            *simulationObserver);
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
        .error{}
    };
}
} // namespace Antares::API