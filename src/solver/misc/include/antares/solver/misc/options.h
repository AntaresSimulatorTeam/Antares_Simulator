/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#ifndef __SOLVER_MISC_GETOPT_H__
#define __SOLVER_MISC_GETOPT_H__

#include <memory>

#include <yuni/yuni.h>
#include <yuni/core/getopt.h>
#include <yuni/core/string.h>

#include <antares/optimization-options/options.h>
#include <antares/study/study.h>

/*!
** \brief Command line settings for launching the simulation
*/
class Settings
{
public:
    void checkAndSetStudyFolder(const std::string& folder);
    void reset();

    //! Where is my study
    YString studyFolder;
    //! Name of the simulation
    Yuni::CString<150, false> simulationName;
    //! Comment file
    std::string commentFile;

    //! Simplex optimizatio range
    Yuni::CString<32, false> simplexOptimRange;

    //! Ignore error/warnings
    int ignoreWarningsErrors = 0;
    //! Ignore constraints
    bool ignoreConstraints = false;

    //! Run the TS generator only
    bool tsGeneratorsOnly = false;

    //! True to disable the writing in the output folder
    bool noOutput = false;
    //! Progression
    bool displayProgression = false;

    Yuni::String PID;
    bool forceZipOutput = false;
    Antares::Solver::Optimization::OptimizationOptions optOptions;
}; // class Settings

/*!
** \brief Interprete options given in command line
*/
std::unique_ptr<Yuni::GetOpt::Parser> CreateParser(Settings& settings,
                                                   Antares::Data::StudyLoadOptions& options);

void checkAndCorrectSettingsAndOptions(Settings& settings, Data::StudyLoadOptions& options);

void checkOrtoolsSolver(const Antares::Solver::Optimization::OptimizationOptions& optOptions);
#endif /* __SOLVER_MISC_GETOPT_H__ */
