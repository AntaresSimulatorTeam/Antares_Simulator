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

    void BindingConstraintGroupRepository::resizeAllTimeseriesNumbers(unsigned int nb_years) {
        std::for_each(groups_.begin(), groups_.end(), [&](auto &group) {
            group->timeSeriesNumbers().timeseriesNumbers.clear();
            group->timeSeriesNumbers().timeseriesNumbers.resize(1, nb_years);
        });
    }

    void BindingConstraintGroupRepository::fixTSNumbersWhenWidthIsOne() {
        std::for_each(groups_.begin(), groups_.end(), [](auto group) {
            group->fixTSNumbersWhenWidthIsOne();
        });
    }

    std::shared_ptr<BindingConstraintGroup> BindingConstraintGroupRepository::operator[](std::string name) {
        if (auto group = std::find_if(groups_.begin(), groups_.end(), [&name](auto group) {
            return group->name() == name;
        }); group != groups_.end()) {
            return *group;
        }
        return nullptr;
    }

    std::vector<std::shared_ptr<BindingConstraintGroup>>::iterator BindingConstraintGroupRepository::begin() {
        return groups_.begin();
    }

    std::vector<std::shared_ptr<BindingConstraintGroup>>::const_iterator
    BindingConstraintGroupRepository::begin() const {
        return groups_.begin();
    }

    std::vector<std::shared_ptr<BindingConstraintGroup>>::iterator BindingConstraintGroupRepository::end() {
        return groups_.end();
    }

    std::vector<std::shared_ptr<BindingConstraintGroup>>::const_iterator BindingConstraintGroupRepository::end() const {
        return groups_.end();
    }

    std::shared_ptr<BindingConstraintGroup> BindingConstraintGroupRepository::add(const std::string& name) {
        auto group = groups_.emplace_back(std::make_shared<BindingConstraintGroup>(name));
        return group;
    }

    void BindingConstraintGroupRepository::clear() {
        groups_.clear();
    }
} // Data