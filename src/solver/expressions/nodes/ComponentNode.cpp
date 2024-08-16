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
#include <antares/solver/expressions/nodes/ComponentNode.h>

namespace Antares::Solver::Nodes
{
ComponentNode::ComponentNode(const std::string& component_id, const std::string& component_name):
    component_id_(component_id),
    component_name_(component_name)
{
}

const std::string& ComponentNode::getComponentId() const
{
    return component_id_;
}

const std::string& ComponentNode::getComponentName() const
{
    return component_name_;
}

bool ComponentNode::operator==(const ComponentNode& other)
{
    return (component_id_ == other.component_id_) && (component_name_ == other.component_name_);
}
} // namespace Antares::Solver::Nodes
