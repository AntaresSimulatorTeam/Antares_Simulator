//
// Created by marechaljas on 28/06/23.
//

#include "BindingConstraintGroup.h"

#include <utility>

namespace Antares::Data {

    void BindingConstraintGroup::add(const std::shared_ptr<BindingConstraint>& constraint) {
        constraints_.insert(constraint);
    }

    BindingConstraintGroup::BindingConstraintGroup(std::string name):
    name_(std::move(name))
    {

    }

} // Data