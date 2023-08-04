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

    bool BindingConstraintGroupRepository::buildFrom(const BindingConstraintsRepository &repository) {
        for (const auto& constraint: repository) {
            const auto group_found = operator[](constraint->group());
            BindingConstraintGroup* group;
            if (group_found) {
                group = group_found;
            } else {
                group = add(constraint->group());
            }
            group->add(constraint);
        }

        return timeSeriesWidthConsistentInGroups();
    }

    bool BindingConstraintGroupRepository::timeSeriesWidthConsistentInGroups() const {
        bool allConsistent = !std::any_of(groups_.begin(), groups_.end(), [](const auto &group) {
                                            const auto& constraints = group->constraints();
                                            if (constraints.empty())
                                                return false;
                                            auto width = (*constraints.begin())->RHSTimeSeries().width;
                                            bool isConsistent = std::all_of(constraints.begin(), constraints.end(), [&width](const std::shared_ptr<BindingConstraint>& bc){
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
        std::for_each(groups_.begin(), groups_.end(), [](auto& group) {
            group->fixTSNumbersWhenWidthIsOne();
        });
    }

    BindingConstraintGroup* BindingConstraintGroupRepository::operator[](std::string name) {
        if (auto group = std::find_if(groups_.begin(), groups_.end(), [&name](auto& group_of_constraint) {
                                        return group_of_constraint->name() == name;
                                    });
            group != groups_.end())
        {
            return group->get();
        }
        return nullptr;
    }

    BindingConstraintGroupRepository::iterator BindingConstraintGroupRepository::begin() {
        return groups_.begin();
    }

    BindingConstraintGroupRepository::const_iterator
    BindingConstraintGroupRepository::begin() const {
        return groups_.begin();
    }

    BindingConstraintGroupRepository::iterator BindingConstraintGroupRepository::end() {
        return groups_.end();
    }

    BindingConstraintGroupRepository::const_iterator BindingConstraintGroupRepository::end() const {
        return groups_.end();
    }

    BindingConstraintGroup* BindingConstraintGroupRepository::add(const std::string& name) {
        return groups_.emplace_back(std::make_unique<BindingConstraintGroup>(name)).get();
    }

    void BindingConstraintGroupRepository::clear() {
        groups_.clear();
    }
} // Data