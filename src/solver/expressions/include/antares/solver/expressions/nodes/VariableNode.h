#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Expressions
{
class VariableNode: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;

    bool IsConstant()
    {
        return false;
    }
};
} // namespace Antares::Solver::Expressions
