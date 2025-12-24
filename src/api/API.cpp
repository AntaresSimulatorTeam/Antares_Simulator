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

#include "API.h"

#include <SimulationObserver.h>

#include <antares/writer/writer_factory.h>
#include "antares/benchmarking/DurationCollector.h"
#include "antares/exception/LoadingError.hpp"
#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/solver/misc/options.h"
#include "antares/solver/simulation/simulation-run.h"

namespace Antares::API
{

static void logTotalTime(unsigned duration)
{
    std::chrono::milliseconds d(duration);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(d);
    d -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(d);
    d -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(d);

    Antares::logs.info().appendFormat("Total simulation time: %02luh%02lum%02lus",
                                      hours.count(),
                                      minutes.count(),
                                      seconds.count());
}

void writeSimulationInfos(const Data::Study& study,
                          Benchmarking::DurationCollector& durationCollector,
                          const Benchmarking::OptimizationInfo& optimizationInfo,
                          IResultWriter* resultWriter)
{
    logTotalTime(durationCollector.getTime("total"));
    Benchmarking::StudyInfoCollector study_info_collector(study);
    Benchmarking::SimulationInfoCollector simulation_info_collector(optimizationInfo);

    // Fill file content with data retrieved by collectors
    Benchmarking::FileContent file_content;
    durationCollector.toFileContent(file_content);
    study_info_collector.toFileContent(file_content);
    simulation_info_collector.toFileContent(file_content);

    // Flush previous info into a record file
    const std::string exec_info_path = "execution_info.ini";
    std::string content = file_content.saveToBufferAsIni();
    resultWriter->addEntryFromBuffer(exec_info_path, content);
}

SimulationResults APIInternal::run(
  const IStudyLoader& study_loader,
  const std::filesystem::path& output,
  const Antares::Solver::Optimization::OptimizationOptions& optOptions)
{
    try
    {
        study_ = study_loader.load();
    }
    catch (const ::Antares::Error::StudyFolderDoesNotExist& e)
    {
        Antares::API::Error err{.reason = e.what()};
        return {.antares_problems = {}, .error = err};
    }
    return execute(output, optOptions);
}

/**
 * @brief The execute method is used to execute the simulation.
 * @return SimulationResults object which contains the results of the simulation.
 *
 * This method is initialy a copy of Application::execute with some modifications hence the apparent
 * dupllication
 */
SimulationResults APIInternal::execute(
  const std::filesystem::path& output,
  const Antares::Solver::Optimization::OptimizationOptions& optOptions) const
{
    // study_ == nullptr e.g when the -h flag is given
    if (!study_)
    {
        using namespace std::string_literals;
        Antares::API::Error err{.reason = "Couldn't create study"s};
        return {.antares_problems{}, .error = err};
    }

    auto& parameters = study_->parameters;
    parameters.optOptions = optOptions;

    Settings settings;
    Benchmarking::DurationCollector durationCollector;
    Benchmarking::OptimizationInfo optimizationInfo;
    auto ioQueueService = std::make_shared<Yuni::Job::QueueService>();
    ioQueueService->maximumThreadCount(1);
    ioQueueService->start();

    study_->folderOutput = output;
    auto resultWriter = Solver::resultWriterFactory(parameters.resultFormat,
                                                    study_->folderOutput,
                                                    ioQueueService,
                                                    durationCollector);

    // In some cases (e.g tests) we don't want to write anything
    if (!output.empty())
    {
        study_->saveAboutTheStudy(*resultWriter);
    }

    SimulationObserver simulationObserver;
    durationCollector("total") << [&]
    {
        optimizationInfo = simulationRun(*study_,
                                         settings,
                                         durationCollector,
                                         *resultWriter,
                                         simulationObserver);
    };

    // Importing Time-Series if asked
    study_->importTimeseriesIntoInput();

    writeSimulationInfos(*study_, durationCollector, optimizationInfo, resultWriter.get());
    return {.antares_problems = simulationObserver.acquireLps(), .error{}};
}
} // namespace Antares::API
