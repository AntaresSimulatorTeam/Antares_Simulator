#pragma once

#include "misc/options.h"
#include <antares/study.h>
#include <antares/study/load-options.h>
#include <antares/timeelapsed.h>
#include "simulation/simulation.h"

#include <yuni/core/string.h>

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

    bool shouldExecute = true;

    TimeElapsed::Aggregator pTimeElapsedAggregator;
    std::unique_ptr<TimeElapsed::Timer> pTotalTimer;
    TimeElapsed::FileWriter mTimeElapsedWriter;
}; // class Application
} // namespace Solver
} // namespace Antares
