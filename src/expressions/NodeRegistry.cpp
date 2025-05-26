#include <antares/expressions/NodeRegistry.h>
#include <antares/expressions/nodes/ExpressionsNodes.h>
#include <antares/expressions/visitors/CloneVisitor.h>

namespace Antares::Expressions
{
NodeRegistry::NodeRegistry(
  Antares::Expressions::Nodes::Node* node,
  Antares::Expressions::Registry<Antares::Expressions::Nodes::Node> registry):
    node(node),
    registry(std::move(registry))
{
}

} // namespace Antares::Expressions
