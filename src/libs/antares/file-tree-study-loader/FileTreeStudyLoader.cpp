
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

#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"

namespace Antares
{
FileTreeStudyLoader::FileTreeStudyLoader(const Antares::Data::StudyLoadOptions& options_,
                                         const Settings& settings,
                                         Solver::IResultWriter::Ptr writer)
    : options_{options_}
    , settings_{settings}
    , writer_{writer}
{

}
std::unique_ptr<Antares::Data::Study> FileTreeStudyLoader::load() {
    study_ = std::make_unique<Antares::Data::Study>(true /* for the solver */);
    readDataForTheStudy();
    return std::move(study_);
}

void FileTreeStudyLoader::writeComment(Data::Study& study)
{
    study.buffer.clear() << "simulation-comments.txt";

    if (!settings_.commentFile.empty())
    {
        writer_->addEntryFromFile(study.buffer.c_str(),
                                             settings_.commentFile.c_str());

        settings_.commentFile.clear();
        settings_.commentFile.shrink();
    }
}

void FileTreeStudyLoader::readDataForTheStudy()
{
  
    // Name of the simulation
    if (!settings_.simulationName.empty())
        study_->simulationComments.name = settings_.simulationName;

    // Force some options
    options_.prepareOutput = !settings_.noOutput;
    options_.ignoreConstraints = settings_.ignoreConstraints;
    options_.loadOnlyNeeded = true;

    // Load the study from a folder
    Benchmarking::Timer timer;

    std::exception_ptr loadingException;
    try
    {
        if (study_->loadFromFolder(settings_.studyFolder, options_))
        {
            logs.info() << "The study is loaded.";
            logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
        }

        timer.stop();
        pDurationCollector.addDuration("study_loading", timer.get_duration());

        if (study_->areas.empty())
        {
            throw Error::NoAreas();
        }

        // no output ?
        study_->parameters.noOutput = settings_.noOutput;

        if (settings_.forceZipOutput)
        {
            pParameters->resultFormat = Antares::Data::zipArchive;
        }
    }
    catch (...)
    {
        loadingException = std::current_exception();
    }
    // This settings can only be enabled from the solver
    // Prepare the output for the study
    study_->prepareOutput();

    // Initialize the result writer
    prepareWriter(study_, pDurationCollector);

    // Some checks may have failed, but we need a writer to copy the logs
    // to the output directory
    // So we wait until we have initialized the writer to rethrow
    if (loadingException)
    {
        std::rethrow_exception(loadingException);
    }

    Antares::Solver::initializeSignalHandlers(resultWriter);

    // Save about-the-study files (comments, notes, etc.)
    study_->saveAboutTheStudy(*resultWriter);

    // Name of the simulation (again, if the value has been overwritten)
    if (!settings_.simulationName.empty())
        study_->simulationComments.name = settings_.simulationName;

    // Removing all callbacks, which are no longer needed
    logs.callback.clear();
    logs.info();

    if (settings_.noOutput)
    {
        logs.info() << "The output has been disabled.";
        logs.info();
    }

    // Errors
    if (pErrorCount || pWarningCount)
    {
        if (pErrorCount || !settings_.ignoreWarningsErrors)
        {
            // The loading of the study produces warnings and/or errors
            // As the option '--force' is not given, we can not continue
            LogDisplayErrorInfos(pErrorCount, pWarningCount, "The simulation must stop.");
            throw FatalError("The simulation must stop.");
        }
        else
        {
            LogDisplayErrorInfos(
              0,
              pWarningCount,
              "As requested, the warnings can be ignored and the simulation will continue",
              false /* not an error */);
            // Actually importing the log file is useless here.
            // However, since we have warnings/errors, it allows to have a piece of
            // log when the unexpected happens.
            if (!study_->parameters.noOutput)
                study_->importLogsToOutputFolder(*resultWriter);
            // empty line
            logs.info();
        }
    }

    // Checking for filename length limits
    if (!settings_.noOutput)
    {
        if (!study_->checkForFilenameLimits(true))
            throw Error::InvalidFileName();

        writeComment(study_);
    }

    // Runtime data dedicated for the solver
    if (!study_->initializeRuntimeInfos())
        throw Error::RuntimeInfoInitialization();

    // Apply transformations needed by the solver only (and not the interface for example)
    study_->performTransformationsBeforeLaunchingSimulation();

    // alloc global vectors
    SIM_AllocationTableaux(study_);
}

} // namespace Antares