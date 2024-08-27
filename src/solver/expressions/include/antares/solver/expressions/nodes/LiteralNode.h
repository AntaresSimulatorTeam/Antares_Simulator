#pragma once

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
class LiteralNode: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;

    constexpr NodeKind type() const override
    {
        return NodeKind::LITERAL_NODE;
    }
};
} // namespace Antares::Solver::Nodes
