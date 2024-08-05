/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
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
#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
std::unique_ptr<Node> CloneVisitor::visit(const AddNode& add)
{
    return std::make_unique<AddNode>(std::move(dispatch(*add.n1_)), std::move(dispatch(*add.n2_)));
}

std::unique_ptr<Node> CloneVisitor::visit(const Negate& neg)
{
    //    return std::make_unique<Negate>(dispatch(*neg.n_));
    return std::make_unique<Negate>(std::move(dispatch(*neg.n_)));
}

std::unique_ptr<Node> CloneVisitor::visit(const Parameter& param)
{
    return std::make_unique<Parameter>(param.value_);
}

std::unique_ptr<Node> CloneVisitor::visit(const Literal& param)
{
    return std::make_unique<Literal>(param.value_);
}

std::unique_ptr<Node> CloneVisitor::visit(const PortFieldNode& port_field_node)
{
    return std::make_unique<PortFieldNode>(port_field_node.port_name_, port_field_node.field_name_);
}
} // namespace Antares::Solver::Expressions
