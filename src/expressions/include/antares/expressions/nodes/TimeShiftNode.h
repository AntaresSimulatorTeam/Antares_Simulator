// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include "antares/expressions/nodes/BinaryNode.h"

namespace Antares::Expressions::Nodes
{

class TimeShiftNode final: public BinaryNode
{
public:
    using BinaryNode::BinaryNode;

    std::string name() const override
    {
        return "TimeShiftNode";
    }
};
} // namespace Antares::Expressions::Nodes
