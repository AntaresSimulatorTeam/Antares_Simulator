// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

#include <iostream>
#include <numeric>
#include <ranges>

#include <boost/algorithm/string.hpp>

#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/HelpVisitNode.h>
#include <antares/expressions/visitors/PrintVisitor.h>

namespace Antares::Expressions::Visitors
{

std::string PrintVisitor::visit(const Nodes::SumNode* node)
{
    const auto args_as_string = visitChildrenNodes(node);
    return "(" + boost::algorithm::join(args_as_string, "+") + ")";
}

std::string PrintVisitor::visit(const Nodes::SubtractionNode* node)
{
    return "(" + dispatch(node->left()) + "-" + dispatch(node->right()) + ")";
}

std::string PrintVisitor::visit(const Nodes::MultiplicationNode* node)
{
    return "(" + dispatch(node->left()) + "*" + dispatch(node->right()) + ")";
}

std::string PrintVisitor::visit(const Nodes::DivisionNode* node)
{
    return "(" + dispatch(node->left()) + "/" + dispatch(node->right()) + ")";
}

std::string PrintVisitor::visit(const Nodes::EqualNode* node)
{
    return dispatch(node->left()) + "==" + dispatch(node->right());
}

std::string PrintVisitor::visit(const Nodes::LessThanOrEqualNode* node)
{
    return dispatch(node->left()) + "<=" + dispatch(node->right());
}

std::string PrintVisitor::visit(const Nodes::GreaterThanOrEqualNode* node)
{
    return dispatch(node->left()) + ">=" + dispatch(node->right());
}

std::string PrintVisitor::visit(const Nodes::NegationNode* node)
{
    return "-(" + dispatch(node->child()) + ")";
}

std::string PrintVisitor::visit(const Nodes::ParameterNode* node)
{
    return node->value();
}

std::string PrintVisitor::visit(const Nodes::VariableNode* node)
{
    return node->value();
}

std::string PrintVisitor::visit(const Nodes::LiteralNode* node)
{
    return std::to_string(node->value());
}

std::string PrintVisitor::visit(const Nodes::PortFieldNode* node)
{
    return node->getPortName() + "." + node->getFieldName();
}

std::string PrintVisitor::visit(const Nodes::PortFieldSumNode* node)
{
    return node->getPortName() + "." + node->getFieldName();
}

std::string trimAndFormat(std::string s)
{
    boost::trim_left(s);

    if (!s.empty() && !s.starts_with('-') && !s.starts_with('+'))
    {
        s = "+" + s;
    }

    return s;
}

std::string PrintVisitor::visit(const Nodes::TimeShiftNode* node)
{
    auto formatedShift = trimAndFormat(dispatch(node->right()));
    return dispatch(node->left()) + "[ t " + formatedShift + " ]";
}

std::string PrintVisitor::visit(const Nodes::TimeIndexNode* node)
{
    return dispatch(node->left()) + "[ " + dispatch(node->right()) + " ]";
}

std::string PrintVisitor::visit(const Nodes::TimeSumNode* node)
{
    return "sum(" + dispatch(node->from()) + " .. " + dispatch(node->to()) + ", "
           + dispatch(node->expression()) + ")";
}

std::string PrintVisitor::visit(const Nodes::TPlusNode* node)
{
    return "t" + trimAndFormat(dispatch(node->child()));
}

std::string PrintVisitor::visit(const Nodes::AllTimeSumNode* node)
{
    return "sum(" + dispatch(node->child()) + ")";
}

std::string PrintVisitor::visit(const Nodes::FunctionNode* node)
{
    std::string nodeType = node->typeToString();
    const std::vector<std::string> args_as_str = visitChildrenNodes(node);
    if (nodeType == "dual")
    {
        return nodeType + "(" + args_as_str[0] + ")[" + args_as_str[1] + "]";
    }
    else
    {
        return nodeType + "(" + boost::algorithm::join(args_as_str, ", ") + ")";
    }
}

std::string PrintVisitor::name() const
{
    return "PrintVisitor";
}
} // namespace Antares::Expressions::Visitors
