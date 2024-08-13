#pragma once

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
class LiteralNode: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;
};
} // namespace Antares::Solver::Nodes
