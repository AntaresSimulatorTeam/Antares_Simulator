#include <antares/expressions/visitors/InvalidNode.h>

namespace Antares::Expressions::Visitors
{

InvalidNode::InvalidNode(const std::string& node_name):
    std::invalid_argument("Antares::Expressions::Nodes Visitor: invalid node type " + node_name)
{
}
} // namespace Antares::Expressions::Visitors
