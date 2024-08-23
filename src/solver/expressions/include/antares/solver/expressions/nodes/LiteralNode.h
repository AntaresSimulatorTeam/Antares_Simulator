#pragma once

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
class LiteralNode: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;

    std::string name() const override
    {
        return "LiteralNode";
    }
};
} // namespace Antares::Solver::Nodes
