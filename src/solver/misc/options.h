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
#ifndef __SOLVER_MISC_GETOPT_H__
#define __SOLVER_MISC_GETOPT_H__

#include <memory>
#include <yuni/yuni.h>
#include <yuni/core/string.h>
#include <yuni/core/getopt.h>

#include <antares/study/study.h>

/*!
** \brief Command line settings for launching the simulation
*/
class Settings
{
public:
    //! Where is my study
    YString studyFolder;
    //! Name of the simulation
    Yuni::CString<150, false> simulationName;
    //! Comment file
    YString commentFile;

    //! Simplex optimizatio range
    Yuni::CString<32, false> simplexOptimRange;

    //! Ignore error/warnings
    int ignoreWarningsErrors = 0;
    //! Ignore constraints
    bool ignoreConstraints = false;
    //!
    bool tsGeneratorsOnly = false;
    //! True to disable the writing in the output folder
    bool noOutput = false;
    //! Progression
    bool displayProgression = false;

    Yuni::String PID;

    bool forceZipOutput = false;

    void checkAndSetStudyFolder(Yuni::String folder);
    void reset();
}; // class Settings

/*!
** \brief Interprete options given in command line
*/
std::unique_ptr<Yuni::GetOpt::Parser> CreateParser(Settings& settings,
                                                   Antares::Data::StudyLoadOptions& options);

void checkAndCorrectSettingsAndOptions(Settings& settings, Data::StudyLoadOptions& options);

void checkOrtoolsSolver(Data::StudyLoadOptions& options);
#endif /* __SOLVER_MISC_GETOPT_H__ */
