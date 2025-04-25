//
// Created by marechaljas on 28/06/23.
//

#include "BindingConstraintGroup.h"

#include <algorithm>
#include <utility>

namespace Antares::Data
{

void BindingConstraintGroup::add(const std::shared_ptr<BindingConstraint>& constraint)
{
    constraints_.insert(constraint);
}

BindingConstraintGroup::BindingConstraintGroup(std::string name) : name_(std::move(name))
{
}

const BindingConstraint::Set& BindingConstraintGroup::constraints() const
{
    return constraints_;
}

unsigned BindingConstraintGroup::numberOfTimeseries() const
{
    // Assume all BC in a group have the same width
    if (constraints_.empty())
    {
        return 0;
    }
    auto it
      = std::ranges::max_element(constraints_,
                                 [](const auto& a, const auto& b)
                                 { return a->RHSTimeSeries().width < b->RHSTimeSeries().width; });
    return (*it)->RHSTimeSeries().width;
}

std::string BindingConstraintGroup::name()
{
    return name_;
}
} // namespace Antares::Data
