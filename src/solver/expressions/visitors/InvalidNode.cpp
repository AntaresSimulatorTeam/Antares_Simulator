#include <antares/solver/expressions/visitors/InvalidNode.h>

namespace Antares::Solver::Visitors
{

InvalidNode::InvalidNode(const std::string& node_name):
    std::invalid_argument("Antares::Solver::Nodes Visitor: invalid node type " + node_name)
{
}
} // namespace Antares::Solver::Visitors
