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
#ifndef __SOLVER_MISC_GETOPT_H__
#define __SOLVER_MISC_GETOPT_H__

#include<memory>
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
    int ignoreWarningsErrors;
    //! Ignore constraints
    bool ignoreConstraints;
    //!
    bool tsGeneratorsOnly;
    //! True to disable the writing in the output folder
    bool noOutput;
    //! Progression
    bool displayProgression;
    //! Swap folder
    Yuni::String swap;

    Yuni::String PID;

    void checkAndSetStudyFolder(Yuni::String folder);
    void reset();
}; // class Settings

/*!
** \brief Interprete options given in command line
*/
std::shared_ptr<Yuni::GetOpt::Parser> CreateParser(Settings& settings, Antares::Data::StudyLoadOptions& options);

void checkAndCorrectSettingsAndOptions(Settings& settings, Data::StudyLoadOptions& options);

void checkOrtoolsSolver(Data::StudyLoadOptions& options);
#endif /* __SOLVER_MISC_GETOPT_H__ */
