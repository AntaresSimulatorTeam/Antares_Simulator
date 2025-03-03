

#include "antares/expressions/nodes/TimeShiftNode.h"

namespace Antares::Expressions::Nodes
{
TimeShiftNode::TimeShiftNode(Node* toBeShifted, int shift):
    UnaryNode(toBeShifted),
    shift_(shift)
{
}

std::string TimeShiftNode::name() const
{
    return "TimeShiftNode";
}
} // namespace Antares::Expressions::Nodes
