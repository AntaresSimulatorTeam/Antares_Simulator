#include <iostream>

#include <antares/solver/expressions/ExpressionsNodes.h>
#include <antares/solver/expressions/PrintVisitor.h>

namespace Antares::Solver::Expressions
{
void PrintVisitor::visit(const Add& add)
{
    add.n1->accept(*this);
    std::cout << "+";
    add.n2->accept(*this);
}

void PrintVisitor::visit(const Negate& neg)
{
    std::cout << "-(";
    neg.n->accept(*this);
    std::cout << ")";
}

void PrintVisitor::visit(const Parameter& param)
{
    std::cout << param.name;
}
} // namespace Antares::Solver::Expressions
