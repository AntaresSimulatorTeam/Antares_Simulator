#include <antares/solver/expressions/NodeRegistry.h>
#include <antares/solver/expressions/nodes/ExpressionsNodes.h>
#include <antares/solver/expressions/visitors/CloneVisitor.h>

namespace Antares::Solver
{
NodeRegistry::NodeRegistry(Antares::Solver::Nodes::Node* node,
                           Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry):
    node(node),
    registry(std::move(registry))
{
}

} // namespace Antares::Solver
