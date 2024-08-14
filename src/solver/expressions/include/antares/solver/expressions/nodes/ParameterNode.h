#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>

namespace Antares::Solver::Nodes
{
class ParameterNode final: public Leaf<std::string>
{
public:
    using Leaf<std::string>::Leaf;
};
} // namespace Antares::Solver::Nodes
