/*
** Copyright 2007-2018 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Antares_Simulator. If not, see <http://www.gnu.org/licenses/>.
**
** SPDX-License-Identifier: licenceRef-GPL3_WITH_RTE-Exceptions
*/
#include <yuni/yuni.h>
#include <yuni/datetime/timestamp.h>
#include <yuni/core/process/rename.h>
#include <yuni/core/system/suspend.h>
#include <stdarg.h>
#include <new>

#include "config.h"
#include <antares/study/study.h>
#include <antares/logs.h>
#include "main.h"
#include "../ui/common/winmain.hxx"

#include <time.h>
#include <antares/hostinfo.h>

#include <antares/resources/resources.h>
#include "../config.h"
#include <antares/emergency.h>
#include <antares/memory/memory.h>
#include <antares/sys/policy.h>
#include <antares/locale.h>
#include "../internet/license.h"
#include "misc/system-memory.h"
//#include <antares/proxy/proxy.h>

#ifdef YUNI_OS_WINDOWS
#include <conio.h>
#else
#include <unistd.h>
#endif

using namespace Antares;
using namespace Yuni;

#define SEP Yuni::IO::Separator

#define GPL_ANNOUNCEMENT \
    "Copyright 2007-2018 RTE  - Authors: The Antares_Simulator Team \n\
\n\
Antares_Simulator is free software : you can redistribute it and / or modify\n\
it under the terms of the GNU General Public License as published by\n\
the Free Software Foundation, either version 3 of the License, or\n\
(at your option) any later version.\n\
\n\
There are special exceptions to the terms and conditions of the\n\
license as they are applied to this software.View the full text of\n\
the exceptions in file COPYING.txt in the directory of a distribution\n\
of this software in source form.\n\
\n\
Antares_Simulator is distributed in the hope that it will be useful, \n\
but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the\n\
GNU General Public License for more details.\n\
\n\
You should have received a copy of the GNU General Public License\n\
along with Antares_Simulator.If not, see <http://www.gnu.org/licenses/>.\n\
\n\n"

SolverApplication::SolverApplication() :
 pStudy(nullptr), pParameters(nullptr), pErrorCount(0), pWarningCount(0)
{
    resetProcessPriority();
}

bool SolverApplication::prepare(int argc, char* argv[])
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

    // Parse arguments and store usefull values
    if (not GrabOptionsFromCommandLine(argc, argv, pSettings, options))
        return false;

    // Determine the log filename to use for this simulation
    resetLogFilename();

    // Starting !
    logs.checkpoint() << "Antares Solver v" << ANTARES_VERSION_PUB_STR;
    WriteHostInfoIntoLogs();
    logs.info();

#ifdef ANTARES_SWAP_SUPPORT
    // Changing the swap folder
    if (not pSettings.swap.empty())
    {
        logs.info() << "  memory pool: scratch folder:" << pSettings.swap;
        Antares::memory.cacheFolder(pSettings.swap);
    }
    else
        logs.info() << "  memory pool: scratch folder:" << Antares::memory.cacheFolder();
#endif

    // Initialize the main structures for the simulation
    // Logs
    Resources::WriteRootFolderToLogs();
    logs.info() << "  :: log filename: " << logs.logfile();
    // Temporary use a callback to count the number of errors and warnings
    logs.callback.connect(this, &SolverApplication::onLogMessage);

    // Allocate a study
    pStudy = new Data::Study(true /* for the solver */);

    // Setting global variables for backward compatibility
    Data::Study::Current::Set(pStudy);
    pParameters = &(pStudy->parameters);

    // Loading the study
    if (not readDataForTheStudy(options))
        return false;

    // LISTE DE CHECKS ...

    // CHECK incompatible de choix simultané des options « simplex range= daily » et « hydro-pricing
    // = MILP ».
    if ((pParameters->simplexOptimizationRange == Antares::Data::SimplexOptimization::sorDay)
        && (pParameters->hydroPricing.hpMode == Antares::Data::HydroPricingMode::hpMILP))
    {
        logs.error()
          << "Simplexe optimization range and hydro pricing mode : values are not compatible ";
        return false;
    }

    // CHECK incompatible de choix simultané des options « simplex range= daily » et «
    // unit-commitment = MILP ».
    if ((pParameters->simplexOptimizationRange == Antares::Data::SimplexOptimization::sorDay)
        && (pParameters->unitCommitment.ucMode == Antares::Data::UnitCommitmentMode::ucMILP))
    {
        logs.error()
          << "Simplexe optimization range and unit commitment mode : values are not compatible ";
        return false;
    }

    // CHECK
    // Daily simplex optimisation and any area's use heurictic target turned to "No" are not
    // compatible.
    if (pParameters->simplexOptimizationRange == Antares::Data::SimplexOptimization::sorDay)
    {
        for (uint i = 0; i < pStudy->areas.size(); ++i)
        {
            auto& area = *(pStudy->areas.byIndex[i]);
            if (!area.hydro.useHeuristicTarget)
            {
                logs.error() << "Area " << area.name
                             << " : simplex daily optimization and use heuristic target == no are "
                                "not compatible.";
                return false;
            }
        }
    }

    // CHECK MinStablePower
    bool tsGenThermal = (0
                         != (pStudy->parameters.timeSeriesToGenerate
                             & Antares::Data::TimeSeries::timeSeriesThermal));
    logs.debug() << "tsGenThermal = " << tsGenThermal;

    if (tsGenThermal)
    {
        std::map<int, YString> areaClusterNames;
        if (!(pStudy->areasThermalClustersMinStablePowerValidity(areaClusterNames)))
        {
            for (auto it = areaClusterNames.begin(); it != areaClusterNames.end(); it++)
            {
                logs.fatal()
                  << it->second
                  << ". Conflict between Min Stable Power, Pnom, spinning and capacity modulation.";
            }

            return false;
        }
    }

    // CHECK PuissanceDisponible
    /* Caracteristiques des paliers thermiques */
    if (!tsGenThermal) // no time series generation asked (off mode)
    {
        for (uint i = 0; i < pStudy->areas.size(); ++i)
        {
            // Alias de la zone courant
            auto& area = *(pStudy->areas.byIndex[i]);

            auto NombreDePaliersThermiques = area.thermal.list.size();

            for (uint l = 0; l != area.thermal.clusterCount; ++l) //
            {
                auto& cluster = *(area.thermal.clusters[l]);
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

    // Start the progress meter
    pStudy->initializeProgressMeter(pSettings.tsGeneratorsOnly);
    if (pSettings.noOutput)
        pSettings.displayProgression = false;

    if (pSettings.displayProgression)
    {
        pStudy->buffer.clear() << pStudy->folderOutput << SEP << "about-the-study" << SEP << "map";
        if (not pStudy->progression.saveToFile(pStudy->buffer))
        {
            logs.error() << "I/O error: impossible to write " << pStudy->buffer;
            return false;
        }
        pStudy->progression.start();
    }
    else
        logs.info() << "  The progression is disabled";

    return true;
}

void SolverApplication::initializeRandomNumberGenerators()
{
    logs.info() << "Initializing random number generators...";
    auto& parameters = pStudy->parameters;
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

void SolverApplication::onLogMessage(int level, const Yuni::String& /*message*/)
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

int SolverApplication::execute()
{
    processCaption(String() << "antares: running \"" << pStudy->header.caption << "\"");

    SystemMemoryLogger memoryReport;
    memoryReport.interval(1000 * 60 * 5); // 5 minutes
    memoryReport.start();

    //! Calculate (*pMatrix)[Data::thermalMinGenModulation][y] * pCluster->unitCount *
    //! pCluster->nominalCapacity;
    for (uint i = 0; i != pStudy->areas.size(); i++)
    {
        // Alias de la zone courant
        auto& area = *(pStudy->areas.byIndex[i]);

        for (uint j = 0; j < area.thermal.list.size(); j++)
        {
            // Alias du cluster courant
            auto& cluster = area.thermal.list.byIndex[j];
            for (uint k = 0; k < 8760; k++)
                cluster->PthetaInf[k] = cluster->modulation[Data::thermalMinGenModulation][k]
                                        * cluster->unitCount * cluster->nominalCapacity;
        }
    }

    // Run the simulation
    {
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
        case Data::stdmUnknown:
        case Data::stdmMax:
            break;
        }
    }

    // Importing Time-Series if asked
    pStudy->importTimeseriesIntoInput();

    // Stop the display of the progression
    pStudy->progression.stop();

    // exit status
    return 0;
}

void SolverApplication::resetLogFilename()
{
    // Assigning the log file
    String logfile;
    logfile << pSettings.studyFolder << SEP << "logs";

    // Making sure that the folder
    if (not IO::Directory::Create(logfile))
    {
        logs.fatal() << "Impossible to create the log folder. Aborting now.";
        logs.info() << "  Target: " << logfile;
        AntaresSolverEmergencyShutdown(); // no return
    }

    // Date/time
    logfile << SEP << "solver-";
    DateTime::TimestampToString(logfile, "%Y%m%d-%H%M%S", 0, false);
    logfile << ".log";

    // Assigning the log filename
    logs.logfile(logfile);

    if (not logs.logfileIsOpened())
    {
        logs.error() << "Impossible to create " << logfile;
        AntaresSolverEmergencyShutdown(); // will never return
    }
}

void SolverApplication::processCaption(const AnyString& caption)
{
    pArgv = Yuni::Process::Rename(pArgc, pArgv, caption);
}

bool SolverApplication::readDataForTheStudy(Data::StudyLoadOptions& options)
{
    processCaption(String() << "antares: loading \"" << pSettings.studyFolder << "\"");
    auto& study = *pStudy;

    // Init the global variable for backward compatibility
    AppelEnModeSimulateur = OUI_ANTARES;

    // Name of the simulation
    if (not pSettings.simulationName.empty())
        study.simulation.name = pSettings.simulationName;

    // Force some options
    options.prepareOutput = not pSettings.noOutput;
    options.ignoreConstraints = pSettings.ignoreConstraints;
    options.loadOnlyNeeded = true;

    // Load the study from a folder
    if (study.loadFromFolder(pSettings.studyFolder, options) and not study.gotFatalError)
    {
        logs.info() << "The study is loaded.";
        logs.info() << LOG_UI_DISPLAY_MESSAGES_OFF;
    }

    if (study.gotFatalError)
        return false;

    if (study.areas.empty())
    {
        logs.fatal() << "no area found";
        return false;
    }

    // no output ?
    study.parameters.noOutput = pSettings.noOutput;

    // Name of the simulation (again, if the value has been overwritten)
    if (not pSettings.simulationName.empty())
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
    if (pErrorCount or pWarningCount or study.gotFatalError)
    {
        if (pErrorCount or not pSettings.ignoreWarningsErrors)
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
            if (not study.parameters.noOutput)
                study.importLogsToOutputFolder();
            // empty line
            logs.info();
        }
    }

    // Checking for filename length limits
    if (not pSettings.noOutput)
    {
        if (not study.checkForFilenameLimits(true))
            return false;

        // comments
        {
            study.buffer.clear() << study.folderOutput << SEP << "simulation-comments.txt";

            if (not pSettings.commentFile.empty())
            {
                IO::Directory::Create(study.folderOutput);
                if (IO::errNone != IO::File::Copy(pSettings.commentFile, study.buffer, true))
                    logs.error() << "impossible to copy `" << pSettings.commentFile << "` to `"
                                 << study.buffer << '`';
                pSettings.commentFile.clear();
                pSettings.commentFile.shrink();
            }
            else
            {
                if (not IO::File::CreateEmptyFile(study.buffer))
                    logs.error() << study.buffer << ": impossible to overwrite its content";
            }
        }
    }

    // Runtime data dedicated for the solver
    if (not study.initializeRuntimeInfos())
        return false;

    // Apply transformations needed by the solver only (and not the interface for example)
    study.performTransformationsBeforeLaunchingSimulation();

    // Allocate all arrays
    SIM_AllocationTableaux();

    // Random-numbers generators
    initializeRandomNumberGenerators();

    return true;
}

bool SolverApplication::completeWithOnlineCheck()
{
    // if first connection ask proxy parameters and try again
    std::cout << "Do you want to use proxy?(yes or no):\n";
    String enable;
    std::cin >> enable;
    if (enable == "yes")
    {
        License::proxy.enabled = true;

        std::cout << "enter host address:\n";
        std::cin >> License::proxy.host;

        std::cout << "enter port:\n";
        std::cin >> License::proxy.port;

        std::cout << "enter login:\n";
        std::cin >> License::proxy.login;

#ifdef YUNI_OS_WINDOWS
        std::cout << "enter password:\n";
        std::string password = "";
        char ch;
        ch = _getch();
        while (ch != 0x0D)
        {
            // character 0x0D is enter
            password.push_back(ch);
            std::cout << '*';
            ch = _getch();
        }
#else
        char* password;
        password = getpass("enter password:\n");
#endif

        // set password
        License::proxy.password << password;

        // check proxy parameters
        if (not License::proxy.check())
        {
            logs.fatal() << "invalid proxy parameters";
            return false;
        }
    }
    else
    {
        License::proxy.enabled = false;
    }

    // verify License online
    if (not License::CheckOnlineLicenseValidity(Data::versionLatest, true))
        return false;

    // if ok, save proxy parameers
    License::proxy.saveProxyFile();

    return true;
}

SolverApplication::~SolverApplication()
{
    // Destroy all remaining bouns (callbacks)
    destroyBoundEvents();

    // Release all allocated data
    if (!(!pStudy))
    {
        logs.info() << LOG_UI_SOLVER_DONE;

        // Copy the log file
        if (not pStudy->parameters.noOutput)
            pStudy->importLogsToOutputFolder();

        // simulation
        SIM_DesallocationTableaux();

        // release all reference to the current study held by this class
        pStudy->clear();
        pStudy = nullptr;

        // only used if a study exists
        // Removing all unused spwa files
        Antares::memory.removeAllUnusedSwapFiles();
        LocalPolicy::Close();
        logs.info() << "Done.";
    }
}

static void NotEnoughMemory()
{
    logs.fatal() << "Not enough memory. aborting.";
    exit(42);
}

/*!
** \brief main
*/
int main(int argc, char** argv)
{
    logs.info(GPL_ANNOUNCEMENT);
    // Name of the running application for the logger
    logs.applicationName("solver");

    // Dealing with the lack of memory
    std::set_new_handler(&NotEnoughMemory);

    // Antares SWAP
    if (not memory.initialize())
        return EXIT_FAILURE;

    // locale
    InitializeDefaultLocale();

    // Getting real UTF8 arguments
    argv = AntaresGetUTF8Arguments(argc, argv);

    // Disabling the log notice about disk space reservation
    Antares::Memory::InformAboutDiskSpaceReservation = false;

    // TODO It would be nice if it were removed...
    // This jump is only required by the internal solver
    CompteRendu.AnomalieDetectee = NON_ANTARES;
    setjmp(CompteRendu.Env);
    if (CompteRendu.AnomalieDetectee == OUI_ANTARES)
    {
        logs.error() << "Error...";
        AntaresSolverEmergencyShutdown(); // will never return
        return 42;
    }

    int ret = EXIT_FAILURE;

    auto* application = new SolverApplication();
    if (application->prepare(argc, argv))
        ret = application->execute();
    delete application;

    FreeUTF8Arguments(argc, argv);

    // to avoid a bug from wxExecute, we should wait a little before returning
    SuspendMilliSeconds(200 /*ms*/);

    return ret;
}
