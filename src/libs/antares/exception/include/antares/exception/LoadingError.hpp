/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
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
**
** SPDX-License-Identifier: MPL-2.0
*/

#pragma once

#include <list>
#include <map>
#include <stdexcept>

#include <yuni/yuni.h>
#include <yuni/string.h>

#include "antares/study/fwd.h"

namespace Antares
{
namespace Error
{
class LoadingError: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class StudyFolderDoesNotExist: public LoadingError
{
public:
    explicit StudyFolderDoesNotExist(const Yuni::String& folder);
};

class ReadingStudy: public LoadingError
{
public:
    ReadingStudy();
};

class NoAreas: public LoadingError
{
public:
    NoAreas();
};

class InvalidFileName: public LoadingError
{
public:
    InvalidFileName();
};

class RuntimeInfoInitialization: public LoadingError
{
public:
    RuntimeInfoInitialization();
};

class WritingPID: public LoadingError
{
public:
    explicit WritingPID(const Yuni::String& file);
};

class IncompatibleParallelOptions: public LoadingError
{
public:
    IncompatibleParallelOptions();
};

class IncompatibleMILPWithoutOrtools: public LoadingError
{
public:
    IncompatibleMILPWithoutOrtools();
};

class IncompatibleMILPOrtoolsSolver: public LoadingError
{
public:
    IncompatibleMILPOrtoolsSolver();
};

class IncompatibleOptRangeHydroPricing: public LoadingError
{
public:
    IncompatibleOptRangeHydroPricing();
};

class IncompatibleOptRangeUCMode: public LoadingError
{
public:
    IncompatibleOptRangeUCMode();
};

class InvalidOptimizationRange: public LoadingError
{
public:
    InvalidOptimizationRange();
};

class InvalidSimulationMode: public LoadingError
{
public:
    InvalidSimulationMode();
};

class InvalidSolver: public LoadingError
{
public:
    explicit InvalidSolver(const std::string& solver, const std::string& availableSolverList);
};

class InvalidSolverSpecificParameters: public LoadingError
{
public:
    explicit InvalidSolverSpecificParameters(const std::string& solver,
                                             const std::string& specificParameters);
};

class InvalidStudy: public LoadingError
{
public:
    explicit InvalidStudy(const std::string& study);
};

class NoStudyProvided: public LoadingError
{
public:
    NoStudyProvided();
};

class InvalidVersion: public LoadingError
{
public:
    InvalidVersion(const std::string& version, const std::string& latest);
};

class IncompatibleDailyOptHeuristicForArea: public LoadingError
{
public:
    explicit IncompatibleDailyOptHeuristicForArea(const Antares::Data::AreaName& name);
};

class InvalidParametersForThermalClusters: public LoadingError
{
public:
    explicit InvalidParametersForThermalClusters(const std::map<int, Yuni::String>& clusterNames);

private:
    std::string buildMessage(const std::map<int, Yuni::String>& clusterNames) const;
};

class CommandLineArguments: public LoadingError
{
public:
    explicit CommandLineArguments(uint errors);
};

class IncompatibleSimulationModeForAdqPatch: public LoadingError
{
public:
    IncompatibleSimulationModeForAdqPatch();
};

class NoAreaInsideAdqPatchMode: public LoadingError
{
public:
    NoAreaInsideAdqPatchMode();
};

class IncompatibleHurdleCostCSR: public LoadingError
{
public:
    IncompatibleHurdleCostCSR();
};

class IncompatibleOutputOptions: public LoadingError
{
public:
    explicit IncompatibleOutputOptions(const std::string& text);
};

class IncompatibleCO2CostColumns: public LoadingError
{
public:
    IncompatibleCO2CostColumns();
};

class IncompatibleFuelCostColumns: public LoadingError
{
public:
    IncompatibleFuelCostColumns();
};

} // namespace Error
} // namespace Antares
