#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{

/**
 * @brief Represents a variable node in a syntax tree, storing a string value.
 */
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
