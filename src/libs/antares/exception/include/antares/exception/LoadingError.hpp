/*
 * Copyright 2007-2025, RTE (https://www.rte-france.com)
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

#include <list>
#include <map>
#include <stdexcept>

#include <yuni/yuni.h>
#include <yuni/string.h>

#include "antares/study/fwd.h"

namespace Antares::Error
{
class LoadingError: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class StudyFolderDoesNotExist: public LoadingError
{
public:
    explicit StudyFolderDoesNotExist(const std::string& folder);
};

class StudyFolderContainsNonASCIIchars: public LoadingError
{
public:
    explicit StudyFolderContainsNonASCIIchars(const std::string& folder);
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

class Duplicates: public LoadingError
{
public:
    Duplicates();
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
    explicit WritingPID(const std::string& filePath);
};

class IncompatibleParallelOptions: public LoadingError
{
public:
    IncompatibleParallelOptions();
};

class IncompatibleMILPOrtoolsSolver: public LoadingError
{
public:
    IncompatibleMILPOrtoolsSolver();
};

class UseMILPsolverWithWrongOptions: public LoadingError
{
public:
    UseMILPsolverWithWrongOptions();
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

class IncompatibleLinearSolverParameters: public LoadingError
{
public:
    IncompatibleLinearSolverParameters();
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

} // namespace Antares::Error
