// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/application/application.h"

#include <antares/antares/fatal-error.h>
#include <antares/application/ScenarioBuilderOwner.h>
#include <antares/benchmarking/timer.h>
#include <antares/checks/checkLoadedInputData.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/infoCollection/StudyInfoCollector.h>
#include <antares/logs/hostinfo.h>
#include <antares/resources/resources.h>
#include <antares/study/duplicates.h>
#include <antares/study/header.h>
#include <antares/sys/policy.h>
#include <antares/writer/writer_factory.h>
#include "antares/antares/version.h"
#include "antares/checks/checksOnLPsolver.h"
#include "antares/config/config.h"
#include "antares/io/outputs/SimulationTableCsv.h"
#include "antares/signal-handling/public.h"
#include "antares/solver/misc/system-memory.h"
#include "antares/solver/misc/write-command-line.h"
#include "antares/solver/simulation/simulation-run.h"
#include "antares/solver/simulation/solver.h"
#include "antares/solver/utils/ortools_utils.h"
using namespace Antares::Check;

namespace fs = std::filesystem;

namespace
{

void logTotalTime(const std::string& msg, unsigned duration)
{
    std::chrono::milliseconds d(duration);
    auto hours = std::chrono::duration_cast<std::chrono::hours>(d);
    d -= hours;
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(d);
    d -= minutes;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(d);

    logs.info().appendFormat("%s: %02luh%02lum%02lus",
                             msg.c_str(),
                             hours.count(),
                             minutes.count(),
                             seconds.count());
}

void printSolvers()
{
    std::cout << "Available linear solvers: " << toString(availableLinearSolversList())
              << std::endl;
    std::cout << "Available quadratic solvers: " << toString(availableQuadraticSolversList())
              << std::endl;
}
} // namespace

namespace Antares::Solver
{
Application::Application()
{
    resetProcessPriority();
}

bool Application::parseCommandLine(Data::StudyLoadOptions& options)
{
    auto parser = CreateParser(pSettings, options);
    auto ret = parser->operator()(pArgc, pArgv);
    switch (ret)
    {
    case Yuni::GetOpt::ReturnCode::error:
        throw Error::CommandLineArguments(parser->errors());
    case Yuni::GetOpt::ReturnCode::help:
        pStudy = nullptr;
        return false;
    default:
        return true;
    }
}

bool Application::handleOptions(const Data::StudyLoadOptions& options)
{
    if (options.displayVersion)
    {
        PrintVersionToStdCout();
        pStudy = nullptr;
        return false;
    }

    if (options.listSolvers)
    {
        printSolvers();
        pStudy = nullptr;
        return false;
    }
    return true;
}

void Application::LogMessageStack(std::vector<std::pair<LogType, std::string>>& stack)
{
    for (const auto& [level, message]: stack)
    {
        switch (level)
        {
        case Application::LogType::Error:
            logs.error() << message;
            break;
        case Application::LogType::Warning:
            logs.warning() << message;
            break;
        }
    }
}

void Application::readDataForTheStudy(Data::StudyLoadOptions& options)
{
    logs.callback.connect(this, &Application::onLogMessage);
    auto& study = *pStudy;

    // Name of the simulation
    if (!pSettings.simulationName.empty())
    {
        study.simulationName = pSettings.simulationName;
    }

    // Force some options
    options.prepareOutput = !pSettings.noOutput;
    options.ignoreConstraints = pSettings.ignoreConstraints;

    // Load the study from a folder
    Benchmarking::Timer timer;

    std::exception_ptr loadingException;
    try
    {
        if (study.loadFromFolder(pSettings.studyFolder, options, pDurationCollector))
        {
            logs.info() << "The study is loaded.";
        }

        if (study.areas.empty())
        {
            throw Error::NoAreas();
        }

        if (!checkForDuplicates(study))
        {
            throw Error::Duplicates();
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

    // Name of the simulation (again, if the value has been overwritten)
    if (!pSettings.simulationName.empty())
    {
        study.simulationName = pSettings.simulationName;
    }

    logs.info();

    if (pSettings.noOutput)
    {
        logs.info() << "The output has been disabled.";
        logs.info();
    }

    // Errors
    if (pErrorCount > 0)
    {
        if (!pSettings.ignoreLoadingErrors)
        {
            // The loading of the study produces warnings and/or errors
            // As the option '--force' is not given, we can not continue
            LogDisplayErrorInfos(pErrorCount, pWarningCount, "The simulation must stop.");
            LogMessageStack(messagesStack);
            throw FatalError("The simulation must stop.");
        }
        else
        {
            LogDisplayErrorInfos(
              pErrorCount,
              pWarningCount,
              "As requested, the warnings can be ignored and the simulation will continue",
              false /* not an error */);
            logs.info() << "However here is the list of errors:";
            LogMessageStack(messagesStack);
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
        if (!study.checkForFilenameLimits())
        {
            throw Error::InvalidFileName();
        }

        writeComment(study);
    }

    if (!study.initializeRuntimeInfos())
    {
        throw Error::RuntimeInfoInitialization();
    }

    // Removing all callbacks, which are no longer needed
    logs.callback.clear();
    logs.info();

    // Apply transformations needed by the solver only (and not the interface for example)
    study.performTransformationsBeforeLaunchingSimulation();

    ScenarioBuilderOwner(study).callScenarioBuilder();
}

// TODO : this function is too long and has a bad name.
// TODO : we should split it into (at least) 4 functions.
// TODO : As a consequence, naming will be easier.
void Application::readStudy_makeChecks_and_printThings(Data::StudyLoadOptions& options)
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

    pStudy = std::make_unique<Antares::Data::Study>();

    pParameters = &(pStudy->parameters);
    readDataForTheStudy(options);

    postParametersChecks();
}

void Application::postParametersChecks() const
{
    // Some more checks require the existence of pParameters, hence of a study.
    // Their execution is delayed up to this point.
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

void Application::prepare(int argc, const char* argv[])
{
    pArgc = argc;
    pArgv = argv;

    // Load the local policy settings
    LocalPolicy::Open();
    LocalPolicy::CheckRootPrefix(argv[0]);

    Resources::Initialize(argc, argv);

    // Options
    Data::StudyLoadOptions options;

    // Bind pSettings / options members to command line arguments
    // Something like bind("--foo", options.foo);
    // So that option.foo will be assigned <value>
    // if the user provides --foo <value>.
    // CAUTION
    // The parser contains references to members of pSettings and options,
    // don't de-allocate these.

    if (!parseCommandLine(options)) // --help
    {
        return;
    }

    if (!handleOptions(options)) // --version, --list-solvers
    {
        return;
    }

    pDurationCollector("loading") << [this, &options]
    {
        printPIDtoDisk(pSettings);

        checkAndCorrectSettingsAndOptions(pSettings, options);

        checkStudyFolder(options.studyFolder);
        pSettings.studyFolder = fixStudyFolder(options.studyFolder);

        auto version = Data::StudyHeader::tryToFindTheVersion(pSettings.studyFolder);
        checkStudyVersion(version, pSettings.studyFolder);

        // Determine the log filename to use for this simulation
        resetLogFilename();

        readStudy_makeChecks_and_printThings(options);

        // Check solver options
        const auto& unitCommitmentMode = pParameters->unitCommitment.ucMode;
        bool milpRequired = (unitCommitmentMode == Data::UnitCommitmentMode::ucMILP);

        checkSolverOptions(options.solverOptions, milpRequired);

        // Set solver options from command line
        pStudy->parameters.optOptions.initializeWith(options.solverOptions);

        using namespace Antares::Solver::Optimization;
        // TODO
        pStudy->parameters.optOptions.exportBehavior = pStudy->parameters.include.exportStructure
                                                         ? ExportBehavior::Once
                                                         : ExportBehavior::Never;
    };

    logTotalTime("Total loading time", pDurationCollector.getTime("loading"));
}

void Application::onLogMessage(int level, const std::string& message)
{
    switch (level)
    {
    case Yuni::Logs::Verbosity::Warning::level:
        ++pWarningCount;
        messagesStack.emplace_back(LogType::Warning, message);
        break;
    case Yuni::Logs::Verbosity::Error::level:
    case Yuni::Logs::Verbosity::Fatal::level:
        ++pErrorCount;
        messagesStack.emplace_back(LogType::Error, message);
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

    // Save about-the-study files (comments, notes, etc.)
    pStudy->saveAboutTheStudy(*resultWriter);
    SystemMemoryLogger memoryReport;
    memoryReport.interval(1000 * 60 * 5); // 5 minutes
    memoryReport.start();

    Simulation::NullSimulationObserver observer;
    pDurationCollector("simulation") << [&]
    {
        pOptimizationInfo = simulationRun(*pStudy,
                                          pSettings,
                                          pDurationCollector,
                                          *resultWriter,
                                          observer);
    };

    // Importing Time-Series if asked
    pStudy->importTimeseriesIntoInput();
}

void Application::resetLogFilename() const
{
    fs::path logfile = fs::path(pSettings.studyFolder.c_str()) / "logs";

    if (!fs::exists(logfile) && !fs::create_directory(logfile))
    {
        throw FatalError(std::string("Impossible to create the log folder at ") + logfile.string()
                         + ". Aborting now.");
    }

    logfile /= "solver-"; // append the filename
    logfile += formatTime(getCurrentTime(), "%Y%m%d-%H%M%S")
               + ".log"; // complete filename with timestamp and extension

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

void Application::writeExecutionInfo()
{
    if (!pStudy)
    {
        return;
    }

    if (pErrorCount == 0 && pWarningCount > 0)
    {
        logs.warning()
          << "Simulation completed but there were some warnings during loading. Here is the list:";
        LogMessageStack(messagesStack);
    }
    if (pErrorCount > 0)
    {
        logs.error()
          << "Simulation completed but there were some errors during loading. Here is the list:";
        LogMessageStack(messagesStack);
    }

    // If no writer is available, we can't write
    if (!resultWriter)
    {
        return;
    }

    writeSimulationInfos(*pStudy, pDurationCollector, pOptimizationInfo, resultWriter.get());

    logTotalTime("Total execution time", pDurationCollector.getTime("full_exec"));
}

void writeSimulationInfos(const Data::Study& study,
                          Benchmarking::DurationCollector& durationCollector,
                          const Benchmarking::OptimizationInfo& optimizationInfo,
                          IResultWriter* resultWriter)
{
    logTotalTime("Total simulation time", durationCollector.getTime("simulation"));
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

Application::~Application()
{
    // Destroy all remaining bouns (callbacks)
    destroyBoundEvents();

    // Release all allocated data
    if (pStudy)
    {
        try
        {
            logs.info() << "[END] Quitting the solver gracefully";
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
