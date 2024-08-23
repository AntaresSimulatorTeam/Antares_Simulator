#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
class VariableNode final: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;

    std::string name() const override
    {
        return "VariableNode";
    }
};
} // namespace Antares::Solver::Nodes
