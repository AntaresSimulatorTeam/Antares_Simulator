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

#include <antares/logs/logs.h>
#include "antares/io/outputs/ExportableName.h"

using namespace Antares::Optimisation::LinearProblemApi;
static constexpr std::string_view pad = "    ";
static constexpr double EPS = 10e-16;

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

    exportableConstraintsNames_ = extractNames(lp.getConstraints());
    exportableVariablesNames_ = extractNames(lp.getVariables());
}

void MPSGenerator::writeHeader()
{
    out_ << "* Antares Simulator " << "\n";
    out_ << "* Number of variables: " << std::to_string(linearProblem_.variableCount()) << "\n";
    out_ << "* Number of constraints: " << std::to_string(linearProblem_.constraintCount()) << "\n";
}

void MPSGenerator::writeRows()
{
    /* ========= ROWS ========= */
    out_ << "ROWS\n";
    out_ << " N  OBJ\n";
    int i = 0;
    for (const auto& constraints = linearProblem_.getConstraints(); const auto& c: constraints)
    {
        out_ << " " << ConstraintSense(c->getLb(), c->getUb(), linearProblem_.infinity()) << "  "
             << exportableConstraintsNames_.at(i) << "\n";
        ++i;
    }
}

void MPSGenerator::writeColumns()
{
    /* ========= COLUMNS ========= */
    out_ << "COLUMNS\n";

    int i = 0;
    for (const auto& vars = linearProblem_.getVariables(); const auto& var: vars)
    {
        const bool isInt = var->isInteger();
        if (isInt)
        {
            out_ << pad << "MARK" << fmt::format("{:010}", i)
                 << "    'MARKER'                 'INTORG'" << "\n";
        }
        if (const auto coef = linearProblem_.getObjectiveCoefficient(var.get());
            isNotEqual(coef, 0.0))
        {
            // TODO
            // out_ << "    " << exportableVariablesNames_.at(i) << "  OBJ  "
            //      << (linearProblem_.isMinimization() ? coef : -coef) << "\n";
            out_ << pad << exportableVariablesNames_.at(i) << "  OBJ  " << coef << "\n";
        }

        // constraints
        int j = 0;
        for (const auto& c: linearProblem_.getConstraints())
        {
            if (const auto coef = c->getCoefficient(var.get()); isNotEqual(coef, 0.0))
            {
                out_ << pad << exportableVariablesNames_.at(i) << "  "
                     << exportableConstraintsNames_.at(j) << "  " << coef << "\n";
            }
            ++j;
        }
        if (isInt)
        {
            out_ << pad << "MARK" << fmt::format("{:010}", i)
                 << "    'MARKER'                 'INTEND'" << "\n";
        }
        ++i;
    }
}

void MPSGenerator::writeRhs()
{
    const double INF = linearProblem_.infinity();
    /* ========= RHS ========= */
    out_ << "RHS\n";
    if (const auto objOffset = linearProblem_.getObjectiveOffset(); isNotEqual(objOffset, 0.0))
    {
        out_ << pad << "RHS1  " << "OBJ" << "  " << -objOffset << "\n";
    }
    int i = 0;
    for (const auto& c: linearProblem_.getConstraints())
    {
        if (const auto rhs = ConstraintRhs(c->getLb(), c->getUb(), INF);
            isNotEqual(std::abs(rhs), INF) && isNotEqual(rhs, 0.))
        {
            out_ << pad << "RHS1  " << exportableConstraintsNames_.at(i) << "  " << rhs << "\n";
        }
        ++i;
    }
}

void MPSGenerator::writeRanges()
{
    out_ << "RANGES\n";
    int i = 0;
    for (const auto& c: linearProblem_.getConstraints())
    {
        if (const auto range = std::abs(c->getUb() - c->getLb());
            isNotEqual(range, linearProblem_.infinity()) && isNotEqual(range, 0.))
        {
            out_ << pad << "RNG1  " << exportableConstraintsNames_.at(i) << "  " << range << "\n";
        }
        ++i;
    }
}

void MPSGenerator::writeBounds()
{
    const double INF = linearProblem_.infinity();
    /* ========= BOUNDS ========= */
    out_ << "BOUNDS\n";
    int i = 0;
    for (const auto& var: linearProblem_.getVariables())
    {
        const auto varName = exportableVariablesNames_.at(i);
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
            out_ << pad << "FR " << bnd << "  " << varName << "\n";
            continue;
        }
        if (isEqual(lb, ub))
        {
            out_ << pad << "FX " << bnd << " " << varName << " " << lb << "\n";
            continue;
        }
        if (isBinary)
        {
            out_ << pad << "BV " << bnd << " " << varName << "\n";
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
                out_ << pad << "MI " << bnd << " " << varName << "\n";
            }
            else if (!lbIsZero)
            {
                out_ << pad << "LI " << bnd << " " << varName << " " << lb << "\n";
            }
            if (!ubIsPlusInfinity)
            {
                out_ << pad << "UI " << bnd << " " << varName << " " << ub << "\n";
            }
        }
        else
        {
            if (lbIsMinusInfinity)
            {
                out_ << pad << "MI " << bnd << " " << varName << "\n";
            }
            else if (!lbIsZero)
            {
                out_ << pad << "LO " << bnd << " " << varName << " " << lb << "\n";
            }

            if (!ubIsPlusInfinity)
            {
                out_ << pad << "UP " << bnd << " " << varName << " " << ub << "\n";
            }
        }
    }
}

void MPSGenerator::writeName()
{
    /* ========= NAME ========= */
    out_ << "NAME " << name_ << "\n";
}

void MPSGenerator::writeEnd()
{
    /* ========= END ========= */
    out_ << "ENDATA\n";
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
    out_.str("");
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
    return out_.str();
}
} // namespace Antares::IO::Outputs
