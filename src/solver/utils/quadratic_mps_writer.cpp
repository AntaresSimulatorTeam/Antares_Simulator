// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include "antares/solver/utils/quadratic_mps_writer.h"

#include <cmath>
#include <cctype>
#include <fmt/format.h>
#include <limits>
#include <stdexcept>
#include <string_view>
#include <unordered_set>
#include <vector>

#include <antares/logs/logs.h>
#include "antares/solver/optimisation/opt_structure_probleme_a_resoudre.h"

#include <pi_constantes_externes.h>

namespace
{
constexpr std::string_view pad = "    ";
constexpr double eps = 1e-12;

bool isEqual(const double a, const double b, const double tolerance = eps)
{
    if (std::isinf(a) && std::isinf(b))
    {
        return (a > 0) == (b > 0);
    }
    if (std::isinf(a) || std::isinf(b))
    {
        return false;
    }
    return std::abs(a - b) < tolerance;
}

bool isNotEqual(const double a, const double b, const double tolerance = eps)
{
    return !isEqual(a, b, tolerance);
}

std::string sanitizeName(std::string name, const char prefix)
{
    for (char& c: name)
    {
        const unsigned char uc = static_cast<unsigned char>(c);
        if (std::isalnum(uc) || c == '_' || c == '.' || c == '-')
        {
            continue;
        }
        c = '_';
    }

    if (name.empty())
    {
        name = std::string(1, prefix);
    }

    const unsigned char first = static_cast<unsigned char>(name.front());
    if (!std::isalpha(first) && name.front() != '_')
    {
        name.insert(name.begin(), prefix);
    }

    return name;
}

std::vector<std::string> buildExportableNames(const std::vector<std::string>& originalNames,
                                              const int count,
                                              const char prefix,
                                              const bool keepNames)
{
    std::vector<std::string> names;
    names.reserve(count);
    std::unordered_set<std::string> alreadyUsed;

    for (int i = 0; i < count; ++i)
    {
        std::string baseName;
        if (keepNames && i < static_cast<int>(originalNames.size()))
        {
            baseName = originalNames[i];
        }
        if (baseName.empty())
        {
            baseName = fmt::format("{}{}", prefix, i);
        }
        baseName = sanitizeName(std::move(baseName), prefix);

        std::string uniqueName = baseName;
        int suffix = 1;
        while (!alreadyUsed.insert(uniqueName).second)
        {
            uniqueName = fmt::format("{}_{}", baseName, suffix++);
        }
        names.push_back(std::move(uniqueName));
    }

    return names;
}

struct MpsColumnEntry
{
    int row;
    double value;
};

using MpsColumn = std::vector<MpsColumnEntry>;

std::vector<MpsColumn> buildColumns(const PROBLEME_ANTARES_A_RESOUDRE& problem)
{
    std::vector<MpsColumn> columns(problem.NombreDeVariables);
    for (int row = 0; row < problem.NombreDeContraintes; ++row)
    {
        const int start = problem.IndicesDebutDeLigne[row];
        const int end = start + problem.NombreDeTermesDesLignes[row];
        for (int i = start; i < end; ++i)
        {
            const int variable = problem.IndicesColonnes[i];
            columns[variable].push_back({row, problem.CoefficientsDeLaMatriceDesContraintes[i]});
        }
    }
    return columns;
}

bool isIntegerVariable(const PROBLEME_ANTARES_A_RESOUDRE& problem, const int variable)
{
    return variable < static_cast<int>(problem.VariablesEntieres.size())
           && problem.VariablesEntieres[variable];
}

bool isBooleanVariable(const PROBLEME_ANTARES_A_RESOUDRE& problem, const int variable)
{
    if (!isIntegerVariable(problem, variable))
    {
        return false;
    }
    return isEqual(problem.Xmin[variable], 0.0) && isEqual(problem.Xmax[variable], 1.0);
}

void writeHeader(std::string& mps, const PROBLEME_ANTARES_A_RESOUDRE& problem)
{
    fmt::format_to(std::back_inserter(mps), "* Antares Simulator QuadraticMPSWriter\n");
    fmt::format_to(std::back_inserter(mps),
                   "* Number of variables: {}\n",
                   problem.NombreDeVariables);
    fmt::format_to(std::back_inserter(mps),
                   "* Number of constraints: {}\n",
                   problem.NombreDeContraintes);
}

void writeName(std::string& mps, const std::string& filename)
{
    fmt::format_to(std::back_inserter(mps), "NAME {}\n", filename);
}

void writeRows(std::string& mps,
               const PROBLEME_ANTARES_A_RESOUDRE& problem,
               const std::vector<std::string>& constraintNames)
{
    mps += "ROWS\n";
    fmt::format_to(std::back_inserter(mps), "{}N  OBJ\n", pad);
    for (int i = 0; i < problem.NombreDeContraintes; ++i)
    {
        char sense = 'N';
        switch (problem.Sens[i])
        {
        case '=':
            sense = 'E';
            break;
        case '<':
            sense = 'L';
            break;
        case '>':
            sense = 'G';
            break;
        default:
            throw std::invalid_argument("Unknown constraint sense: "
                                        + problem.Sens.substr(i, 1));
        }
        fmt::format_to(std::back_inserter(mps), "{}{}  {}\n", pad, sense, constraintNames[i]);
    }
}

void writeColumns(std::string& mps,
                  const PROBLEME_ANTARES_A_RESOUDRE& problem,
                  const std::vector<std::string>& variableNames,
                  const std::vector<std::string>& constraintNames)
{
    mps += "COLUMNS\n";
    const auto columns = buildColumns(problem);

    for (int i = 0; i < problem.NombreDeVariables; ++i)
    {
        if (isIntegerVariable(problem, i))
        {
            fmt::format_to(std::back_inserter(mps),
                           "{}MARK{:010}    'MARKER'                 'INTORG'\n",
                           pad,
                           i);
        }

        if (isNotEqual(problem.CoutLineaire[i], 0.0))
        {
            fmt::format_to(std::back_inserter(mps),
                           "{}{}  OBJ  {}\n",
                           pad,
                           variableNames[i],
                           problem.CoutLineaire[i]);
        }

        for (const auto& [row, value]: columns[i])
        {
            fmt::format_to(std::back_inserter(mps),
                           "{}{}  {}  {}\n",
                           pad,
                           variableNames[i],
                           constraintNames[row],
                           value);
        }

        if (isIntegerVariable(problem, i))
        {
            fmt::format_to(std::back_inserter(mps),
                           "{}MARK{:010}    'MARKER'                 'INTEND'\n",
                           pad,
                           i);
        }
    }
}

void writeRhs(std::string& mps,
              const PROBLEME_ANTARES_A_RESOUDRE& problem,
              const std::vector<std::string>& constraintNames)
{
    mps += "RHS\n";
    for (int i = 0; i < problem.NombreDeContraintes; ++i)
    {
        if (isNotEqual(problem.SecondMembre[i], 0.0))
        {
            fmt::format_to(std::back_inserter(mps),
                           "{}RHS1  {}  {}\n",
                           pad,
                           constraintNames[i],
                           problem.SecondMembre[i]);
        }
    }
}

void writeRanges(std::string& mps)
{
    mps += "RANGES\n";
}

void writeBounds(std::string& mps,
                 const PROBLEME_ANTARES_A_RESOUDRE& problem,
                 const std::vector<std::string>& variableNames)
{
    constexpr double infinity = std::numeric_limits<double>::infinity();

    mps += "BOUNDS\n";
    for (int i = 0; i < problem.NombreDeVariables; ++i)
    {
        const auto& variableName = variableNames[i];
        const bool isInteger = isIntegerVariable(problem, i);
        const double lb = problem.Xmin[i];
        const double ub = problem.Xmax[i];
        const std::string boundName = "BND1";

        switch (problem.TypeDeVariable[i])
        {
        case VARIABLE_FIXE:
            fmt::format_to(std::back_inserter(mps),
                           "{}FX {} {} {}\n",
                           pad,
                           boundName,
                           variableName,
                           lb);
            break;

        case VARIABLE_BORNEE_DES_DEUX_COTES:
            if (isEqual(lb, ub))
            {
                fmt::format_to(std::back_inserter(mps),
                               "{}FX {} {} {}\n",
                               pad,
                               boundName,
                               variableName,
                               lb);
                break;
            }
            if (isBooleanVariable(problem, i))
            {
                fmt::format_to(std::back_inserter(mps), "{}BV {} {}\n", pad, boundName, variableName);
                break;
            }
            if (!isEqual(lb, 0.0))
            {
                fmt::format_to(std::back_inserter(mps),
                               "{}{} {} {} {}\n",
                               pad,
                               isInteger ? "LI" : "LO",
                               boundName,
                               variableName,
                               lb);
            }
            if (!isEqual(ub, infinity))
            {
                fmt::format_to(std::back_inserter(mps),
                               "{}{} {} {} {}\n",
                               pad,
                               isInteger ? "UI" : "UP",
                               boundName,
                               variableName,
                               ub);
            }
            break;

        case VARIABLE_BORNEE_INFERIEUREMENT:
            if (isInteger)
            {
                fmt::format_to(std::back_inserter(mps),
                               "{}LI {} {} {}\n",
                               pad,
                               boundName,
                               variableName,
                               lb);
            }
            else if (!isEqual(lb, 0.0))
            {
                fmt::format_to(std::back_inserter(mps),
                               "{}LO {} {} {}\n",
                               pad,
                               boundName,
                               variableName,
                               lb);
            }
            break;

        case VARIABLE_BORNEE_SUPERIEUREMENT:
            fmt::format_to(std::back_inserter(mps), "{}MI {} {}\n", pad, boundName, variableName);
            fmt::format_to(std::back_inserter(mps),
                           "{}{} {} {} {}\n",
                           pad,
                           isInteger ? "UI" : "UP",
                           boundName,
                           variableName,
                           ub);
            break;

        case VARIABLE_NON_BORNEE:
            fmt::format_to(std::back_inserter(mps), "{}FR {} {}\n", pad, boundName, variableName);
            break;

        default:
            throw std::invalid_argument("Unknown variable type: "
                                        + std::to_string(problem.TypeDeVariable[i]));
        }
    }
}

void writeQuadraticMatrix(std::string& mps,
                          const PROBLEME_ANTARES_A_RESOUDRE& problem,
                          const std::vector<std::string>& variableNames)
{
    mps += "QMATRIX\n";
    for (int i = 0; i < problem.NombreDeVariables; ++i)
    {
        if (isEqual(problem.CoutQuadratique[i], 0.0))
        {
            continue;
        }
        fmt::format_to(std::back_inserter(mps),
                       "{}{}  {}  {}\n",
                       pad,
                       variableNames[i],
                       variableNames[i],
                       2.0 * problem.CoutQuadratique[i]);
    }
}

void writeEnd(std::string& mps)
{
    mps += "ENDATA\n";
}

std::string buildQuadraticMps(const PROBLEME_ANTARES_A_RESOUDRE& problem,
                              const std::string& filename,
                              const bool keepNames)
{
    if (problem.NombreDeVariables == 0)
    {
        Antares::logs.warning() << "Quadratic problem '" << filename
                                 << "' contains no variables.";
        return {};
    }

    const auto variableNames = buildExportableNames(problem.NomDesVariables,
                                                    problem.NombreDeVariables,
                                                    'x',
                                                    keepNames);
    const auto constraintNames = buildExportableNames(problem.NomDesContraintes,
                                                      problem.NombreDeContraintes,
                                                      'r',
                                                      keepNames);

    std::string mps;
    writeHeader(mps, problem);
    writeName(mps, filename);
    writeRows(mps, problem, constraintNames);
    writeColumns(mps, problem, variableNames, constraintNames);
    writeRhs(mps, problem, constraintNames);
    writeRanges(mps);
    writeBounds(mps, problem, variableNames);
    writeQuadraticMatrix(mps, problem, variableNames);
    writeEnd(mps);
    return mps;
}

} // namespace

void writeQuadraticMps(const PROBLEME_ANTARES_A_RESOUDRE& problem,
                       Antares::Solver::IResultWriter& writer,
                       const std::string& filename,
                       const bool keepNames)
{
    Antares::logs.info() << "Writing quadratic MPS File: `" << filename << "'";
    std::string mps = buildQuadraticMps(problem, filename, keepNames);
    writer.addEntryFromBuffer(filename, mps);
}
