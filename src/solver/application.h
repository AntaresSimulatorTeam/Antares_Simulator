/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,** Adequacy and Performance assesment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
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
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#pragma once

#include "misc/options.h"
#include <antares/study/study.h>
#include <antares/study/load-options.h>
#include <antares/benchmarking/DurationCollector.h>
#include <antares/benchmarking/timer.h>
#include "simulation/simulation.h"
#include "antares/infoCollection/StudyInfoCollector.h"

#include <antares/writer/i_writer.h>
#include <yuni/core/string.h>

namespace Antares::Solver
{
class Application final : public Yuni::IEventObserver<Application, Yuni::Policy::SingleThreaded>
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

    std::shared_ptr<Yuni::Job::QueueService> ioQueueService;
    IResultWriter::Ptr resultWriter = nullptr;

    void prepareWriter(Antares::Data::Study& study,
                       Benchmarking::IDurationCollector& duration_collector);

}; // class Application
} // namespace Antares::Solver

