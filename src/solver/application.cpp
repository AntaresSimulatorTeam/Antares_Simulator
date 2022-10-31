#include "application.h"

#include <antares/sys/policy.h>
#include <antares/resources/resources.h>
#include <antares/hostinfo.h>
#include <antares/exception/LoadingError.hpp>
#include <antares/emergency.h>
#include "../config.h"

#include "misc/system-memory.h"
#include "utils/ortools_utils.h"

#include <yuni/io/io.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/core/process/rename.h>

#include <algorithm>

namespace
{
void checkStudyVersion(const AnyString& optStudyFolder)
{
    using namespace Antares::Data;
    auto version = StudyTryToFindTheVersion(optStudyFolder);
    if (version == versionUnknown)
    {
        throw Error::InvalidStudy(optStudyFolder);
    }
    else
    {
        if ((uint)version > (uint)versionLatest)
        {
            throw Error::InvalidVersion(VersionToCStr(version),
                                        VersionToCStr((Version)versionLatest));
        }
    }
}

void printVersion()
{
#ifdef GIT_SHA1_SHORT_STRING
    std::cout << ANTARES_VERSION_STR << " (revision " << GIT_SHA1_SHORT_STRING << ")" << std::endl;
#else
    std::cout << ANTARES_VERSION_STR << std::endl;
#endif
}

void printSolvers()
{
    std::cout << "Available solvers :" << std::endl;
    OrtoolsUtils utils;
    for (const auto& solver : utils.getAvailableOrtoolsSolverName())
    {
        std::cout << solver << std::endl;
    }
}

// CHECK incompatible de choix simultané des options « simplex range= daily » et « hydro-pricing
// = MILP ».
void checkSimplexRangeHydroPricing(Antares::Data::SimplexOptimization optRange,
                                   Antares::Data::HydroPricingMode hpMode)
{
    if (optRange == Antares::Data::SimplexOptimization::sorDay
        && hpMode == Antares::Data::HydroPricingMode::hpMILP)
    {
        throw Error::IncompatibleOptRangeHydroPricing();
    }
}

// CHECK incompatible de choix simultané des options « simplex range= daily » et «
// unit-commitment = MILP ».
void checkSimplexRangeUnitCommitmentMode(Antares::Data::SimplexOptimization optRange,
                                         Antares::Data::UnitCommitmentMode ucMode)
{
    if (optRange == Antares::Data::SimplexOptimization::sorDay
        && ucMode == Antares::Data::UnitCommitmentMode::ucMILP)
    {
        throw Error::IncompatibleOptRangeUCMode();
    }
}

// Daily simplex optimisation and any area's use heurictic target turned to "No" are not
// compatible.
void checkSimplexRangeHydroHeuristic(Antares::Data::SimplexOptimization optRange,
                                     const Antares::Data::AreaList& areas)
{
    if (optRange == Antares::Data::SimplexOptimization::sorDay)
    {
        for (uint i = 0; i < areas.size(); ++i)
        {
            const auto& area = *(areas.byIndex[i]);
            if (!area.hydro.useHeuristicTarget)
            {
                throw Error::IncompatibleDailyOptHeuristicForArea(area.name);
            }
        }
    }
}

// Adequacy Patch can only be used with Economy Study/Simulation Mode.
void checkAdqPatchStudyModeEconomyOnly(const bool adqPatchOn,
                                       const Antares::Data::StudyMode studyMode)
{
    if ((studyMode != Antares::Data::StudyMode::stdmEconomy) && adqPatchOn)
    {
        throw Error::IncompatibleStudyModeForAdqPatch();
    }
}
// When Adequacy Patch is on at least one area must be inside Adequacy patch mode.
void checkAdqPatchContainsAdqPatchArea(const bool adqPatchOn, const Antares::Data::AreaList& areas)
{
    using namespace Antares::Data;
    if (adqPatchOn)
    {
        const bool containsAdqArea
          = std::any_of(areas.cbegin(), areas.cend(), [](const std::pair<AreaName, Area*>& area) {
                return area.second->adequacyPatchMode == AdequacyPatch::physicalAreaInsideAdqPatch;
            });
        if (!containsAdqArea)
            throw Error::NoAreaInsideAdqPatchMode();
    }
}

void checkMinStablePower(bool tsGenThermal, const Antares::Data::AreaList& areas)
{
    if (tsGenThermal)
    {
        std::map<int, YString> areaClusterNames;
        if (!(areasThermalClustersMinStablePowerValidity(areas, areaClusterNames)))
        {
            throw Error::InvalidParametersForThermalClusters(areaClusterNames);
        }
    }

    // CHECK PuissanceDisponible
    /* Caracteristiques des paliers thermiques */
    if (!tsGenThermal) // no time series generation asked (off mode)
    {
        for (uint i = 0; i < areas.size(); ++i)
        {
            // Alias de la zone courant
            auto& area = *(areas.byIndex[i]);

            for (uint l = 0; l != area.thermal.clusterCount(); ++l) //
            {
                const auto& cluster = *(area.thermal.clusters[l]);
                auto PmaxDUnGroupeDuPalierThermique = cluster.nominalCapacityWithSpinning;
                auto PminDUnGroupeDuPalierThermique
                  = (cluster.nominalCapacityWithSpinning < cluster.minStablePower)
                      ? cluster.nominalCapacityWithSpinning
                      : cluster.minStablePower;

                bool condition = false;
                bool report = false;

                for (uint y = 0; y != cluster.series->series.height; ++y)
                {
                    for (uint x = 0; x != cluster.series->series.width; ++x)
                    {
                        auto rightpart = PminDUnGroupeDuPalierThermique
                                         * ceil(cluster.series->series.entry[x][y]
                                                / PmaxDUnGroupeDuPalierThermique);
                        condition = rightpart > cluster.series->series.entry[x][y];
                        if (condition)
                        {
                            cluster.series->series.entry[x][y] = rightpart;
                            report = true;
                        }
                    }
                }

                if (report)
                    logs.warning() << "Area : " << area.name << " cluster name : " << cluster.name()
                                   << " available power lifted to match Pmin and Pnom requirements";
            }
        }
    }
}

void checkSplitMPSWithORTOOLS(bool ortoolsUsed, bool splitExportedMPS)
{
    if (ortoolsUsed && splitExportedMPS)
    {
        throw Error::InvalidParametersORTools_SplitMPS();
    }
}

} // namespace

namespace Antares
{
namespace Solver
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
    if (!parser->operator()(argc, argv))
        throw Error::CommandLineArguments(parser->errors());

    if (options.displayVersion)
    {
        printVersion();
        return;
    }

    if (options.listSolvers)
    {
        printSolvers();
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
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_PUB_STR << " ("
                      << GIT_SHA1_SHORT_STRING << ")";
#else
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_PUB_STR;
#endif
    WriteHostInfoIntoLogs();
    logs.info();

    // Initialize the main structures for the simulation
    // Logs
    Resources::WriteRootFolderToLogs();
    logs.info() << "  :: log filename: " << logs.logfile();
    // Temporary use a callback to count the number of errors and warnings
    logs.callback.connect(this, &Application::onLogMessage);

    // Allocate a study
    pStudy = std::make_shared<Antares::Data::Study>(true /* for the solver */);

    // Setting global variables for backward compatibility
    Data::Study::Current::Set(pStudy);
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

    checkAdqPatchStudyModeEconomyOnly(pParameters->adqPatch.enabled, pParameters->mode);
    checkAdqPatchContainsAdqPatchArea(pParameters->adqPatch.enabled, pStudy->areas);

    bool tsGenThermal
      = (0 != (pParameters->timeSeriesToGenerate & Antares::Data::TimeSeries::timeSeriesThermal));

    checkMinStablePower(tsGenThermal, pStudy->areas);

    checkSplitMPSWithORTOOLS(pParameters->ortoolsUsed, pParameters->include.splitExportedMPS);

    // Start the progress meter
    pStudy->initializeProgressMeter(pSettings.tsGeneratorsOnly);
    if (pSettings.noOutput)
        pSettings.displayProgression = false;

    if (pSettings.displayProgression)
    {
        pStudy->buffer.clear() << pStudy->folderOutput << Yuni::IO::Separator << "about-the-study"
                               << Yuni::IO::Separator << "map";
        if (!pStudy->progression.saveToFile(pStudy->buffer))
        {
            throw Error::WritingProgressFile(pStudy->buffer);
        }
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
    case Data::stdmAdequacyDraft:
        runSimulationInAdequacyDraftMode();
        break;
    default:
        break;
    }

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
        logs.fatal() << "Impossible to create the log folder. Aborting now.";
        logs.info() << "  Target: " << logfile;
        AntaresSolverEmergencyShutdown(); // no return
    }

    // Date/time
    logfile << Yuni::IO::Separator << "solver-";
    Yuni::DateTime::TimestampToString(logfile, "%Y%m%d-%H%M%S", 0, false);
    logfile << ".log";

    // Assigning the log filename
    logs.logfile(logfile);

    if (!logs.logfileIsOpened())
    {
        logs.error() << "Impossible to create " << logfile;
        AntaresSolverEmergencyShutdown(); // will never return
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

    // Init the global variable for backward compatibility
    AppelEnModeSimulateur = OUI_ANTARES;

    // Name of the simulation
    if (!pSettings.simulationName.empty())
        study.simulation.name = pSettings.simulationName;

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

    // Name of the simulation (again, if the value has been overwritten)
    if (!pSettings.simulationName.empty())
        study.simulation.name = pSettings.simulationName;

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
            AntaresSolverEmergencyShutdown();
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
            study.buffer.clear() << study.folderOutput << Yuni::IO::Separator
                                 << "simulation-comments.txt";

            if (!pSettings.commentFile.empty())
            {
                Yuni::IO::Directory::Create(study.folderOutput);
                if (Yuni::IO::errNone
                    != Yuni::IO::File::Copy(pSettings.commentFile, study.buffer, true))
                    logs.error() << "impossible to copy `" << pSettings.commentFile << "` to `"
                                 << study.buffer << '`';
                pSettings.commentFile.clear();
                pSettings.commentFile.shrink();
            }
            else
            {
                if (!Yuni::IO::File::CreateEmptyFile(study.buffer))
                    logs.error() << study.buffer << ": impossible to overwrite its content";
            }
        }
    }

    // Runtime data dedicated for the solver
    if (!study.initializeRuntimeInfos())
        throw Error::RuntimeInfoInitialization();

    // Apply transformations needed by the solver only (and not the interface for example)
    study.performTransformationsBeforeLaunchingSimulation();

    // Allocate all arrays
    SIM_AllocationTableaux();

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

    // Info collectors : they retrieve data from study and simulation
    Benchmarking::StudyInfoCollector study_info_collector(*pStudy);
    Benchmarking::SimulationInfoCollector simulation_info_collector(pOptimizationInfo);

    // Fill file content with data retrieved by collectors
    Benchmarking::FileContent file_content;
    pDurationCollector.toFileContent(file_content);
    study_info_collector.toFileContent(file_content);
    simulation_info_collector.toFileContent(file_content);

    // Flush previous info into a record file
    Yuni::String filePath;
    filePath.clear() << pStudy->folderOutput << Yuni::IO::Separator << "execution_info.ini";
    Benchmarking::iniFilewriter ini_file_writer(filePath, file_content);
    ini_file_writer.flush();
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
        if (!pStudy->parameters.noOutput)
            pStudy->importLogsToOutputFolder();

        // simulation
        SIM_DesallocationTableaux();

        // release all reference to the current study held by this class
        pStudy->clear();
        pStudy = nullptr;

        LocalPolicy::Close();
    }
}
} // namespace Solver
} // namespace Antares
