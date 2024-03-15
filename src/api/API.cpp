
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
#include "antares/benchmarking/timer.h"
#include "antares/exception/LoadingError.hpp"
#include "antares/signal-handling/public.h"
#include "antares/antares/fatal-error.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/checks/checkLoadedInputData.h"
#include <antares/writer/writer_factory.h>

namespace Antares::API
{
Data::StudyLoadOptions generateOptions()
{
    Data::StudyLoadOptions options;

    options.nbYears = 0;
    options.prepareOutput = false;
    options.loadOnlyNeeded = false;
    options.forceYearByYear = false;
    options.forceDerated = false;
    options.noTimeseriesImportIntoInput = false;
    //options.simplexOptimizationRange = Data::SimplexOptimization::None;
    options.mpsToExport = false;
    options.namedProblems = false;
    options.solverLogs = false;
    options.ignoreConstraints = false;
    options.forceMode = Data::SimulationMode::Expansion;
    options.enableParallel = false;
    options.forceParallel = false;
    options.maxNbYearsInParallel = 0;
    options.usedByTheSolver = false;
    options.ortoolsUsed = false;
    options.ortoolsSolver = "sirius";
    options.logMessage = "";
    options.displayVersion = false;
    options.listSolvers = false;
    options.forceExpansion = false;
    options.forceEconomy = false;
    options.forceAdequacy = false;
    options.studyFolder = "";
    options.simulationName = "";
    return options;
}

void onLogMessage(int level, const Yuni::String& /*message*/) {
    //TODO
}

void APIInternal::prepareWriter(const Antares::Data::Study& study,
                                Benchmarking::IDurationCollector& duration_collector)
{
    ioQueueService = std::make_shared<Yuni::Job::QueueService>();
    ioQueueService->maximumThreadCount(1);
    ioQueueService->start();
    resultWriter = Solver::resultWriterFactory(
      study.parameters.resultFormat, study.folderOutput, ioQueueService, duration_collector);
}

void APIInternal::writeComment(Data::Study& study)
{
    study.buffer.clear() << "simulation-comments.txt";
    
    if (!this->settings_.commentFile.empty())
    {
        this->resultWriter->addEntryFromFile(study.buffer.c_str(),
                                             this->settings_.commentFile.c_str());

        this->settings_.commentFile.clear();
        this->settings_.commentFile.shrink();
    }
}

void APIInternal::readDataForTheStudy(Data::StudyLoadOptions& options)
{
    auto& study = *study_;

    // Name of the simulation
    if (!settings_.simulationName.empty())
        study.simulationComments.name = settings_.simulationName;

    // Force some options
    options.prepareOutput = !settings_.noOutput;
    options.ignoreConstraints = settings_.ignoreConstraints;
    options.loadOnlyNeeded = true;

    // Load the study from a folder
    Benchmarking::Timer timer;

    std::exception_ptr loadingException;
    try
    {
        if (study.loadFromFolder(settings_.studyFolder, options))
        {
            logs.info() << "The study is loaded.";
            logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
        }

        timer.stop();
        durationCollector_.addDuration("study_loading", timer.get_duration());

        if (study.areas.empty())
        {
            throw Error::NoAreas();
        }

        // no output ?
        study.parameters.noOutput = settings_.noOutput;

        if (settings_.forceZipOutput)
        {
            parameters_->resultFormat = Antares::Data::zipArchive;
        }
    }
    catch (...)
    {
        loadingException = std::current_exception();
    }
    // This settings can only be enabled from the solver
    // Prepare the output for the study
    study.prepareOutput();

    // Initialize the result writer
    prepareWriter(study, durationCollector_);

    // Some checks may have failed, but we need a writer to copy the logs
    // to the output directory
    // So we wait until we have initialized the writer to rethrow
    if (loadingException)
    {
        std::rethrow_exception(loadingException);
    }

    Antares::Solver::initializeSignalHandlers(resultWriter);

    // Save about-the-study files (comments, notes, etc.)
    study.saveAboutTheStudy(*resultWriter);

    // Name of the simulation (again, if the value has been overwritten)
    if (!settings_.simulationName.empty())
        study.simulationComments.name = settings_.simulationName;

    // Removing all callbacks, which are no longer needed
    logs.callback.clear();
    logs.info();

    if (settings_.noOutput)
    {
        logs.info() << "The output has been disabled.";
        logs.info();
    }

    // Errors
    if (errorCount_ || warningCount_)
    {
        if (errorCount_ || !settings_.ignoreWarningsErrors)
        {
            // The loading of the study produces warnings and/or errors
            // As the option '--force' is not given, we can not continue
            LogDisplayErrorInfos(errorCount_, warningCount_, "The simulation must stop.");
            throw FatalError("The simulation must stop.");
        }
        else
        {
            LogDisplayErrorInfos(
              0,
              warningCount_,
              "As requested, the warnings can be ignored and the simulation will continue",
              false /* not an error */);
            // Actually importing the log file is useless here.
            // However, since we have warnings/errors, it allows to have a piece of
            // log when the unexpected happens.
            if (!study.parameters.noOutput)
                study.importLogsToOutputFolder(*resultWriter);
            // empty line
            logs.info();
        }
    }

    // Checking for filename length limits
    if (!settings_.noOutput)
    {
        if (!study.checkForFilenameLimits(true))
            throw Error::InvalidFileName();

        writeComment(study);
    }

    // Runtime data dedicated for the solver
    if (!study.initializeRuntimeInfos())
        throw Error::RuntimeInfoInitialization();

    // Apply transformations needed by the solver only (and not the interface for example)
    study.performTransformationsBeforeLaunchingSimulation();

    // alloc global vectors
    SIM_AllocationTableaux(study);
}

void APIInternal::startSimulation()
{
// Starting !
#ifdef GIT_SHA1_SHORT_STRING
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR << " (" << GIT_SHA1_SHORT_STRING
                      << ")";
#else
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR;
#endif

    logs.info() << "  :: log filename: " << logs.logfile();

    logs.callback.connect(onLogMessage);

    study_ = std::make_shared<Antares::Data::Study>(true /* for the solver */);

    parameters_ = &(study_->parameters);

    readDataForTheStudy(options_);

    study_->initializeProgressMeter(settings_.tsGeneratorsOnly);
    if (settings_.noOutput)
        settings_.displayProgression = false;

    if (settings_.displayProgression)
    {
        auto& filename = study_->buffer;
        filename.clear() << "about-the-study" << Yuni::IO::Separator << "map";
        study_->progression.saveToFile(filename, *resultWriter);
        study_->progression.start();
    }
    else
        logs.info() << "  The progression is disabled";
}

void prepare(std::filesystem::path study_path)
{

    //Handle logging



    //    // Options
    //    Data::StudyLoadOptions options;
    //    options.usedByTheSolver = true;
    //
    //    // Perform some checks
    //    checkAndCorrectSettingsAndOptions(settings_, options);
    //
    //    settings_.checkAndSetStudyFolder(options.studyFolder);
    //
    //    checkStudyVersion(settings_.studyFolder);
    //
    //    // Determine the log filename to use for this simulation
    //    resetLogFilename();
    //
    //    // Starting !
    //#ifdef GIT_SHA1_SHORT_STRING
    //    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR << " (" << GIT_SHA1_SHORT_STRING
    //                      << ")";
    //#else
    //    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR;
    //#endif
    //    WriteHostInfoIntoLogs();
    //
    //    // Write command-line options into logs
    //    // Incidentally, it also seems to contain the full path to the executable
    //    logs.info();
    //
    //    logs.info() << "  :: log filename: " << logs.logfile();
    //    // Temporary use a callback to count the number of errors and warnings
    //    logs.callback.connect(this, &Application::onLogMessage);
    //
    //    // Allocate a study
    //    study_ = std::make_shared<Antares::Data::Study>(true /* for the solver */);
    //
    //    // Setting global variables for backward compatibility
    //    parameters_ = &(study_->parameters);
    //
    //    // Loading the study
    //    readDataForTheStudy(options);
    //
    //    // Some more checks require the existence of parameters_, hence of a study.
    //    // Their execution is delayed up to this point.
    //    checkOrtoolsUsage(
    //      parameters_->unitCommitment.ucMode, parameters_->ortoolsUsed, parameters_->ortoolsSolver);
    //
    //    checkSimplexRangeHydroPricing(parameters_->simplexOptimizationRange,
    //                                  parameters_->hydroPricing.hpMode);
    //
    //    checkSimplexRangeUnitCommitmentMode(parameters_->simplexOptimizationRange,
    //                                        parameters_->unitCommitment.ucMode);
    //
    //    checkSimplexRangeHydroHeuristic(parameters_->simplexOptimizationRange, study_->areas);
    //
    //    if (parameters_->adqPatchParams.enabled)
    //        parameters_->adqPatchParams.checkAdqPatchParams(parameters_->mode,
    //                                                        study_->areas,
    //                                                        parameters_->include.hurdleCosts);
    //
    //    bool tsGenThermal
    //      = (0 != (parameters_->timeSeriesToGenerate & Antares::Data::TimeSeriesType::timeSeriesThermal));
    //
    //    checkMinStablePower(tsGenThermal, study_->areas);
    //
    //    checkFuelCostColumnNumber(study_->areas);
    //    checkCO2CostColumnNumber(study_->areas);
}

SimulationResults APIInternal::run(std::filesystem::path study_path) {
    options_ = generateOptions();
    options_.studyFolder = study_path.string();
    checkAndCorrectSettingsAndOptions(settings_, options_);
    settings_.checkAndSetStudyFolder(options_.studyFolder);
    Check::checkStudyVersion(settings_.studyFolder);
    startSimulation();
    return {.simulationPath{study_->folderOutput.c_str()}};
}

} // namespace API