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
#include "antares/application/application.h"

#include <yuni/datetime/timestamp.h>

#include <antares/antares/fatal-error.h>
#include <antares/benchmarking/timer.h>
#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/infoCollection/StudyInfoCollector.h>
#include <antares/logs/hostinfo.h>
#include <antares/resources/resources.h>
#include <antares/study/version.h>
#include <antares/sys/policy.h>
#include <antares/writer/writer_factory.h>
#include "antares/antares/version.h"
#include "antares/config/config.h"
#include "antares/file-tree-study-loader/FileTreeStudyLoader.h"
#include "antares/signal-handling/public.h"
#include "antares/solver/misc/system-memory.h"
#include "antares/solver/misc/write-command-line.h"
#include "antares/solver/simulation/adequacy_mode.h"
#include "antares/solver/simulation/economy_mode.h"
#include "antares/solver/simulation/simulation.h"
#include "antares/solver/simulation/timeseries-numbers.h"
#include "antares/solver/utils/ortools_utils.h"
#include "antares/study/simulation.h"

using namespace Antares::Check;

namespace fs = std::filesystem;

namespace
{
void printSolvers()
{
    std::cout << "Available solvers: " << availableOrToolsSolversString() << std::endl;
}
} // namespace

namespace Antares::Solver
{
Application::Application()
{
    resetProcessPriority();
}

void Application::parseCommandLine(Data::StudyLoadOptions& options)
{
    auto parser = CreateParser(pSettings, options);
    auto ret = parser->operator()(pArgc, pArgv);
    switch (ret)
    {
    case Yuni::GetOpt::ReturnCode::error:
        throw Error::CommandLineArguments(parser->errors());
    case Yuni::GetOpt::ReturnCode::help:
        pStudy = nullptr;
        return;
    default:
        break;
    }
}

void Application::handleOptions(const Data::StudyLoadOptions& options)
{
    if (options.displayVersion)
    {
        PrintVersionToStdCout();
        pStudy = nullptr;
        return;
    }

    if (options.listSolvers)
    {
        printSolvers();
        pStudy = nullptr;
        return;
    }
}

void Application::readDataForTheStudy(Data::StudyLoadOptions& options)
{
    auto& study = *pStudy;

    // Name of the simulation
    if (!pSettings.simulationName.empty())
    {
        study.simulationComments.name = pSettings.simulationName;
    }

    // Force some options
    options.prepareOutput = !pSettings.noOutput;
    options.ignoreConstraints = pSettings.ignoreConstraints;
    options.loadOnlyNeeded = true;

    // Load the study from a folder
    Benchmarking::Timer timer;

    std::exception_ptr loadingException;
    try
    {
        pDurationCollector("study_loading") << [&]
        {
            if (study.loadFromFolder(pSettings.studyFolder, options))
            {
                logs.info() << "The study is loaded.";
                logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
            }
        };
        // Sampled time-series Numbers
        // We will resize all matrix related to the time-series numbers
        // This operation can be done once since the number of years is constant
        // for a single simulation
        study.resizeAllTimeseriesNumbers(1 + study.runtime->rangeLimits.year[Data::rangeEnd]);
        // Now, we will prepare the time-series numbers
        if (not Antares::Solver::TimeSeriesNumbers::CheckNumberOfColumns(study.areas))
        {
            throw FatalError(
              "Inconsistent number of time-series detected. Please check your input data.");
        }

        // extraire ce code vers après le load des données
        ///!\ important, generer pour ttes les zones, années et filiaires de prod (numero de TS)
        ///(ttes les TS)
        if (not Antares::Solver::TimeSeriesNumbers::Generate(study))
        {
            throw FatalError("An unrecoverable error has occured. Can not continue.");
        }
        // ///!\ important
        if (study.parameters.useCustomScenario)
        {
            ApplyCustomScenario(study);
        }
        /// faire le check de l'hydro ici
        //**fin*/
        if (study.areas.empty())
        {
            throw Error::NoAreas();
        }

        // no output ?
        study.parameters.noOutput = pSettings.noOutput;

        if (pSettings.forceZipOutput)
        {
            pParameters->resultFormat = Antares::Data::zipArchive;
        }
    }
    catch (...)
    {
        loadingException = std::current_exception();
    }

    // For solver
    study.parameters.optOptions = options.optOptions;

    // This settings can only be enabled from the solver
    // Prepare the output for the study
    study.prepareOutput();

    // Initialize the result writer
    prepareWriter(study, pDurationCollector);

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
    if (!pSettings.simulationName.empty())
    {
        study.simulationComments.name = pSettings.simulationName;
    }

    // Removing all callbacks, which are no longer needed
    logs.callback.clear();
    logs.info();

    if (pSettings.noOutput)
    {
        logs.info() << "The output has been disabled.";
        logs.info();
    }

    // Errors
    if (pErrorCount || pWarningCount)
    {
        if (pErrorCount || !pSettings.ignoreWarningsErrors)
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
            if (!study.parameters.noOutput)
            {
                study.importLogsToOutputFolder(*resultWriter);
            }
            // empty line
            logs.info();
        }
    }

    // Checking for filename length limits
    if (!pSettings.noOutput)
    {
        if (!study.checkForFilenameLimits(true))
        {
            throw Error::InvalidFileName();
        }

        writeComment(study);
    }

    // Runtime data dedicated for the solver
    if (!study.initializeRuntimeInfos())
    {
        throw Error::RuntimeInfoInitialization();
    }

    // Apply transformations needed by the solver only (and not the interface for example)
    study.performTransformationsBeforeLaunchingSimulation();

    // alloc global vectors
    SIM_AllocationTableaux(study);
}

void Application::startSimulation(Data::StudyLoadOptions& options)
{
// Starting !
#ifdef GIT_SHA1_SHORT_STRING
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR << " (" << GIT_SHA1_SHORT_STRING
                      << ")";
#else
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR;
#endif
    WriteHostInfoIntoLogs();

    WriteCommandLineIntoLogs(pArgc, pArgv);

    logs.info() << "  :: log filename: " << logs.logfile();

    logs.callback.connect(this, &Application::onLogMessage);

    pStudy = std::make_unique<Antares::Data::Study>(true /* for the solver */);

    pParameters = &(pStudy->parameters);
    //
    readDataForTheStudy(options);

    postParametersChecks();

    pStudy->initializeProgressMeter(pSettings.tsGeneratorsOnly);
    if (pSettings.noOutput)
    {
        pSettings.displayProgression = false;
    }

    if (pSettings.displayProgression)
    {
        auto& filename = pStudy->buffer;
        filename.clear() << "about-the-study" << Yuni::IO::Separator << "map";
        pStudy->progression.saveToFile(filename, *resultWriter);
        pStudy->progression.start();
    }
    else
    {
        logs.info() << "  The progression is disabled";
    }
}

void Application::postParametersChecks() const
{ // Some more checks require the existence of pParameters, hence of a study.
    // Their execution is delayed up to this point.
    checkOrtoolsUsage(pParameters->unitCommitment.ucMode,
                      pParameters->optOptions.ortoolsUsed,
                      pParameters->optOptions.ortoolsSolver);

    checkSimplexRangeHydroPricing(pParameters->simplexOptimizationRange,
                                  pParameters->hydroPricing.hpMode);

    checkSimplexRangeUnitCommitmentMode(pParameters->simplexOptimizationRange,
                                        pParameters->unitCommitment.ucMode);

    checkSimplexRangeHydroHeuristic(pParameters->simplexOptimizationRange, pStudy->areas);

    if (pParameters->adqPatchParams.enabled)
    {
        pParameters->adqPatchParams.checkAdqPatchParams(pParameters->mode,
                                                        pStudy->areas,
                                                        pParameters->include.hurdleCosts);
    }

    bool tsGenThermal = (0
                         != (pParameters->timeSeriesToGenerate
                             & Antares::Data::TimeSeriesType::timeSeriesThermal));

    checkMinStablePower(tsGenThermal, pStudy->areas);

    checkFuelCostColumnNumber(pStudy->areas);
    checkCO2CostColumnNumber(pStudy->areas);
}

void Application::prepare(int argc, char* argv[])
{
    pArgc = argc;
    pArgv = argv;

    // Load the local policy settings
    LocalPolicy::Open();
    LocalPolicy::CheckRootPrefix(argv[0]);

    Resources::Initialize(argc, argv);

    // Options
    Data::StudyLoadOptions options;
    options.usedByTheSolver = true;

    // Bind pSettings / options members to command line arguments
    // Something like bind("--foo", options.foo);
    // So that option.foo will be assigned <value>
    // if the user provides --foo <value>.
    // CAUTION
    // The parser contains references to members of pSettings and options,
    // don't de-allocate these.

    parseCommandLine(options);

    handleOptions(options);

    // Perform some checks
    checkAndCorrectSettingsAndOptions(pSettings, options);

    pSettings.checkAndSetStudyFolder(options.studyFolder);

    checkStudyVersion(pSettings.studyFolder);

    // Determine the log filename to use for this simulation
    resetLogFilename();

    startSimulation(options);
}

void Application::onLogMessage(int level, const std::string& /*message*/)
{
    switch (level)
    {
    case Yuni::Logs::Verbosity::Warning::level:
        ++pWarningCount;
        break;
    case Yuni::Logs::Verbosity::Error::level:
    case Yuni::Logs::Verbosity::Fatal::level:
        ++pErrorCount;
        break;
    default:
        break;
    }
}

void Application::execute()
{
    // pStudy == nullptr e.g when the -h flag is given
    if (!pStudy)
    {
        return;
    }

    SystemMemoryLogger memoryReport;
    memoryReport.interval(1000 * 60 * 5); // 5 minutes
    memoryReport.start();

    pStudy->computePThetaInfForThermalClusters();

    // Run the simulation
    switch (pStudy->runtime->mode)
    {
    case Data::SimulationMode::Economy:
    case Data::SimulationMode::Expansion:
        runSimulationInEconomicMode();
        break;
    case Data::SimulationMode::Adequacy:
        runSimulationInAdequacyMode();
        break;
    default:
        break;
    }
    // TODO : make an interface class for ISimulation, check writer & queue before
    // runSimulationIn<XXX>Mode()

    // Importing Time-Series if asked
    pStudy->importTimeseriesIntoInput();

    // Stop the display of the progression
    pStudy->progression.stop();
}

void Application::runSimulationInEconomicMode()
{
    Solver::runSimulationInEconomicMode(*pStudy,
                                        pSettings,
                                        pDurationCollector,
                                        *resultWriter,
                                        pOptimizationInfo);
}

void Application::runSimulationInAdequacyMode()
{
    Solver::runSimulationInAdequacyMode(*pStudy,
                                        pSettings,
                                        pDurationCollector,
                                        *resultWriter,
                                        pOptimizationInfo);
}

static std::string timeToString()
{
    using namespace std::chrono;
    auto time = system_clock::to_time_t(system_clock::now());
    std::tm local_time = *std::localtime(&time);

    char time_buffer[256];
    std::strftime(time_buffer, sizeof(time_buffer), "%Y%m%d-%H%M%S", &local_time);

    std::string currentTime = time_buffer;

    return currentTime;
}

void Application::resetLogFilename() const
{
    fs::path logfile = fs::path(pSettings.studyFolder.c_str()) / "logs";

    if (!fs::exists(logfile) && !fs::create_directory(logfile))
    {
        throw FatalError(std::string("Impossible to create the log folder at ") + logfile.string()
                         + ". Aborting now.");
    }

    logfile /= "solver-";               // append the filename
    logfile += timeToString() + ".log"; // complete filename with timestamp and extension

    // Assigning the log filename
    logs.logfile(logfile.string());

    if (!logs.logfileIsOpened())
    {
        throw FatalError(std::string("Impossible to create the log file at ") + logfile.string());
    }
}

void Application::prepareWriter(const Antares::Data::Study& study,
                                Benchmarking::DurationCollector& duration_collector)
{
    ioQueueService = std::make_shared<Yuni::Job::QueueService>();
    ioQueueService->maximumThreadCount(1);
    ioQueueService->start();
    resultWriter = resultWriterFactory(study.parameters.resultFormat,
                                       study.folderOutput,
                                       ioQueueService,
                                       duration_collector);
}

void Application::writeComment(Data::Study& study)
{
    study.buffer.clear() << "simulation-comments.txt";

    if (!pSettings.commentFile.empty())
    {
        resultWriter->addEntryFromFile(study.buffer.c_str(), pSettings.commentFile.c_str());

        pSettings.commentFile.clear();
    }
}

static void logTotalTime(unsigned duration)
{
    std::chrono::milliseconds d(duration);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(d);
    d -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(d);
    d -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(d);

    logs.info().appendFormat("Total simulation time: %02luh%02lum%02lus",
                             hours.count(),
                             minutes.count(),
                             seconds.count());
}

void Application::writeExectutionInfo()
{
    if (!pStudy)
    {
        return;
    }

    pTotalTimer.stop();
    pDurationCollector.addDuration("total", pTotalTimer.get_duration());

    logTotalTime(pTotalTimer.get_duration());

    // If no writer is available, we can't write
    if (!resultWriter)
    {
        return;
    }

    // Info collectors : they retrieve data from study and simulation
    Benchmarking::StudyInfoCollector study_info_collector(*pStudy);
    Benchmarking::SimulationInfoCollector simulation_info_collector(pOptimizationInfo);

    // Fill file content with data retrieved by collectors
    Benchmarking::FileContent file_content;
    pDurationCollector.toFileContent(file_content);
    study_info_collector.toFileContent(file_content);
    simulation_info_collector.toFileContent(file_content);

    // Flush previous info into a record file
    const std::string exec_info_path = "execution_info.ini";
    std::string content = file_content.saveToBufferAsIni();
    resultWriter->addEntryFromBuffer(exec_info_path, content);
}

Application::~Application()
{
    // Destroy all remaining bouns (callbacks)
    destroyBoundEvents();

    // Release all allocated data
    if (pStudy)
    {
        try
        {
            logs.info() << LOG_UI_SOLVER_DONE;
        }
        catch (...)
        {
        }; // Catching log exception

        // Copy the log file if a result writer is available
        if (!pStudy->parameters.noOutput && resultWriter)
        {
            pStudy->importLogsToOutputFolder(*resultWriter);
        }

        // release all reference to the current study held by this class
        pStudy->clear();
        pStudy = nullptr;

        LocalPolicy::Close();
    }
}
} // namespace Antares::Solver
