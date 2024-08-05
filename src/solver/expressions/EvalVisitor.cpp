
#include "antares/solver/expressions/EvalVisitor.h"

#include <antares/solver/expressions/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
double EvalVisitor::visit(const AddNode& add)
{
    return dispatch(*add.n1_) + dispatch(*add.n2_);
}

double EvalVisitor::visit(const ParameterNode& param)
{
    return 0.;
}

double EvalVisitor::visit(const LiteralNode& lit)
{
    return lit.value_;
}

double EvalVisitor::visit(const NegationNode& neg)
{
    return -dispatch(*neg.n_);
}

double EvalVisitor::visit(const PortFieldNode& port_field_node)
{
    return 0.;
}
} // namespace Antares::Solver::Expressions
