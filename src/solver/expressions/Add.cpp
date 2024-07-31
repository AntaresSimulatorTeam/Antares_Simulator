#include <antares/solver/expressions/Add.h>

Add::Add(Node* n1, Node* n2):
    n1(n1),
    n2(n2)
{
}

std::any Add::accept(Visitor& visitor) override
{
    return visitor.visit(*this);
}
