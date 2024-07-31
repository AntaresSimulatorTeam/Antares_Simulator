#include <iostream>

#include <antares/solver/expressions/ExpressionsNodes.h>
#include <antares/solver/expressions/PrintVisitor.h>

namespace Antares::Solver::Expressions
{
std::any PrintVisitor::visit(const Add& add)
{
    add.n1->accept(*this);
    std::cout << "+";
    add.n2->accept(*this);
    return {};
}

std::any PrintVisitor::visit(const Negate& neg)
{
    std::cout << "-(";
    neg.n->accept(*this);
    std::cout << ")";
    return {};
}

std::any PrintVisitor::visit(const Parameter& param)
{
    std::cout << param.name;
    return {};
}
} // namespace Antares::Solver::Expressions
