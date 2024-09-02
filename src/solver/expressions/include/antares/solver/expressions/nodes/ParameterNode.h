#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
/**
 * @brief Represents a parameter node in a syntax tree, storing a string value.
 */
class ParameterNode final: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;

    std::string name() const override
    {
        return "ParameterNode";
    }
};
} // namespace Antares::Solver::Nodes
