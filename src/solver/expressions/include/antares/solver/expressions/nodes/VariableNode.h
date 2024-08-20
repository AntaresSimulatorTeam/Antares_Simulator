#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>
#include <antares/solver/expressions/nodes/TimeIndex.h>

namespace Antares::Solver::Nodes
{
class VariableNode final: public Leaf<std::string>, public TimeIndex
{
public:
    explicit VariableNode(const std::string& value);
    explicit VariableNode(const std::string& value, bool time_varying, bool scenario_varying);
};
} // namespace Antares::Solver::Nodes
