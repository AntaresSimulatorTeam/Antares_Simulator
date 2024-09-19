
/*
 * Copyright 2007-2024, RTE (https://www.rte-france.com)
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

#include "antares/solver/expressions/visitors/CloneVisitor.h"

class ParameterSubstitutionVisitor: public Antares::Solver::Visitors::CloneVisitor
{
public:
    ParameterSubstitutionVisitor(Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry,
                                 const ComponentInstance& component):
        Antares::Solver::Visitors::CloneVisitor(registry),
        component_(component),
        registry_(registry)
    {
    }

    Antares::Solver::Nodes::Node* visit(const Antares::Solver::Nodes::ParameterNode* node) override
    {
        auto it = component_.parameters.find(node->value());
        if (it != component_.parameters.end())
        {
            return registry_.create<Antares::Solver::Nodes::LiteralNode>(it->second);
        }
        else
        {
            throw std::runtime_error("Parameter not found: " + node->name() + "in component ");
        }
    }

    const ComponentInstance& component_;
    Antares::Solver::Registry<Antares::Solver::Nodes::Node>& registry_;
};
