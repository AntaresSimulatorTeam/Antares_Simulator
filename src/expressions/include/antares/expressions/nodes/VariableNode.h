#pragma once

#include <string>

#include <antares/expressions/nodes/Leaf.h>
#include "antares/modeler-optimisation-container/VariabilityType.h"

namespace Antares::Optimisation
{
enum class VariabilityType : unsigned int;
}

namespace Antares::Expressions::Nodes
{

/**
 * @brief Represents a variable node in a syntax tree, storing a string value.
 */
class VariableNode final: public Leaf<std::string>
{
public:
    explicit VariableNode(const std::string& value,
                          unsigned int index,
                          Optimisation::VariabilityType variability = Optimisation::
                            VariabilityType::VARYING_IN_TIME_AND_SCENARIO):
        Leaf<std::string>(value),
        variability_(variability),
        index_(index)

    {
    }

    std::string name() const override
    {
        return "VariableNode";
    }

    Optimisation::VariabilityType variability() const
    {
        return variability_;
    }

    unsigned int Index() const
    {
        return index_;
    }

private:
    // Is the variable time-dependent / scenario-dependent ?
    const Optimisation::VariabilityType variability_;
    // Local index within the component, starting from 0
    const unsigned int index_ = 0;
};
} // namespace Antares::Expressions::Nodes
