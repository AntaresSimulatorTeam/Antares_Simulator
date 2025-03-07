

#include "antares/expressions/nodes/ParameterizedUnaryNode.h"

namespace Antares::Expressions::Nodes
{
ParameterizedUnaryNode::ParameterizedUnaryNode(Node* child, int value):
    UnaryNode(child),
    value_(value)
{
}
// function to convert int to "+value" or "-value"
std::string ParameterizedUnaryNode::valueToSignedString() const
{
    return (value_ >= 0) ? ("+" + std::to_string(value_)) : std::to_string(value_);
}

} // namespace Antares::Expressions::Nodes
