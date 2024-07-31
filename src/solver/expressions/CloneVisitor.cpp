#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
void CloneVisitor::visit(const Add& add)
{
    add.n1->accept(*this);
    auto n1 = ptr_;

    add.n2->accept(*this);
    auto n2 = ptr_;

    ptr_ = make_shared<Add>(n1, n2);
}

void CloneVisitor::visit(const Negate& neg)
{
    neg.n->accept(*this);
    auto n = ptr_;

    ptr_ = std::make_shared<Negate>(n);
}

void CloneVisitor::visit(const Parameter& param)
{
    ptr_ = std::make_shared<Parameter>(param.name);
}
} // namespace Antares::Solver::Expressions
