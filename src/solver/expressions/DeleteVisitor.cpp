#include <antares/solver/expressions/DeleteVisitor.h>
#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
std::any DeleteVisitor::visit(const Add& add)
{
    add.n1->accept(*this);
    add.n2->accept(*this);

    delete &add;
    return {};
}

std::any DeleteVisitor::visit(const Negate& neg)
{
    neg.n->accept(*this);
    delete &neg;
    return {};
}

std::any DeleteVisitor::visit(const Parameter& param)
{
    delete &param;
    return {};
}
} // namespace Antares::Solver::Expressions
