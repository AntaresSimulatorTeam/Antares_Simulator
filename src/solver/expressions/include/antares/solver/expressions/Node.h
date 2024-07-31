#pragma once
#include <antares/solver/expressions/Visitor.h>

namespace Antares::Solver::Expressions
{
class Node
{
public:
    virtual ~Node() = default;
    virtual void accept(Visitor& visitor) = 0;
};
} // namespace Antares::Solver::Expressions
