

#include "antares/expressions/nodes/TimeIndexNode.h"

namespace Antares::Expressions::Nodes
{
TimeIndexNode::TimeIndexNode(Node* toBeindexed, int index):
    UnaryNode(toBeindexed),
    index_(index)
{
}

std::string TimeIndexNode::name() const
{
    return "TimeIndexNode";
}
} // namespace Antares::Expressions::Nodes
