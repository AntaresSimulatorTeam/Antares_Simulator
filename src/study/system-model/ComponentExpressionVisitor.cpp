#include "ComponentExpressionVisitor.h"
#include <antares/expressions/nodes/ExpressionsNodes.h>

#include "antares/expressions/nodes/ParameterNode.h"

using namespace Antares::Expressions;

std::string ComponentExpressionVisitor::name() const
{
    return "ComponentExpressionVisitor";
}

Nodes::Node* ComponentExpressionVisitor::visit(const Nodes::ParameterNode* node)
{
    auto parameter_id = node->value();
    if (!parameter_values_.contains(parameter_id))
    {
        throw std::invalid_argument("Parameter '" + parameter_id + "' has no associated value");
    }
    return registry_.create<Nodes::LiteralNode>(parameter_values_.at(parameter_id));
}