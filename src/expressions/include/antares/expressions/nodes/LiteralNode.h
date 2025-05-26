#pragma once

#include <antares/expressions/nodes/Leaf.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a literal node in a syntax tree, storing a double value.
 */
class LiteralNode: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;

    std::string name() const override
    {
        return "LiteralNode";
    }
};
} // namespace Antares::Expressions::Nodes
