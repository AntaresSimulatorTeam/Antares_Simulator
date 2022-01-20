#pragma once

#include "misc/options.h"
#include <antares/study.h>
#include <antares/study/load-options.h>
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

    /*!
    ** \brief Initialize data before launching the simulation
    **
    ** \return False if the operation failed.
    */
    int prepare(int argc, char* argv[]);

    /*!
    ** \brief Execute the simulation
    **
    ** \return Exit status
    */
    void execute();

    /*!
    ** \brief Reset the process priority (Windows only)
    */
    void resetProcessPriority();

private:
    /*!
    ** \brief Reset the log filename and open it
    */
    void resetLogFilename();

    /*!
     * \brief Load data of the study from a local or remote folder
     */
    void readDataForTheStudy(Antares::Data::StudyLoadOptions& options);

    void runSimulationInAdequacyMode();
    void runSimulationInAdequacyDraftMode();
    void runSimulationInEconomicMode();

    void initializeRandomNumberGenerators();

    void onLogMessage(int level, const YString& message);

    /*!
    ** \brief Run an arbitrary simulation
    **
    ** see main/{adequacy.cpp, economy.cpp, adequacy-draft.cpp} for specific
    ** instanciation
    */
    template<class SimulationT>
    void runSimulation();

    void processCaption(const Yuni::String& caption);

    //! The settings given from the command line
    Settings pSettings;
    //! The current Antares study
    Antares::Data::Study::Ptr pStudy;
    //! General data related to the current study
    Antares::Data::Parameters* pParameters;
    //! The total number of errors which have been generated
    uint pErrorCount;
    //! The total muber of warnings which have been generated
    uint pWarningCount;

    int pArgc;
    char** pArgv;

}; // class Application

#include "application.hxx"
} // namespace Solver
} // namespace Antares
