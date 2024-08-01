
#include "antares/solver/expressions/EvalVisitor.h"

#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
double EvalVisitor::visit(const Add& add)
{
    return Base::visit(*add.n1_) + Base::visit(*add.n2_);
}

double EvalVisitor::visit(const Parameter& param)
{
    return 0.;
}

double EvalVisitor::visit(const Literal& lit)
{
    return lit.value_;
}

double EvalVisitor::visit(const Negate& neg)
{
    return -Base::visit(*neg.n_);
}
} // namespace Antares::Solver::Expressions
