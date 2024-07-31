#include <antares/solver/expressions/CloneVisitor.h>
#include <antares/solver/expressions/ExpressionsNodes.h>

std::any CloneVisitor::visit(const Add& add)
{
    auto* n1 = std::any_cast<Node*>(add.n1->accept(*this));
    Node* n2 = std::any_cast<Node*>(add.n2->accept(*this));

    Node* result = new Add(n1, n2);
    return result;
}

std::any CloneVisitor::visit(const Negate& neg)
{
    Node* n = std::any_cast<Node*>(neg.n->accept(*this));
    Node* result = new Negate(n);
    return result;
}

std::any CloneVisitor::visit(const Parameter& param)
{
    Node* result = new Parameter(param.name);
    return result;
}
