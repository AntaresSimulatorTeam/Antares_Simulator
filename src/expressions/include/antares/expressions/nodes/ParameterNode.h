#pragma once

#include <string>

#include <antares/expressions/nodes/Leaf.h>
#include <antares/modeler-optimisation-container/TimeVariation.h>

namespace Antares::Expressions::Nodes
{
/**
 * @brief Represents a parameter node in a syntax tree, storing a string value.
 */
class ParameterNode final: public Leaf<std::string>
{
public:
    explicit ParameterNode(const std::string& value,
                           Optimisation::TimeScenarioVariability time_index = Optimisation::
                             TimeScenarioVariability::VARYING_IN_TIME_AND_SCENARIO):
        Leaf<std::string>(value),
        time_index_(time_index)
    {
    }

    std::string name() const override
    {
        return "ParameterNode";
    }

    Optimisation::TimeScenarioVariability timeIndex() const
    {
        return time_index_;
    }

private:
    Optimisation::TimeScenarioVariability time_index_;
};
} // namespace Antares::Expressions::Nodes
