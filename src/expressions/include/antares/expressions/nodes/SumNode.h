// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/nodes/ParentNode.h"

namespace Antares::Expressions::Nodes
{

class SumNode final: public ParentNode
{
public:
    using ParentNode::ParentNode;

    std::string name() const override
    {
        return "SumNode";
    }
};
} // namespace Antares::Expressions::Nodes
