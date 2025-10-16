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
#include "antares/exception/LoadingError.hpp"

#include <sstream>

namespace Antares::Error
{
StudyFolderDoesNotExist::StudyFolderDoesNotExist(const std::string& folder):
    LoadingError(std::string("Study folder ") + folder + " does not exist.")
{
}

StudyFolderContainsNonASCIIchars::StudyFolderContainsNonASCIIchars(const std::string& folder):
    LoadingError(std::string("Study folder contains non ASCII chars : ") + folder)
{
}

NoAreas::NoAreas():
    LoadingError("No area found. A valid study contains contains at least one.")
{
}

Duplicates::Duplicates():
    LoadingError("One or more duplicates found.")
{
}

InvalidFileName::InvalidFileName():
    LoadingError("Invalid file names detected.")
{
}

RuntimeInfoInitialization::RuntimeInfoInitialization():
    LoadingError("Error initializing runtime infos.")
{
}

WritingPID::WritingPID(const std::string& filePath):
    LoadingError(std::string("Impossible to write pid file ") + filePath)
{
}

InvalidOptimizationRange::InvalidOptimizationRange():
    LoadingError("Invalid command line value for --optimization-range ('day' or 'week' expected)")
{
}

InvalidSimulationMode::InvalidSimulationMode():
    LoadingError("Only one simulation mode is allowed: --expansion, --economy, --adequacy")
{
}

static std::string InvalidSolverHelper(const std::string& solver,
                                       const std::string& availableSolvers)
{
    std::ostringstream message;
    message << "Can't use solver '" << solver
            << "' : it does not match any available OR-Tools solver. Possible choices are "
            << availableSolvers;
    return message.str();
}

InvalidSolver::InvalidSolver(const std::string& solver, const std::string& availableSolvers):
    LoadingError(InvalidSolverHelper(solver, availableSolvers))
{
}

IncompatibleLinearSolverParameters::IncompatibleLinearSolverParameters():
    LoadingError("You can't supply parameters for one particular and both optimizations")
{
}

static std::string InvalidSolverParameterMessage(const std::string& solver,
                                                 const std::string& parameters)
{
    std::ostringstream message;
    message << "Specific parameters '" << parameters
            << "' are not valid or not supported for solver " << solver;
    return message.str();
}

InvalidSolverSpecificParameters::InvalidSolverSpecificParameters(const std::string& solver,
                                                                 const std::string& parameters):
    LoadingError(InvalidSolverParameterMessage(solver, parameters))
{
}

InvalidStudy::InvalidStudy(const std::string& study):
    LoadingError(std::string("The folder `") + study + "` does not seem to be a valid study")
{
}

InvalidVersion::InvalidVersion(const std::string& version, const std::string& latest):
    LoadingError(std::string("Invalid version for the study : found `") + version
                 + "`, expected <=`" + latest + '`')
{
}

NoStudyProvided::NoStudyProvided():
    LoadingError("A study folder is required. Use '--help' for more information")
{
}

IncompatibleParallelOptions::IncompatibleParallelOptions():
    LoadingError("Options --parallel and --force-parallel are incompatible")
{
}

IncompatibleMILPOrtoolsSolver::IncompatibleMILPOrtoolsSolver():
    LoadingError("'milp' mode does not work with OR-Tools using Sirius solver")
{
}

UseMILPsolverWithWrongOptions::UseMILPsolverWithWrongOptions():
    LoadingError("'milp' solver cannot be used with options for optimization 1 or 2")
{
}

IncompatibleOptRangeHydroPricing::IncompatibleOptRangeHydroPricing():
    LoadingError("Simplex optimization range and hydro pricing mode : values are not compatible ")
{
}

IncompatibleOptRangeUCMode::IncompatibleOptRangeUCMode():
    LoadingError("Simplexe optimization range and unit commitment mode : values are not compatible")
{
}

CommandLineArguments::CommandLineArguments(unsigned int errors):
    LoadingError("Invalid command-line arguments provided : " + std::to_string(errors)
                 + " error(s) found")
{
}

IncompatibleOutputOptions::IncompatibleOutputOptions(const std::string& text):
    LoadingError(text)
{
}

IncompatibleCO2CostColumns::IncompatibleCO2CostColumns():
    LoadingError(
      "Number of columns for CO2 Cost can be one or same as number of TS in Availability")
{
}

IncompatibleFuelCostColumns::IncompatibleFuelCostColumns():
    LoadingError(
      "Number of columns for Fuel Cost can be one or same as number of TS in Availability")
{
}

} // namespace Antares::Error
