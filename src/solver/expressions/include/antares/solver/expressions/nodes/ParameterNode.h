#pragma once

#include <string>

#include <antares/solver/expressions/nodes/Leaf.h>
#include "antares/solver/expressions/visitors/TimeIndex.h"

namespace Antares::Solver::Visitors
{
enum class TimeIndex : unsigned int;
}

namespace Antares::Solver::Nodes
{
/**
 * @brief Represents a parameter node in a syntax tree, storing a string value.
 */
class ParameterNode final: public Leaf<std::string>
{
public:
    explicit ParameterNode(
      const std::string& value,
      Visitors::TimeIndex time_index = Visitors::TimeIndex::VARYING_IN_TIME_AND_SCENARIO):
        Leaf<std::string>(value),
        time_index_(time_index)
    {
    }

    std::string name() const override
    {
        return "ParameterNode";
    }

    Visitors::TimeIndex timeIndex() const
    {
        return time_index_;
    }

private:
    Visitors::TimeIndex time_index_;
};
} // namespace Antares::Solver::Nodes
