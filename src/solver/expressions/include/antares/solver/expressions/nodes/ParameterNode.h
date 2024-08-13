#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Expressions
{
class ParameterNode: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;
};
} // namespace Antares::Solver::Expressions
