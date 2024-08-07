#pragma once

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Expressions
{
class LiteralNode: public Leaf<double>
{
public:
    using Leaf<double>::Leaf;
};
} // namespace Antares::Solver::Expressions
