#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
std::any CloneVisitor::visit(const Add& add)
{
    auto n1 = std::any_cast<std::shared_ptr<Node>>(add.n1->accept(*this));
    auto n2 = std::any_cast<std::shared_ptr<Node>>(add.n2->accept(*this));

    return std::static_pointer_cast<Node>(make_shared<Add>(n1, n2));
}

std::any CloneVisitor::visit(const Negate& neg)
{
    auto n = std::any_cast<std::shared_ptr<Node>>(neg.n->accept(*this));
    return std::static_pointer_cast<Node>(std::make_shared<Negate>(n));
}

std::any CloneVisitor::visit(const Parameter& param)
{
    return std::static_pointer_cast<Node>((std::make_shared<Parameter>(param.name)));
}
} // namespace Antares::Solver::Expressions
