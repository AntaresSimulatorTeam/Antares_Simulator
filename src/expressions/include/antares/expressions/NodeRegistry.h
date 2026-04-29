#pragma once

#include <antares/expressions/Registry.hxx>
#include <antares/expressions/nodes/Node.h>

namespace Antares::Expressions
{
class NodeRegistry final
{
public:
    NodeRegistry() = default;
    NodeRegistry(Nodes::Node* node, Registry<Nodes::Node> registry);

    // Shallow copy
    NodeRegistry(NodeRegistry&&) = default;
    NodeRegistry& operator=(NodeRegistry&&) = default;

    Nodes::Node* node = nullptr;
    Registry<Nodes::Node> registry;
};
} // namespace Antares::Expressions
