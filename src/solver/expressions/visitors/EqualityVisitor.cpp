#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/EqualityVisitor.h>

namespace Antares::Solver::Visitors
{

NodeComparisonResult EqualityVisitor::visit(const Nodes::AddNode& add1, const Nodes::AddNode& add2)
{
    return dispatch(*add1[0], *add2[0]) && dispatch(*add1[1], *add2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::SubtractionNode& sub1,
                                            const Nodes::SubtractionNode& sub2)
{
    return dispatch(*sub1[0], *sub2[0]) && dispatch(*sub1[1], *sub2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::MultiplicationNode& mult1,
                                            const Nodes::MultiplicationNode& mult2)
{
    return dispatch(*mult1[0], *mult2[0]) && dispatch(*mult1[1], *mult2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::DivisionNode& div1,
                                            const Nodes::DivisionNode& div2)
{
    return dispatch(*div1[0], *div2[0]) && dispatch(*div1[1], *div2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::EqualNode& equ1,
                                            const Nodes::EqualNode& equ2)
{
    return dispatch(*equ1[0], *equ2[0]) && dispatch(*equ1[1], *equ2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::LessThanOrEqualNode& lt1,
                                            const Nodes::LessThanOrEqualNode& lt2)
{
    return dispatch(*lt1[0], *lt2[0]) && dispatch(*lt1[1], *lt2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::GreaterThanOrEqualNode& gt1,
                                            const Nodes::GreaterThanOrEqualNode& gt2)
{
    return dispatch(*gt1[0], *gt2[0]) && dispatch(*gt1[1], *gt2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::VariableNode& var1,
                                            const Nodes::VariableNode& var2)
{
    return var1.getValue() == var2.getValue();
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::ParameterNode& param1,
                                            const Nodes::ParameterNode& param2)
{
    return param1.getValue() == param2.getValue();
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::LiteralNode& lit1,
                                            const Nodes::LiteralNode& lit2)
{
    return lit1.getValue() == lit1.getValue();
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::NegationNode& neg1,
                                            const Nodes::NegationNode& neg2)
{
    return dispatch(*neg1[0], *neg2[0]) && dispatch(*neg1[1], *neg2[1]);
}

NodeComparisonResult EqualityVisitor::visit(const Nodes::PortFieldNode& port_field_node1,
                                            const Nodes::PortFieldNode& port_field_node2)
{
    return port_field_node1 == port_field_node2;
}

NodeComparisonResult EqualityVisitor::visit(
  const Nodes::ComponentVariableNode& component_variable_node1,
  const Nodes::ComponentVariableNode& component_variable_node2)
{
    return component_variable_node1 == component_variable_node2;
}

NodeComparisonResult EqualityVisitor::visit(
  const Nodes::ComponentParameterNode& component_parameter_node1,
  const Nodes::ComponentParameterNode& component_parameter_node2)
{
    return component_parameter_node1 == component_parameter_node2;
}

} // namespace Antares::Solver::Visitors
