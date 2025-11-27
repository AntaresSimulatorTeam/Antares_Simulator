#pragma once

#include <string>

#include <antares/expressions/nodes/Leaf.h>
#include <antares/modeler-optimisation-container/VariabilityType.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a parameter node in a syntax tree, storing a string value.
 */
class ParameterNode final: public Leaf<std::string>
{
public:
    explicit ParameterNode(const std::string& value,
                           Optimisation::VariabilityType time_index = Optimisation::
                             VariabilityType::VARYING_IN_TIME_AND_SCENARIO):
        Leaf<std::string>(value),
        time_index_(time_index)
    {
    }

    std::string name() const override
    {
        return "ParameterNode";
    }

    Optimisation::VariabilityType timeIndex() const
    {
        return time_index_;
    }

private:
    Optimisation::VariabilityType time_index_;
};
} // namespace Antares::Expressions::Nodes
