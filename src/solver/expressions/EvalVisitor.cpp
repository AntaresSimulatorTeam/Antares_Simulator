
#include "antares/solver/expressions/EvalVisitor.h"

#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
double EvalVisitor::visit(Add& add)
{
    return dispatch(*add.n1_) + dispatch(*add.n2_);
}

double EvalVisitor::visit(Parameter& param)
{
    return 0.;
}

double EvalVisitor::visit(Literal& lit)
{
    return lit.value_;
}

double EvalVisitor::visit(Negate& neg)
{
    return -dispatch(*neg.n_);
}
} // namespace Antares::Solver::Expressions
