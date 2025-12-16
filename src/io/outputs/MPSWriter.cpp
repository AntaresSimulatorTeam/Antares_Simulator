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
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
#include "antares/io/outputs/MPSWriter.h"

#include <fstream>
using namespace Antares::Optimisation::LinearProblemApi;

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

void WriteMPS(const ILinearProblem& lp, const std::filesystem::path& path, const std::string& name)
{
    std::ofstream out(path);
    if (!out)
    {
        throw std::runtime_error("Cannot open MPS file: " + path.string());
    }

    const auto INF = lp.infinity();

    /* ========= NAME ========= */
    out << "NAME " << name << "\n";

    /* ========= ROWS ========= */
    out << "ROWS\n";
    out << " N  OBJ\n";

    const auto& constraints = lp.getConstraints();
    for (const auto& c: constraints)
    {
        out << " " << ConstraintSense(c->getLb(), c->getUb(), INF) << "  " << c->getName() << "\n";
    }

    /* ========= COLUMNS ========= */
    out << "COLUMNS\n";

    const bool hasInt = !lp.isLP();
    const auto& vars = lp.getVariables();
    for (size_t j = 0; j < vars.size(); ++j)
    {
        const auto& var = vars[j];

        const bool isInt = var->isInteger();
        // if (hasInt && isInt)
        // {
        //     out << "    INTORG\n";
        // }

        if (const auto coef = lp.getObjectiveCoefficient(var.get()); coef != 0.0)
        {
            // TODO
            out << "    " << var->getName() << "  OBJ  " << (lp.isMinimization() ? coef : -coef)
                << "\n";
        }

        // constraints
        for (const auto& c: constraints)
        {
            if (const auto coef = c->getCoefficient(var.get()); coef != 0.0)
            {
                out << "    " << var->getName() << "  " << c->getName() << "  " << coef << "\n";
            }
        }

        // if (hasInt && isInt)
        // {
        //     out << "    INTEND\n";
        // }
    }

    /* ========= RHS ========= */
    out << "RHS\n";
    if (const auto objOffset = lp.getObjectiveOffset(); objOffset != 0.0)
    {
        out << "    RHS1  " << "OBJ" << "  " << -objOffset << "\n";
    }
    for (const auto& c: constraints)
    {
        if (const auto rhs = ConstraintRhs(c->getLb(), c->getUb(), INF);
            std::abs(rhs) != INF && std::abs(rhs) != 0.)
        {
            out << "    RHS1  " << c->getName() << "  " << rhs << "\n";
        }
    }

    /* ========= BOUNDS ========= */
    out << "BOUNDS\n";
    for (const auto& var: vars)
    {
        const auto varName = var->getName();
        const std::string bnd = "BND";

        double lb = var->getLb();
        double ub = var->getUb();
        const bool isInt = var->isInteger();
        const bool isBinary = IsBoolean(*var);
        if (lb == -INF && ub == INF)
        {
            out << " FR " << bnd << "  " << varName << "\n";
            continue;
        }
        if (lb == ub)
        {
            out << " FX " << bnd << " " << varName << " " << lb << "\n";
            continue;
        }
        if (isBinary)
        {
            out << " BV " << bnd << " " << varName << "\n";
            continue;
        }
        if (isInt)
        {
            if (lb == -INF)
            {
                out << " MI " << bnd << " " << varName << "\n";
            }
            else if (lb != 0.0 || ub == INF)
            {
                out << " LI " << bnd << " " << varName << " " << lb << "\n";
            }
            if (ub != INF)
            {
                out << " UI " << bnd << " " << varName << " " << ub << "\n";
            }
        }
        else
        {
            if (lb == -INF)
            {
                out << " MI " << bnd << " " << varName << "\n";
            }
            else if (lb != 0.0)
            {
                out << " LO " << bnd << " " << varName << " " << lb << "\n";
            }

            else if (ub != INF)
            {
                out << " UP " << bnd << " " << varName << " " << ub << "\n";
            }
        }
    }

    /* ========= END ========= */
    out << "ENDATA\n";
}
