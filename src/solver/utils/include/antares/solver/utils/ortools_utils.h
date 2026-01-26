// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <map>
#include <string>

#include <antares/writer/i_writer.h>

// ignore unused parameters warnings from ortools
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

#include "ortools_wrapper.h"

namespace operations_research::math_opt
{
enum class SolverType;
}

using namespace operations_research;

// TODO use Objective().Value() instead
// This is a temporary workaround for Windows
double getObjectiveValue(const MPSolver* solver);

/*!
 *  \brief Returns a comma-seperated-list of available ortools linear solver names on our side
 *
 *  \return Comma-seperated-list of available ortools linear solver names
 */
std::string toString(const std::list<std::string>& solverList);

/*!
 *  \brief Returns a list of available ortools linear solver names on our side
 *
 *  \return List of available ortools linear solver names
 */
std::list<std::string> availableLinearSolversList();

/*!
 *  \brief Returns a list of available ortools quadratic solver names on our side
 *
 *  \return List of available ortools quadratic solver names
 */
std::list<std::string> availableQuadraticSolversList();

/*!
 *  \brief Check if a linear solver is available
 */
bool isLinearSolverAvailable(const std::string& solverName);

/*!
 *  \brief Create a MPSolver with correct linear or mixed variant
 *
 *  \return MPSolver
 */
MPSolver* MPSolverFactory(const bool isMip, const std::string& solverName);

std::string generateTempPath(const std::string& filename);
void removeTemporaryFile(const std::string& tmpPath);

class OrtoolsUtils final
{
public:
    struct SolverNames
    {
        std::optional<std::string> LPSolverName, MIPSolverName;
    };

    static const std::map<std::string, SolverNames> mpSolverMap;
    static const std::map<std::string, math_opt::SolverType> mathoptSolverMap;
};
