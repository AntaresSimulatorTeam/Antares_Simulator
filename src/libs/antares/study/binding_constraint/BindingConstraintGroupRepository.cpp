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

    bool BindingConstraintGroupRepository::buildFrom(BindingConstraintsRepository &repository) {
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

        bool tsAreConsistent = timeSeriesWidthConsistentInGroups();

        return tsAreConsistent;
    }

    bool BindingConstraintGroupRepository::timeSeriesWidthConsistentInGroups() const {
        bool hasError = false;
        for(const auto& group: this->groups_) {
            unsigned count = 0;
            for (const auto& bc: group->constraints()) {
                auto width = bc->RHSTimeSeries().width;
                if (count == 0) {
                    count = width;
                    continue;
                }
                if (count != width) {
                    logs.error() << "Inconsistent time series width for constraint of the same group. Group at fault: "
                                 << bc->group()
                                 << " .Previous width was " << count
                                 << " new constraint " << bc->name()
                                 << " found with width of " << width;
                    hasError = true;
                }
            }
        }
        return !hasError;
    }
} // Data