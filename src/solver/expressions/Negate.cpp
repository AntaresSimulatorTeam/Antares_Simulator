#include <antares/solver/expressions/Negate.h>

namespace Antares::Solver::Expressions
{
Negate::Negate(std::shared_ptr<Node> n):
    n(n)
{
}

void Negate::accept(Visitor& visitor)
{
    return visitor.visit(*this);
}
} // namespace Antares::Solver::Expressions
