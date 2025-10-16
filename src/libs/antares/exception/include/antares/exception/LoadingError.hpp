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

#include <map>
#include <stdexcept>

namespace Antares::Error
{
class LoadingError: public std::runtime_error
{
public:
    using std::runtime_error::runtime_error;
};

class StudyFolderDoesNotExist final: public LoadingError
{
public:
    explicit StudyFolderDoesNotExist(const std::string& folder);
};

class StudyFolderContainsNonASCIIchars final: public LoadingError
{
public:
    explicit StudyFolderContainsNonASCIIchars(const std::string& folder);
};

class NoAreas final: public LoadingError
{
public:
    NoAreas();
};

class Duplicates final: public LoadingError
{
public:
    Duplicates();
};

class InvalidFileName final: public LoadingError
{
public:
    InvalidFileName();
};

class RuntimeInfoInitialization final: public LoadingError
{
public:
    RuntimeInfoInitialization();
};

class WritingPID final: public LoadingError
{
public:
    explicit WritingPID(const std::string& filePath);
};

class IncompatibleParallelOptions final: public LoadingError
{
public:
    IncompatibleParallelOptions();
};

class IncompatibleMILPOrtoolsSolver final: public LoadingError
{
public:
    IncompatibleMILPOrtoolsSolver();
};

class UseMILPsolverWithWrongOptions final: public LoadingError
{
public:
    UseMILPsolverWithWrongOptions();
};

class IncompatibleOptRangeHydroPricing final: public LoadingError
{
public:
    IncompatibleOptRangeHydroPricing();
};

class IncompatibleOptRangeUCMode final: public LoadingError
{
public:
    IncompatibleOptRangeUCMode();
};

class InvalidOptimizationRange final: public LoadingError
{
public:
    InvalidOptimizationRange();
};

class InvalidSimulationMode final: public LoadingError
{
public:
    InvalidSimulationMode();
};

class InvalidSolver final: public LoadingError
{
public:
    explicit InvalidSolver(const std::string& solver, const std::string& availableSolverList);
};

class InvalidSolverSpecificParameters final: public LoadingError
{
public:
    explicit InvalidSolverSpecificParameters(const std::string& solver,
                                             const std::string& specificParameters);
};

class IncompatibleLinearSolverParameters final: public LoadingError
{
public:
    IncompatibleLinearSolverParameters();
};

class InvalidStudy final: public LoadingError
{
public:
    explicit InvalidStudy(const std::string& study);
};

class NoStudyProvided final: public LoadingError
{
public:
    NoStudyProvided();
};

class InvalidVersion final: public LoadingError
{
public:
    InvalidVersion(const std::string& version, const std::string& latest);
};

class CommandLineArguments final: public LoadingError
{
public:
    explicit CommandLineArguments(unsigned int errors);
};

class IncompatibleOutputOptions final: public LoadingError
{
public:
    explicit IncompatibleOutputOptions(const std::string& text);
};

class IncompatibleCO2CostColumns final: public LoadingError
{
public:
    IncompatibleCO2CostColumns();
};

class IncompatibleFuelCostColumns final: public LoadingError
{
public:
    IncompatibleFuelCostColumns();
};

} // namespace Antares::Error
