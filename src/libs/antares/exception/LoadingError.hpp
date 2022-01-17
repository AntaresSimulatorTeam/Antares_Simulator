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
#include <yuni/yuni.h>
#include <yuni/string.h>

namespace Antares
{
namespace Error
{
class StudyFolderDoesNotExist : public std::runtime_error
{
public:
    explicit StudyFolderDoesNotExist(const Yuni::String& folder);
};

class ReadingStudy : public std::runtime_error
{
public:
    explicit ReadingStudy();
};

class NoAreas : public std::runtime_error
{
public:
    explicit NoAreas();
};

class InvalidFileName : public std::runtime_error
{
public:
    explicit InvalidFileName();
};

class RuntimeInfoInitialization : public std::runtime_error
{
public:
    explicit RuntimeInfoInitialization();
};

class WritingPID : public std::runtime_error
{
public:
    explicit WritingPID(const Yuni::String& file);
};

class InvalidNumberOfMCYears : public std::runtime_error
{
public:
    explicit InvalidNumberOfMCYears(uint nbYears);
};

class IncompatibleParallelOptions : public std::runtime_error
{
public:
    explicit IncompatibleParallelOptions();
};

class InvalidOptimizationRange : public std::runtime_error
{
public:
    explicit InvalidOptimizationRange();
};

class InvalidSimulationMode : public std::runtime_error
{
public:
    explicit InvalidSimulationMode();
};

class InvalidSolver : public std::runtime_error
{
public:
    explicit InvalidSolver(const std::string& solver);
};

class InvalidStudy : public std::runtime_error
{
public:
    explicit InvalidStudy(const Yuni::String& study);
};

class NoStudyProvided : public std::runtime_error
{
public:
    explicit NoStudyProvided();
};

class InvalidVersion : public std::runtime_error
{
public:
    explicit InvalidVersion(const char* version, const char* latest);
};
} // namespace Error
} // namespace Antares
