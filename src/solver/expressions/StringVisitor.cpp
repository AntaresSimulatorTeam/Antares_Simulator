#include <string>

#include <antares/solver/expressions/ExpressionsNodes.h>
#include <antares/solver/expressions/StringVisitor.h>

namespace Antares::Solver::Expressions
{
void StringVisitor::visit(const Add& add)
{
    add.n1->accept(*this);
    str_ += "+";
    add.n2->accept(*this);
}

void StringVisitor::visit(const Negate& neg)
{
    str_ += "-(";
    neg.n->accept(*this);
    str_ += ")";
}

void StringVisitor::visit(const Parameter& param)
{
    str_ += param.name;
}
} // namespace Antares::Solver::Expressions
