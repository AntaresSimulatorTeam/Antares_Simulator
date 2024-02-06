#pragma once

#include <map>
#include <string>

#include <antares/writer/i_writer.h>

// ignore unused parameters warnings from ortools
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include "ortools/linear_solver/linear_solver.h"
#pragma GCC diagnostic pop

using namespace operations_research;

/*!
 *  \brief Return list of available ortools solver name on our side
 *
 *  \return List of available ortools solver name
 */
std::list<std::string> getAvailableOrtoolsSolverName();

/*!
 *  \brief Return a single string containing all solvers available, separated by a ", " and ending with a ".".
 *
 */
std::string availableOrToolsSolversString();

/*!
 *  \brief Create a MPSolver with correct linear or mixed variant
 *
 *  \return MPSolver
 */
MPSolver *MPSolverFactory(bool isMip, const std::string &solverName);

class OrtoolsUtils
{
public:
    struct SolverNames
    {
        std::string LPSolverName, MIPSolverName;
    };
    static const std::map<std::string, struct SolverNames> solverMap;
};