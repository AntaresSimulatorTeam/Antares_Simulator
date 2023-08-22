#include "application.h"

#include <antares/sys/policy.h>
#include <antares/resources/resources.h>
#include <antares/hostinfo.h>
#include <antares/fatal-error.h>
#include <antares/benchmarking/timer.h>

#include <antares/exception/InitializationError.hpp>
#include <antares/exception/LoadingError.hpp>
#include <antares/checks/checkLoadedInputData.h>
#include <antares/version.h>

#include "signal-handling/public.h"

#include "misc/system-memory.h"
#include "misc/write-command-line.h"

#include "utils/ortools_utils.h"
#include "../config.h"
#include <antares/benchmarking/StudyInfoCollector.h>

#include <yuni/io/io.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/core/process/rename.h>

#include <algorithm>

using namespace Antares::Check;

namespace
{
void printSolvers()
{
    std::cout << "Available solvers :" << std::endl;
    for (const auto& solver : getAvailableOrtoolsSolverName())
    {
        std::cout << solver << std::endl;
    }
}
} // namespace

namespace Antares::Solver
{
Application::Application()
{
    resetProcessPriority();
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
    auto parser = CreateParser(pSettings, options);
    // Parse the command line arguments

    switch (auto ret = parser->operator()(argc, argv); ret)
    {
        using namespace Yuni::GetOpt;
    case ReturnCode::error:
        throw Error::CommandLineArguments(parser->errors());
        break;
    case ReturnCode::help:
        // End the program
        pStudy = nullptr;
        return;
    default:
        break;
    }

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

    // Perform some checks
    checkAndCorrectSettingsAndOptions(pSettings, options);

    pSettings.checkAndSetStudyFolder(options.studyFolder);

    checkStudyVersion(pSettings.studyFolder);

    // Determine the log filename to use for this simulation
    resetLogFilename();

    // Starting !
#ifdef GIT_SHA1_SHORT_STRING
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR << " (" << GIT_SHA1_SHORT_STRING
                      << ")";
#else
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_STR;
#endif
    WriteHostInfoIntoLogs();

    // Write command-line options into logs
    // Incidentally, it also seems to contain the full path to the executable
    logs.info();
    WriteCommandLineIntoLogs(argc, argv);

    logs.info() << "  :: log filename: " << logs.logfile();
    // Temporary use a callback to count the number of errors and warnings
    logs.callback.connect(this, &Application::onLogMessage);

    // Allocate a study
    pStudy = std::make_shared<Antares::Data::Study>(true /* for the solver */);

    // Setting global variables for backward compatibility
    pParameters = &(pStudy->parameters);

    // Loading the study
    readDataForTheStudy(options);

    // Some more checks require the existence of pParameters, hence of a study.
    // Their execution is delayed up to this point.
    checkSimplexRangeHydroPricing(pParameters->simplexOptimizationRange,
                                  pParameters->hydroPricing.hpMode);

    checkSimplexRangeUnitCommitmentMode(pParameters->simplexOptimizationRange,
                                        pParameters->unitCommitment.ucMode);

    checkSimplexRangeHydroHeuristic(pParameters->simplexOptimizationRange, pStudy->areas);

    if (pParameters->adqPatchParams.enabled)
        pParameters->adqPatchParams.checkAdqPatchParams(pParameters->mode,
                                                        pStudy->areas,
                                                        pParameters->include.hurdleCosts);

    bool tsGenThermal
      = (0 != (pParameters->timeSeriesToGenerate & Antares::Data::TimeSeries::timeSeriesThermal));

    checkMinStablePower(tsGenThermal, pStudy->areas);

    checkFuelCostColumnNumber(pStudy->areas);
    checkCO2CostColumnNumber(pStudy->areas);

    // Start the progress meter
    pStudy->initializeProgressMeter(pSettings.tsGeneratorsOnly);
    if (pSettings.noOutput)
        pSettings.displayProgression = false;

    if (pSettings.displayProgression)
    {
        auto& filename = pStudy->buffer;
        filename.clear() << "about-the-study" << Yuni::IO::Separator << "map";
        pStudy->progression.saveToFile(filename, pStudy->resultWriter);
        pStudy->progression.start();
    }
    else
        logs.info() << "  The progression is disabled";
}

void Application::initializeRandomNumberGenerators()
{
    logs.info() << "Initializing random number generators...";
    const auto& parameters = pStudy->parameters;
    auto& runtime = *pStudy->runtime;

    for (uint i = 0; i != Data::seedMax; ++i)
    {
#ifndef NDEBUG
        logs.debug() << "  random number generator: " << Data::SeedToCString((Data::SeedIndex)i)
                     << ", seed: " << parameters.seed[i];
#endif
        runtime.random[i].reset(parameters.seed[i]);
    }
}

void Application::onLogMessage(int level, const Yuni::String& /*message*/)
{
    switch (level)
    {
    case Yuni::Logs::Verbosity::Warning::level:
        ++pWarningCount;
        break;
    case Yuni::Logs::Verbosity::Error::level:
        ++pErrorCount;
        break;
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
        return;

    processCaption(Yuni::String() << "antares: running \"" << pStudy->header.caption << "\"");

    SystemMemoryLogger memoryReport;
    memoryReport.interval(1000 * 60 * 5); // 5 minutes
    memoryReport.start();

    pStudy->computePThetaInfForThermalClusters();

    // Run the simulation
    switch (pStudy->runtime->mode)
    {
    case Data::stdmEconomy:
        runSimulationInEconomicMode();
        break;
    case Data::stdmAdequacy:
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

void Application::resetLogFilename() const
{
    // Assigning the log file
    Yuni::String logfile;
    logfile << pSettings.studyFolder << Yuni::IO::Separator << "logs";

    // Making sure that the folder
    if (!Yuni::IO::Directory::Create(logfile))
    {
        throw FatalError(std::string("Impossible to create the log folder at ") + logfile.c_str() + ". Aborting now.");
    }

    // Date/time
    logfile << Yuni::IO::Separator << "solver-";
    Yuni::DateTime::TimestampToString(logfile, "%Y%m%d-%H%M%S", 0, false);
    logfile << ".log";

    // Assigning the log filename
    logs.logfile(logfile);

    if (!logs.logfileIsOpened())
    {
        throw FatalError(std::string("Impossible to create the log file at ") + logfile.c_str());
    }
}

void Application::processCaption(const Yuni::String& caption)
{
    pArgv = Yuni::Process::Rename(pArgc, pArgv, caption);
}

void Application::readDataForTheStudy(Data::StudyLoadOptions& options)
{
    processCaption(Yuni::String() << "antares: loading \"" << pSettings.studyFolder << "\"");
    auto& study = *pStudy;

    // Name of the simulation
    if (!pSettings.simulationName.empty())
        study.simulationComments.name = pSettings.simulationName;

    // Force some options
    options.prepareOutput = !pSettings.noOutput;
    options.ignoreConstraints = pSettings.ignoreConstraints;
    options.loadOnlyNeeded = true;

    // Load the study from a folder
    Benchmarking::Timer timer;

    if (study.loadFromFolder(pSettings.studyFolder, options) && !study.gotFatalError)
    {
        logs.info() << "The study is loaded.";
        logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
    }

    timer.stop();
    pDurationCollector.addDuration("study_loading", timer.get_duration());

    if (study.gotFatalError)
        throw Error::ReadingStudy();

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

    // This settings can only be enabled from the solver
    // Prepare the output for the study
    study.prepareOutput();

    // Initialize the result writer
    study.prepareWriter(&pDurationCollector);
    Antares::Solver::initializeSignalHandlers(study.resultWriter);

    // Save about-the-study files (comments, notes, etc.)
    study.saveAboutTheStudy();

    // Name of the simulation (again, if the value has been overwritten)
    if (!pSettings.simulationName.empty())
        study.simulationComments.name = pSettings.simulationName;

    // Removing all callbacks, which are no longer needed
    logs.callback.clear();
    logs.info();

    if (pSettings.noOutput)
    {
        logs.info() << "The output has been disabled.";
        logs.info();
    }

    // Errors
    if (pErrorCount || pWarningCount || study.gotFatalError)
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
                study.importLogsToOutputFolder();
            // empty line
            logs.info();
        }
    }

    // Checking for filename length limits
    if (!pSettings.noOutput)
    {
        if (!study.checkForFilenameLimits(true))
            throw Error::InvalidFileName();

        // comments
        {
            study.buffer.clear() << "simulation-comments.txt";

            if (!pSettings.commentFile.empty())
            {
                auto writer = pStudy->resultWriter;
                if (writer)
                    writer->addEntryFromFile(study.buffer.c_str(), pSettings.commentFile.c_str());

                pSettings.commentFile.clear();
                pSettings.commentFile.shrink();
            }
        }
    }

    // Runtime data dedicated for the solver
    if (!study.initializeRuntimeInfos())
        throw Error::RuntimeInfoInitialization();

    // Apply transformations needed by the solver only (and not the interface for example)
    study.performTransformationsBeforeLaunchingSimulation();

    // Allocate all arrays
    SIM_AllocationTableaux(study);

    // Random-numbers generators
    initializeRandomNumberGenerators();
}

void Application::writeExectutionInfo()
{
    if (!pStudy)
        return;

    // Last missing duration to get : measure of total simulation duration
    pTotalTimer.stop();
    pDurationCollector.addDuration("total", pTotalTimer.get_duration());

    auto writer = pStudy->resultWriter;
    // If no writer is available, we can't write
    if (!writer)
        return;

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
    writer->addEntryFromBuffer(exec_info_path, content);
}

Application::~Application()
{
    // Destroy all remaining bouns (callbacks)
    destroyBoundEvents();

    // Release all allocated data
    if (!(!pStudy))
    {
        logs.info() << LOG_UI_SOLVER_DONE;

        // Copy the log file
        if (!pStudy->parameters.noOutput) {
            pStudy->importLogsToOutputFolder();
        }

        // release all reference to the current study held by this class
        pStudy->clear();
        pStudy = nullptr;

        LocalPolicy::Close();
    }
}
} // namespace Antares::Solver

