

#include "antares/expressions/nodes/TimeShiftNode.h"

namespace Antares::Expressions::Nodes
{
TimeShiftNode::TimeShiftNode(Node* child, int value):
    UnaryNode(child),
    value_(value)
{
}

int TimeShiftNode::value() const
{
    return value_;
}

// function to convert int to "+value" or "-value"
std::string TimeShiftNode::valueToSignedString() const
{
    return (value_ >= 0) ? ("+" + std::to_string(value_)) : std::to_string(value_);
}
std::string TimeShiftNode::name() const
{
    return "TimeShiftNode";
}

} // namespace Antares::Expressions::Nodes
