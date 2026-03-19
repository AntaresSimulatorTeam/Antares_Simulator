// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <yuni/core/string.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/benchmarking/timer.h>
#include <antares/study/load-options.h>
#include <antares/study/study.h>
#include <antares/writer/i_writer.h>
#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/solver/misc/options.h"
#include "antares/solver/simulation/ISimulationObserver.h"

namespace Antares::Solver
{

class Application final: public Yuni::IEventObserver<Application, Yuni::Policy::SingleThreaded>
{
public:
    //! \name Constructor & Destructor
    //@{
    /*!
    ** \brief Default Constructor
    */
    Application();
    /*!
    ** \brief Destructor
    */
    ~Application() override;
    //@}

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

    /*!
    ** \brief Initialize data before launching the simulation
    **
    ** \return False if the operation failed.
    */
    void prepare(int argc, const char* argv[]);

    /*!
    ** \brief Execute the simulation
    **
    ** \return Exit status
    */
    void execute();

    /*!
    ** \brief Reset the process priority (Windows only)
    */
    void resetProcessPriority() const;

    void writeExecutionInfo();

    /**
     * @brief /!\ Acquire the study. Leave Application object in an invalid state.
     * @return pair  {study, IResultWriter}
     */
    std::pair<std::unique_ptr<Data::Study>, IResultWriter::Ptr> acquireStudy()
    {
        return {std::move(pStudy), std::move(resultWriter)};
    }

    Benchmarking::DurationCollector& getDurationCollector()
    {
        return pDurationCollector;
    }

private:
    enum class LogType
    {
        Warning,
        Error
    };

    /*!
    ** \brief Reset the log filename and open it
    */
    void resetLogFilename() const;

    /*!
     * \brief Load data of the study from a local or remote folder
     */
    void readDataForTheStudy(Antares::Data::StudyLoadOptions& options);

    void onLogMessage(int level, const std::string& message);

    //! The settings given from the command line
    Settings pSettings;
    //! The current Antares study
    std::unique_ptr<Antares::Data::Study> pStudy;
    //! General data related to the current study
    Antares::Data::Parameters* pParameters = nullptr;
    //! The total number of errors which have been generated
    uint pErrorCount = 0;
    //! The total muber of warnings which have been generated
    uint pWarningCount = 0;

    int pArgc = 0;
    const char** pArgv = nullptr;

    // Benchmarking
    Benchmarking::DurationCollector pDurationCollector;
    Benchmarking::OptimizationInfo pOptimizationInfo;

    std::shared_ptr<Yuni::Job::QueueService> ioQueueService;
    IResultWriter::Ptr resultWriter = nullptr;
    std::vector<std::pair<LogType, std::string>> messagesStack;

    void prepareWriter(const Antares::Data::Study& study,
                       Benchmarking::DurationCollector& duration_collector);

    void writeComment(Data::Study& study);
    void readStudy_makeChecks_and_printThings(Data::StudyLoadOptions& options);
    // Return false if the user requested the version ,available solvers, etc, true otherwise
    bool handleOptions(const Data::StudyLoadOptions& options);
    void LogMessageStack(std::vector<std::pair<LogType, std::string>>& stack);
    // Return false if the user requested help, true otherwise
    bool parseCommandLine(Data::StudyLoadOptions& options);
    void postParametersChecks() const;
}; // class Application

void writeSimulationInfos(const Data::Study& study,
                          Benchmarking::DurationCollector& durationCollector,
                          const Benchmarking::OptimizationInfo& optimizationInfo,
                          IResultWriter* resultWriter);

} // namespace Antares::Solver
