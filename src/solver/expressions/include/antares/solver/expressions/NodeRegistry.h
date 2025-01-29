#pragma once

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/Node.h>

namespace Antares::Solver
{
class NodeRegistry
{
public:
    NodeRegistry() = default;
    NodeRegistry(Nodes::Node* node, Registry<Nodes::Node> registry);

    // Shallow copy
    NodeRegistry(NodeRegistry&&) = default;
    NodeRegistry& operator=(NodeRegistry&&) = default;

    Nodes::Node* node;
    Registry<Nodes::Node> registry;
};
} // namespace Antares::Solver
