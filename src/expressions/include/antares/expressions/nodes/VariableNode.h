#pragma once

#include <string>

#include <antares/expressions/nodes/Leaf.h>
#include "antares/modeler-optimisation-container/TimeIndex.h"

namespace Antares::Optimisation
{
enum class TimeIndex : unsigned int;
}

namespace Antares::Expressions::Nodes
{

/**
 * @brief Represents a variable node in a syntax tree, storing a string value.
 */
class VariableNode final: public Leaf<std::string>
{
public:
    explicit VariableNode(
      const std::string& value,
      unsigned int index,
      Optimisation::TimeIndex time_index = Optimisation::TimeIndex::VARYING_IN_TIME_AND_SCENARIO):
        Leaf<std::string>(value),
        time_index_(time_index),
        index_(index)

    {
    }

    std::string name() const override
    {
        return "VariableNode";
    }

    Optimisation::TimeIndex timeIndex() const
    {
        return time_index_;
    }

    unsigned int Index() const
    {
        return index_;
    }

private:
    // Is the variable time-dependent / scenario-dependent ?
    const Optimisation::TimeIndex time_index_;
    // Local index within the component, starting from 0
    const unsigned int index_ = 0;
};
} // namespace Antares::Expressions::Nodes
