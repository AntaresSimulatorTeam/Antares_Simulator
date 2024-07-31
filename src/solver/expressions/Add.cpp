#include <antares/solver/expressions/Add.h>

namespace Antares::Solver::Expressions
{
Add::Add(Node* n1, Node* n2):
    n1(n1),
    n2(n2)
{
}

std::any Add::accept(Visitor& visitor)
{
    return visitor.visit(*this);
}
} // namespace Antares::Solver::Expressions
