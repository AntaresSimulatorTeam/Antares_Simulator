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

#pragma once

#include <stdexcept>
#include <map>

#include <yuni/yuni.h>
#include <yuni/string.h>
#include <antares/study/fwd.h>

namespace Antares
{
namespace Error
{
class LoadingError : public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class StudyFolderDoesNotExist : public LoadingError
{
public:
    explicit StudyFolderDoesNotExist(const Yuni::String& folder);
};

class ReadingStudy : public LoadingError
{
public:
    explicit ReadingStudy();
};

class NoAreas : public LoadingError
{
public:
    explicit NoAreas();
};

class InvalidFileName : public LoadingError
{
public:
    explicit InvalidFileName();
};

class RuntimeInfoInitialization : public LoadingError
{
public:
    explicit RuntimeInfoInitialization();
};

class WritingPID : public LoadingError
{
public:
    explicit WritingPID(const Yuni::String& file);
};

class InvalidNumberOfMCYears : public LoadingError
{
public:
    explicit InvalidNumberOfMCYears(uint nbYears);
};

class IncompatibleParallelOptions : public LoadingError
{
public:
    explicit IncompatibleParallelOptions();
};

class IncompatibleOptRangeHydroPricing : public LoadingError
{
public:
    explicit IncompatibleOptRangeHydroPricing();
};

class IncompatibleOptRangeUCMode : public LoadingError
{
public:
    explicit IncompatibleOptRangeUCMode();
};

class InvalidOptimizationRange : public LoadingError
{
public:
    explicit InvalidOptimizationRange();
};

class InvalidSimulationMode : public LoadingError
{
public:
    explicit InvalidSimulationMode();
};

class InvalidSolver : public LoadingError
{
public:
    explicit InvalidSolver(const std::string& solver);
};

class InvalidStudy : public LoadingError
{
public:
    explicit InvalidStudy(const Yuni::String& study);
};

class NoStudyProvided : public LoadingError
{
public:
    explicit NoStudyProvided();
};

class InvalidVersion : public LoadingError
{
public:
    explicit InvalidVersion(const char* version, const char* latest);
};

class IncompatibleDailyOptHeuristicForArea : public LoadingError
{
public:
    explicit IncompatibleDailyOptHeuristicForArea(const Antares::Data::AreaName& name);
};

class WritingProgressFile : public LoadingError
{
public:
    explicit WritingProgressFile(const Yuni::String& study);
};

class InvalidParametersForThermalClusters : public LoadingError
{
public:
    explicit InvalidParametersForThermalClusters(const std::map<int, Yuni::String>& clusterNames);
};

class CommandLineArguments : public LoadingError
{
public:
    explicit CommandLineArguments(uint errors);
};
} // namespace Error
} // namespace Antares
