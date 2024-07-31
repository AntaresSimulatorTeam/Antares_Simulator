#include <antares/solver/expressions/Negate.h>

namespace Antares::Solver::Expressions
{
Negate::Negate(Node* n):
    n(n)
{
}

std::any Negate::accept(Visitor& visitor)
{
    return visitor.visit(*this);
}
} // namespace Antares::Solver::Expressions
