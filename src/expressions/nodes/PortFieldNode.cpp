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
#include <antares/expressions/nodes/PortFieldNode.h>

namespace Antares::Expressions::Nodes
{
PortFieldNode::PortFieldNode(const std::string& port_name, const std::string& field_name):
    Hashable(port_name_, field_name_),
    port_name_(port_name),
    field_name_(field_name)
{
}

const std::string& PortFieldNode::getPortName() const
{
    return port_name_;
}

const std::string& PortFieldNode::getFieldName() const
{
    return field_name_;
}
} // namespace Antares::Expressions::Nodes
