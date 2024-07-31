#include <antares/solver/expressions/Negate.h>

Negate::Negate(Node* n):
    n(n)
{
}

std::any Negate::accept(Visitor& visitor)
{
    return visitor.visit(*this);
}
