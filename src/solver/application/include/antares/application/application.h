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
#pragma once

#include <yuni/core/string.h>

#include <antares/benchmarking/DurationCollector.h>
#include <antares/benchmarking/timer.h>
#include <antares/study/load-options.h>
#include <antares/study/study.h>
#include <antares/writer/i_writer.h>
#include "antares/infoCollection/StudyInfoCollector.h"
#include "antares/solver/misc/options.h"

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

    /**
     * @brief /!\ Acquire the study. Leave Application object in an invalid state.
     * @return The study
     */
    std::unique_ptr<Data::Study> acquireStudy()
    {
        return std::move(pStudy);
    }

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
    void runSimulationInEconomicMode();

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
    char** pArgv = nullptr;

    // Benchmarking
    Benchmarking::Timer pTotalTimer;
    Benchmarking::DurationCollector pDurationCollector;
    Benchmarking::OptimizationInfo pOptimizationInfo;

    std::shared_ptr<Yuni::Job::QueueService> ioQueueService;
    IResultWriter::Ptr resultWriter = nullptr;

    void prepareWriter(const Antares::Data::Study& study,
                       Benchmarking::DurationCollector& duration_collector);

    void writeComment(Data::Study& study);
    void startSimulation(Data::StudyLoadOptions& options);
    // Return false if the user requested the version ,available solvers, etc, true otherwise
    bool handleOptions(const Data::StudyLoadOptions& options);
    // Return false if the user requested help, true otherwise
    bool parseCommandLine(Data::StudyLoadOptions& options);
    void handleParserReturn(Yuni::GetOpt::Parser* parser);
    void postParametersChecks() const;

}; // class Application
} // namespace Antares::Solver
