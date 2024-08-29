#pragma once

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
/**
 * @brief Represents a literal node in a syntax tree, storing a double value.
 */
class LiteralNode: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;
};
} // namespace Antares::Solver::Nodes
