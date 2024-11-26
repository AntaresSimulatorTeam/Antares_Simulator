#pragma once

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/Node.h>

namespace Antares::Solver
{
class NodeRegistry
{
public:
    NodeRegistry() = default;
    NodeRegistry(Antares::Solver::Nodes::Node* node,
                 Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry);

    // Shallow copy
    NodeRegistry(NodeRegistry&&) = default;
    NodeRegistry& operator=(NodeRegistry&&) = default;

    Antares::Solver::Nodes::Node* node;
    Antares::Solver::Registry<Antares::Solver::Nodes::Node> registry;
};
} // namespace Antares::Solver
