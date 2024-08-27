#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
class VariableNode final: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;

    constexpr NodeKind type() const override
    {
        return NodeKind::VARIABLE_NODE;
    }
};
} // namespace Antares::Solver::Nodes
