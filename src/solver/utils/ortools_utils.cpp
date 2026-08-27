// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <filesystem>
#include <optional>
#include <ortools/math_opt/cpp/parameters.h>
#include <yuni/io/directory.h>

#include <boost/algorithm/string/join.hpp>

#include <antares/enums/Enum.hpp>
#include <antares/logs/logs.h>
#include <antares/solver/utils/ortools_utils.h>

using namespace operations_research;

// TODO use Objective().Value() instead
// This is a temporary workaround for Windows
double getObjectiveValue(const MPSolver* solver)
{
    double ret = 0;
    const auto& objective = solver->Objective();
    for (const auto* variable: solver->variables())
    {
        ret += variable->solution_value() * objective.GetCoefficient(variable);
    }
    ret += objective.offset();
    return ret;
}

std::string generateTempPath(const std::string& filename)
{
    namespace fs = std::filesystem;
    std::ostringstream tmpPath;
    tmpPath << fs::temp_directory_path().string() << Yuni::IO::SeparatorAsString << filename;
    return tmpPath.str();
}

void removeTemporaryFile(const std::string& tmpPath)
{
    namespace fs = std::filesystem;
    bool ret = false;
    try
    {
        ret = fs::remove(tmpPath);
    }
    catch (fs::filesystem_error& e)
    {
        Antares::logs.error() << e.what();
    }
    if (!ret)
    {
        Antares::logs.warning() << "Could not remove temporary file " << tmpPath;
    }
}

const std::map<std::string, struct OrtoolsUtils::SolverNames> OrtoolsUtils::mpSolverMap = {
  {"xpress", {"xpress_lp", "xpress"}},
  {"sirius", {"sirius_lp", std::nullopt}}, // only allowed in LP (MIP only supports binaries)
  {"coin", {"clp", "cbc"}},
  {"glpk", {"glpk_lp", "glpk"}},
  {"scip", {std::nullopt, "scip"}}, // SCIP only supports MIPs
  {"highs", {"highs_lp", "highs"}},
  {"pdlp", {"pdlp", std::nullopt}}, // PDLP only supports LPs
  {"gurobi", {"gurobi_lp", "gurobi"}}};

const std::map<std::string, math_opt::SolverType> OrtoolsUtils::mathoptSolverMap = {
  {"pdlp", math_opt::SolverType::kPdlp},
  {"scip", math_opt::SolverType::kGscip},
  {"xpress", math_opt::SolverType::kXpress},
  {"gurobi", math_opt::SolverType::kGurobi}};

std::list<std::string> availableLinearSolversList()
{
    std::list<std::string> result;

    for (const auto& solverName: OrtoolsUtils::mpSolverMap)
    {
        MPSolver::OptimizationProblemType solverType;
        if (solverName.second.LPSolverName.has_value())
        {
            MPSolver::ParseSolverType(solverName.second.LPSolverName.value(), &solverType);
        }
        else
        {
            MPSolver::ParseSolverType(solverName.second.MIPSolverName.value(), &solverType);
        }

        if (MPSolver::SupportsProblemType(solverType))
        {
            result.push_back(solverName.first);
        }
    }
    return result;
}

bool isLinearSolverAvailable(const std::string& solverName)
{
    auto it = OrtoolsUtils::mpSolverMap.find(solverName);
    if (it == OrtoolsUtils::mpSolverMap.end())
    {
        return false;
    }

    MPSolver::OptimizationProblemType solverType;
    if (it->second.LPSolverName.has_value())
    {
        MPSolver::ParseSolverType(it->second.LPSolverName.value(), &solverType);
    }
    else
    {
        MPSolver::ParseSolverType(it->second.MIPSolverName.value(), &solverType);
    }

    return MPSolver::SupportsProblemType(solverType);
}

std::list<std::string> availableQuadraticSolversList()
{
    std::list<std::string> result;
    // Sirius is supported, but not through mathopt
    result.push_back("sirius");
    for (const auto& solverName: OrtoolsUtils::mathoptSolverMap)
    {
        result.push_back(solverName.first);
    }
    return result;
}

std::string toString(const std::list<std::string>& solverList)
{
    return boost::algorithm::join(solverList, ",") + ".";
}

static std::optional<std::string> translateSolverName(const std::string& solverName, bool isMip)
{
    if (!OrtoolsUtils::mpSolverMap.contains(solverName))
    {
        return {};
    }
    auto names = OrtoolsUtils::mpSolverMap.at(solverName);
    if (isMip)
    {
        return names.MIPSolverName;
    }
    else
    {
        return names.LPSolverName;
    }
}

MPSolver* MPSolverFactory(const bool isMip, const std::string& solverName)
{
    const std::string notFound = "Solver " + solverName
                                 + " not supported for linear problems optimization.";
    const std::invalid_argument except(notFound);

    auto internalSolverName = translateSolverName(solverName, isMip);
    if (!internalSolverName.has_value())
    {
        throw std::invalid_argument("Solver " + solverName
                                    + " is not supported by Antares or does not support "
                                    + (isMip ? "MIP" : "LP") + " problems.");
    }

    MPSolver* solver = MPSolver::CreateSolver(*internalSolverName);
    if (!solver)
    {
        throw std::invalid_argument("Solver " + solverName + " (" + *internalSolverName
                                    + ") could not be loaded by OR-Tools MPSolver.");
    }
    return solver;
}
