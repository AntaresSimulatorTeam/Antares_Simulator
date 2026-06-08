// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#pragma once

#include <antares/expressions/nodes/Leaf.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a literal node in a syntax tree, storing a double value.
 */
class LiteralNode final: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;

    std::string name() const override
    {
        return "LiteralNode";
    }
};
} // namespace Antares::Expressions::Nodes
