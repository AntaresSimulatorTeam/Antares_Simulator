#pragma once

#include <antares/solver/expressions/Registry.hxx>
#include <antares/solver/expressions/nodes/Node.h>

namespace Antares::Solver
{
struct NodeRegistry
{
    Node* node;
    Antares::Solver::Registry<Node> registry;
};
} // namespace Antares::Solver
