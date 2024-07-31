#include <iostream>

#include <antares/solver/expressions/PrintVisitor.h>

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
