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
#include "antares/io/outputs/MPSWriter.h"

#include <fstream>
using namespace Antares::Optimisation::LinearProblemApi;
namespace Antares::IO
{
char ConstraintSense(const double& lb, const double& ub, const double& infinity)
{
    if (lb == -infinity && ub == infinity)
    {
        return 'N';
    }
    else if (lb == ub)
    {
        return 'E';
    }
    else if (lb == -infinity)
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
    return variable.isInteger() && std::ceil(variable.getLb()) == 0.0
           && std::floor(variable.getUb()) == 1.0;
}

MPSWriter::MPSWriter(const Antares::Optimisation::LinearProblemApi::ILinearProblem& lp,
                     const std::filesystem::path& path,
                     const std::string& name):
    linearProblem_(lp),
    out_(path),
    name_(name)

{
    if (!out_.is_open())
    {
        throw std::runtime_error("Cannot open MPS file: " + path.string());
    }
}

void MPSWriter::writeHeader()
{
    out_ << "* Antares Simulator " << "\n";
    out_ << "* Number of variables: " << std::to_string(linearProblem_.variableCount()) << "\n";
    out_ << "* Number of constraints: " << std::to_string(linearProblem_.constraintCount()) << "\n";
}

void MPSWriter::writeRows()
{
    /* ========= ROWS ========= */
    out_ << "ROWS\n";
    out_ << " N  OBJ\n";

    for (const auto& constraints = linearProblem_.getConstraints(); const auto& c: constraints)
    {
        out_ << " " << ConstraintSense(c->getLb(), c->getUb(), linearProblem_.infinity()) << "  "
             << c->getName() << "\n";
    }
}

void MPSWriter::writeColumns()
{
    /* ========= COLUMNS ========= */
    out_ << "COLUMNS\n";

    for (const auto& vars = linearProblem_.getVariables(); const auto& var: vars)
    {
        if (const auto coef = linearProblem_.getObjectiveCoefficient(var.get()); coef != 0.0)
        {
            // TODO
            out_ << "    " << var->getName() << "  OBJ  "
                 << (linearProblem_.isMinimization() ? coef : -coef) << "\n";
        }

        // constraints
        for (const auto& c: linearProblem_.getConstraints())
        {
            if (const auto coef = c->getCoefficient(var.get()); coef != 0.0)
            {
                out_ << "    " << var->getName() << "  " << c->getName() << "  " << coef << "\n";
            }
        }
    }
}

void MPSWriter::writeRhs()
{
    const double INF = linearProblem_.infinity();
    /* ========= RHS ========= */
    out_ << "RHS\n";
    if (const auto objOffset = linearProblem_.getObjectiveOffset(); objOffset != 0.0)
    {
        out_ << "    RHS1  " << "OBJ" << "  " << -objOffset << "\n";
    }
    for (const auto& c: constraints)
    {
        if (const auto rhs = ConstraintRhs(c->getLb(), c->getUb(), INF);
            std::abs(rhs) != INF && std::abs(rhs) != 0.)
        {
            out_ << "    RHS1  " << c->getName() << "  " << rhs << "\n";
        }
    }
}

void MPSWriter::writeBounds()
{
    const double INF = linearProblem_.infinity();
    /* ========= BOUNDS ========= */
    out_ << "BOUNDS\n";
    for (const auto& var: linearProblem_.getVariables())
    {
        const auto varName = var->getName();
        const std::string bnd = "BND1";

        const double lb = var->getLb();
        const double ub = var->getUb();
        const bool isInt = var->isInteger();
        const bool isBinary = IsBoolean(*var);
        if (lb == -INF && ub == INF)
        {
            out_ << " FR " << bnd << "  " << varName << "\n";
            continue;
        }
        if (lb == ub)
        {
            out_ << " FX " << bnd << " " << varName << " " << lb << "\n";
            continue;
        }
        if (isBinary)
        {
            out_ << " BV " << bnd << " " << varName << "\n";
            continue;
        }
        if (isInt)
        {
            if (lb == -INF)
            {
                out_ << " MI " << bnd << " " << varName << "\n";
            }
            else if (lb != 0.0 || ub == INF)
            {
                out_ << " LI " << bnd << " " << varName << " " << lb << "\n";
            }
            if (ub != INF)
            {
                out_ << " UI " << bnd << " " << varName << " " << ub << "\n";
            }
        }
        else
        {
            if (lb == -INF)
            {
                out_ << " MI " << bnd << " " << varName << "\n";
            }
            else if (lb != 0.0)
            {
                out_ << " LO " << bnd << " " << varName << " " << lb << "\n";
            }

            else if (ub != INF)
            {
                out_ << " UP " << bnd << " " << varName << " " << ub << "\n";
            }
        }
    }
}

void MPSWriter::writeName()
{
    /* ========= NAME ========= */
    out_ << "NAME " << name << "\n";
}

void MPSWriter::writeEnd()
{
    /* ========= END ========= */
    out_ << "ENDATA\n";
}

void MPSWriter::write()
{
    writeHeader();

    writeName();

    writeRows();

    writeColumns();

    writeRhs();

    writeBounds();

    writeEnd();
}
} // namespace Antares::IO
