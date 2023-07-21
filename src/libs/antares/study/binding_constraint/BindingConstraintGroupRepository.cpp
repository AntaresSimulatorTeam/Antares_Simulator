//
// Created by marechaljas on 28/06/23.
//

#include "BindingConstraintGroupRepository.h"
#include <algorithm>
#include <numeric>
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

        return timeSeriesWidthConsistentInGroups();
    }

    bool BindingConstraintGroupRepository::timeSeriesWidthConsistentInGroups() const {
        bool allConsistent = !std::any_of(groups_.begin(), groups_.end(), [](const std::shared_ptr<Antares::Data::BindingConstraintGroup> &group) {
                                            if (group->constraints().empty())
                                                return false;
                                            auto width = (*group->constraints().begin())->RHSTimeSeries().width;
                                            bool isConsistent = std::all_of(group->constraints().begin(), group->constraints().end(), [&width](const std::shared_ptr<BindingConstraint>& bc){
                                                              bool sameWidth = bc->RHSTimeSeries().width == width;
                                                              if (!sameWidth) {
                                                                  logs.error() << "Inconsistent time series width for constraint of the same group. Group at fault: "
                                                                               << bc->group()
                                                                               << " .Previous width was " << width
                                                                               << " new constraint " << bc->name()
                                                                               << " found with width of " << bc->RHSTimeSeries().width;
                                                              }
                                                              return sameWidth;
                                                          });
                                            return !isConsistent;
        });
        return allConsistent;
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