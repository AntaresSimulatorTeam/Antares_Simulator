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
#pragma once

#include <string>

#include <antares/expressions/NodeRegistry.h>

namespace Antares::Expressions::Nodes
{
class Node;
}

namespace Antares::Study::SystemModel
{

// TODO: add unit tests for this class
class Expression
{
public:
    Expression() = default;

    explicit Expression(const std::string& value, Expressions::NodeRegistry root):
        value_(value),
        root_(std::move(root)),
        empty_(false)
    {
    }

    const std::string& Value() const
    {
        return value_;
    }

    Expressions::Nodes::Node* RootNode() const
    {
        return root_.node;
    }

    bool Empty() const
    {
        return empty_;
    }

private:
    std::string value_;
    Expressions::NodeRegistry root_;
    bool empty_ = true;
};

} // namespace Antares::Study::SystemModel
