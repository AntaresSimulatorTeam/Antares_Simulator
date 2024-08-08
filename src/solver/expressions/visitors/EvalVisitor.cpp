
#include "antares/solver/expressions/visitors/EvalVisitor.h"

#include <antares/solver/expressions/nodes/ExpressionsNodes.h>

namespace Antares::Solver::Expressions
{
double EvalVisitor::visit(const AddNode& add)
{
    return dispatch(*add[0]) + dispatch(*add[1]);
}

double EvalVisitor::visit(const SubtractionNode& add)
{
    return dispatch(*add[0]) - dispatch(*add[1]);
}

double EvalVisitor::visit(const MultiplicationNode& add)
{
    return dispatch(*add[0]) * dispatch(*add[1]);
}

double EvalVisitor::visit(const DivisionNode& add)
{
    if (auto divisor = dispatch(*add[1]); divisor != 0)
    {
        return dispatch(*add[0]) / divisor;
    }
    else
    {
        throw EvalVisitorDivisionException("DivisionNode Divide by zero");
    }
}

double EvalVisitor::visit(const EqualNode& add)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const LessThanNode& add)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const LessThanOrEqualNode& add)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const GreaterThanNode& add)
{
    // not implemented for comparison node
    return 0.;
}

double EvalVisitor::visit(const GreaterThanOrEqualNode& add)
{
    // not implemented for comparison node
    return 0.;
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
    return -dispatch(*neg[0]);
}

double EvalVisitor::visit(const PortFieldNode& port_field_node)
{
    return 0.;
}

double EvalVisitor::visit(const ComponentVariableNode& component_variable_node)
{
    // TODO
    return 0.;
}

double EvalVisitor::visit(const ComponentParameterNode& component_parameter_node)
{
    // TODO
    return 0.;
}
} // namespace Antares::Solver::Expressions
