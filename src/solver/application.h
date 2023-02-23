#pragma once

#include "misc/options.h"
#include <antares/study.h>
#include <antares/study/load-options.h>
#include <antares/benchmarking.h>
#include "simulation/simulation.h"

#include <i_writer.h>

#include <yuni/core/string.h>

// TODO[FOM] Move to appropriate file
// Note that of version 9.x, OR-Tools redefines the GLOG
// library. Including glog/logging.h will result in compilation
// errors (symbol redifinition)
#include "ortools/base/logging.h"
class AntaresSink : public google::LogSink
{
    void send(LogSeverity severity,
              const char* /* full_filename */,
              const char* /* base_filename */,
              int /* line */,
              const struct ::tm* /* tm_time */,
              const char* message,
              size_t message_len) override
    {
        // message *is not* '\0'-terminated
        // We could use std::string_view to avoid copies,
        // but Yuni::Logs::Logger doesn't support them at the moment.
        const std::string msg(message, message_len);
        switch (severity)
        {
        case google::GLOG_INFO:
            Antares::logs.info() << msg;
            break;
        case google::GLOG_WARNING:
            Antares::logs.warning() << msg;
            break;
        case google::GLOG_ERROR:
            Antares::logs.error() << msg;
            break;
        case google::GLOG_FATAL:
            Antares::logs.fatal() << msg;
            break;
        default:
            Antares::logs.info() << "unknown log level " << message;
        }
    }
};

namespace Antares
{
namespace Solver
{
class Application : public Yuni::IEventObserver<Application, Yuni::Policy::SingleThreaded>
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
    ~Application() final;
    //@}

    Application(const Application&) = delete;

    Application& operator=(const Application&) = delete;

    /*!
    ** \brief Initialize data before launching the simulation
    **
    ** \return False if the operation failed.
    */
    void prepare(int argc, char* argv[]);

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

    void writeExectutionInfo();

    void installSignalHandlers() const;

private:
    /*!
    ** \brief Reset the log filename and open it
    */
    void resetLogFilename() const;

    /*!
     * \brief Load data of the study from a local or remote folder
     */
    void readDataForTheStudy(Antares::Data::StudyLoadOptions& options);

    void runSimulationInAdequacyMode();
    void runSimulationInAdequacyDraftMode();
    void runSimulationInEconomicMode();

    void initializeRandomNumberGenerators();

    void onLogMessage(int level, const YString& message);

    void processCaption(const Yuni::String& caption);

    //! The settings given from the command line
    Settings pSettings;
    //! The current Antares study
    Antares::Data::Study::Ptr pStudy = nullptr;
    //! General data related to the current study
    Antares::Data::Parameters* pParameters = nullptr;
    //! The total number of errors which have been generated
    uint pErrorCount = 0;
    //! The total muber of warnings which have been generated
    uint pWarningCount = 0;

    int pArgc;
    char** pArgv;

    // Benchmarking
    Benchmarking::Timer pTotalTimer;
    Benchmarking::DurationCollector pDurationCollector;
    Benchmarking::OptimizationInfo pOptimizationInfo;
    // Log sink for OR-Tools
    AntaresSink sink;
}; // class Application
} // namespace Solver
} // namespace Antares
