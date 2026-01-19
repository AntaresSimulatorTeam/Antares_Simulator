/*
** Copyright 2007-2025, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHout_ ANY WARRANTY; without_ even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/io/outputs/MPSGenerator.h"

#include <cmath>
#include <fmt/format.h>
#include <fstream>

#include <antares/logs/logs.h>
#include "antares/io/outputs/ExportableName.h"

using namespace Antares::Optimisation::LinearProblemApi;
static constexpr std::string_view pad = "    ";
static constexpr double EPS = 1e-12;

bool isEqual(const double& a, const double& b, const double& eps = EPS)
{
    if (std::isinf(a) && std::isinf(b))
    {
        return (a > 0) == (b > 0); // Both +inf or both -inf
    }

    if (std::isinf(a) || std::isinf(b))
    {
        return false; // One is inf, other isn't
    }
    return std::abs(a - b) < eps;
}

bool isNotEqual(const double& a, const double& b, const double& eps = EPS)
{
    return !isEqual(a, b, eps);
}

namespace Antares::IO::Outputs
{
char ConstraintSense(const double& lb, const double& ub, const double& infinity)
{
    const bool lbIsMinusInfinity = isEqual(lb, -infinity);
    const bool ubIsPlusInfinity = isEqual(ub, infinity);
    if (lbIsMinusInfinity && ubIsPlusInfinity)
    {
        return 'N';
    }
    else if (isEqual(lb, ub))
    {
        return 'E';
    }
    else if (lbIsMinusInfinity)
    {
        return 'L';
    }
    else
    {
        return 'G';
    }
}

double ConstraintRhs(const double& lb, const double& ub, const double& infinity)
{
    if (const auto sense = ConstraintSense(lb, ub, infinity); sense == 'L')
    {
        return ub;
    }
    else //(sense == 'G' || sense == 'N' || sense == 'E'
    {
        return lb;
    }
}

bool IsBoolean(const IMipVariable& variable)
{
    const bool lbIsZero = isEqual(variable.getLb(), 0.0);
    const bool ubIsOne = isEqual(variable.getUb(), 1.0);
    return variable.isInteger() && lbIsZero && ubIsOne;
}

MPSGenerator::MPSGenerator(const ILinearProblem& lp, const std::string& name):
    linearProblem_(lp),
    name_(name)

{
    if (lp.variableCount() == 0)
    {
        logs.warning() << "Linear problem '" << name << "' contains no variables.";
    }

    exportableConstraintsNames_ = ExtractNames(lp.getConstraints());
    exportableVariablesNames_ = ExtractNames(lp.getVariables());
}

void MPSGenerator::writeHeader()
{
    fmt::format_to(std::back_inserter(out_), "* Antares Simulator\n");
    fmt::format_to(std::back_inserter(out_),
                   "* Number of variables: {}\n",
                   linearProblem_.variableCount());
    fmt::format_to(std::back_inserter(out_),
                   "* Number of constraints: {}\n",
                   linearProblem_.constraintCount());
}

void MPSGenerator::writeRows()
{
    /* ========= ROWS ========= */
    out_ += "ROWS\n";
    fmt::format_to(std::back_inserter(out_), "{}N  OBJ\n", pad);
    int i = 0;
    for (const auto& constraints = linearProblem_.getConstraints(); const auto& c: constraints)
    {
        fmt::format_to(std::back_inserter(out_),
                       "{}{}  {}\n",
                       pad,
                       ConstraintSense(c->getLb(), c->getUb(), linearProblem_.infinity()),
                       exportableConstraintsNames_[i]);
        ++i;
    }
}

struct MpsColumnEntry
{
    int row; // constraint index
    double value;
};

using MpsColumn = std::vector<MpsColumnEntry>;

std::vector<MpsColumn> Columns(const ILinearProblem& linearProblem)
{
    std::vector<MpsColumn> columns;
    columns.resize(linearProblem.variableCount());

    const auto& constraints = linearProblem.getConstraints();
    for (int row = 0; row < linearProblem.constraintCount(); ++row)
    {
        const auto coeffs = constraints[row]->getCoefficients();

        for (const auto& [var, val]: coeffs)
        {
            columns[var].emplace_back(row, val);
        }
    }
    return columns;
}

void MPSGenerator::writeColumns()
{
    /* ========= COLUMNS ========= */
    out_ += "COLUMNS\n";
    const auto columns = Columns(linearProblem_);
    int i = 0;
    for (const auto& vars = linearProblem_.getVariables(); const auto& var: vars)
    {
        const bool isInt = var->isInteger();
        if (isInt)
        {
            fmt::format_to(std::back_inserter(out_),
                           "{}MARK{:010}    'MARKER'                 'INTORG'\n",
                           pad,
                           i);
        }
        auto* varPtr = var.get();
        auto& exportableVariablesName = exportableVariablesNames_[i];
        if (auto coef = linearProblem_.getObjectiveCoefficient(varPtr); isNotEqual(coef, 0.0))
        {
            // TODO Maximisation problem...
            // if (linearProblem_.isMaximization())  coef*=-1;
            fmt::format_to(std::back_inserter(out_),
                           "{}{}  OBJ  {}\n",
                           pad,
                           exportableVariablesName,
                           coef);
        }

        // constraints
        const auto& mpsColumn = columns[i];
        for (const auto& [row, coef]: mpsColumn)
        {
            fmt::format_to(std::back_inserter(out_),
                           "{}{}  {}  {}\n",
                           pad,
                           exportableVariablesName,
                           exportableConstraintsNames_[row],
                           coef);
        }

        if (isInt)
        {
            fmt::format_to(std::back_inserter(out_),
                           "{}MARK{:010}    'MARKER'                 'INTEND'\n",
                           pad,
                           i);
        }
        ++i;
    }
}

void MPSGenerator::writeRhs()
{
    const double INF = linearProblem_.infinity();
    /* ========= RHS ========= */
    out_ += "RHS\n";
    if (const auto objOffset = linearProblem_.getObjectiveOffset(); isNotEqual(objOffset, 0.0))
    {
        fmt::format_to(std::back_inserter(out_), "{}RHS1  OBJ  {}\n", pad, -objOffset);
    }
    int i = 0;
    for (const auto& c: linearProblem_.getConstraints())
    {
        if (const auto rhs = ConstraintRhs(c->getLb(), c->getUb(), INF);
            isNotEqual(std::abs(rhs), INF) && isNotEqual(rhs, 0.))
        {
            fmt::format_to(std::back_inserter(out_),
                           "{}RHS1  {}  {}\n",
                           pad,
                           exportableConstraintsNames_[i],
                           rhs);
        }
        ++i;
    }
}

void MPSGenerator::writeRanges()
{
    out_ += "RANGES\n";
    int i = 0;
    for (const auto& c: linearProblem_.getConstraints())
    {
        if (const auto range = std::abs(c->getUb() - c->getLb());
            isNotEqual(range, linearProblem_.infinity()) && isNotEqual(range, 0.))
        {
            fmt::format_to(std::back_inserter(out_),
                           "{}RNG1  {}  {}\n",
                           pad,
                           exportableConstraintsNames_[i],
                           range);
        }
        ++i;
    }
}

void MPSGenerator::writeBounds()
{
    const double INF = linearProblem_.infinity();
    /* ========= BOUNDS ========= */
    out_ += "BOUNDS\n";
    int i = 0;
    for (const auto& var: linearProblem_.getVariables())
    {
        const auto varName = exportableVariablesNames_[i];
        ++i;
        const std::string bnd = "BND1";

        const double lb = var->getLb();
        const double ub = var->getUb();
        const bool isInt = var->isInteger();
        const bool isBinary = IsBoolean(*var);
        const bool lbIsMinusInfinity = isEqual(lb, -INF);
        const bool ubIsPlusInfinity = isEqual(ub, INF);
        const bool lbIsZero = isEqual(lb, 0.0);
        if (lbIsMinusInfinity && ubIsPlusInfinity)
        {
            fmt::format_to(std::back_inserter(out_), "{}FR {}  {}\n", pad, bnd, varName);
            continue;
        }
        if (isEqual(lb, ub))
        {
            fmt::format_to(std::back_inserter(out_), "{}FX {} {} {}\n", pad, bnd, varName, lb);
            continue;
        }
        if (isBinary)
        {
            fmt::format_to(std::back_inserter(out_), "{}BV {} {}\n", pad, bnd, varName);
            continue;
        }

        if (lbIsZero && ubIsPlusInfinity) // this case is the default
        {
            // out_ << pad << "PL " << bnd << " " << varName << "\n";
            continue;
        }

        if (isInt)
        {
            if (lbIsMinusInfinity)
            {
                fmt::format_to(std::back_inserter(out_), "{}MI {} {}\n", pad, bnd, varName);
            }
            else if (!lbIsZero)
            {
                fmt::format_to(std::back_inserter(out_), "{}LI {} {} {}\n", pad, bnd, varName, lb);
            }
            if (!ubIsPlusInfinity)
            {
                fmt::format_to(std::back_inserter(out_), "{}UI {} {} {}\n", pad, bnd, varName, ub);
            }
        }
        else
        {
            if (lbIsMinusInfinity)
            {
                fmt::format_to(std::back_inserter(out_), "{}MI {} {}\n", pad, bnd, varName);
            }
            else if (!lbIsZero)
            {
                fmt::format_to(std::back_inserter(out_), "{}LO {} {} {}\n", pad, bnd, varName, lb);
            }

            if (!ubIsPlusInfinity)
            {
                fmt::format_to(std::back_inserter(out_), "{}UP {} {} {}\n", pad, bnd, varName, ub);
            }
        }
    }
}

void MPSGenerator::writeName()
{
    /* ========= NAME ========= */
    fmt::format_to(std::back_inserter(out_), "NAME {}\n", name_);
}

void MPSGenerator::writeEnd()
{
    /* ========= END ========= */
    out_ += "ENDATA\n";
}

void MPSFileWriter::write(const std::filesystem::path& filename, const std::string& content)
{
    std::ofstream file(filename);
    if (!file.is_open())
    {
        throw std::runtime_error("Failed to open file: " + filename.string());
    }
    file << content;
}

std::string MPSGenerator::run()
{
    out_ = "";
    if (linearProblem_.variableCount() == 0)
    {
        logs.warning() << "mps will not be produced for empty linear problem '" << name_ << "'.";
        return "";
    }
    writeHeader();

    writeName();

    writeRows();

    writeColumns();

    writeRhs();

    writeRanges();

    writeBounds();

    writeEnd();
    return std::move(out_);
}
} // namespace Antares::IO::Outputs
