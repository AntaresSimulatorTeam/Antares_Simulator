

#include "antares/expressions/nodes/ParameterizedUnaryNode.h"

namespace Antares::Expressions::Nodes
{
ParameterizedUnaryNode::ParameterizedUnaryNode(Node* child, int value):
    UnaryNode(child),
    value_(value)
{
}

} // namespace Antares::Expressions::Nodes
