//
// Created by marechaljas on 28/06/23.
//

#include "BindingConstraintGroupRepository.h"
#include <algorithm>
#include <memory>
#include "BindingConstraintsRepository.h"
#include "BindingConstraintGroup.h"

namespace Antares::Data {

    unsigned BindingConstraintGroupRepository::size() const {
        return groups_.size();
    }

    void BindingConstraintGroupRepository::buildFrom(BindingConstraintsRepository &repository) {
        for (auto constraint: repository) {
            auto group_found = std::find_if(groups_.begin(), groups_.end(), [constraint](auto group) {
                return group->name() == constraint->group();
            });
            std::shared_ptr<BindingConstraintGroup> group;
            if (group_found == groups_.end()) {
                group = groups_.emplace_back(std::make_shared<BindingConstraintGroup>(constraint->group()));
            } else {
                group = *group_found;
            }
            group->add(constraint);
        }
    }
} // Data