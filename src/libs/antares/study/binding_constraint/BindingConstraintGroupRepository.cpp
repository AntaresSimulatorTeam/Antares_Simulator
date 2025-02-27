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

    bool BindingConstraintGroupRepository::buildFrom(const BindingConstraintsRepository &repository) {
        groupsByName_.clear();
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

    static bool checkAllElementsIdenticalOrOne(std::vector<std::pair<unsigned, std::string>>& p)
    {
        // Erase 1 from the vector
        std::erase_if(p, [](const auto& pair) { return pair.first == 1; });
        auto width = p.begin()->first;
        for (const auto& [w, msg]: p)
        {
            if (w != width)
            {
                logs.error() << "Inconsitent time series width, found: " << w << " Previous was "
                    << width << " for " << msg;

                return false;
            }
        }
        return true;
    }

    bool BindingConstraintGroupRepository::timeSeriesWidthConsistentInGroups() const {
        bool allConsistent = std::ranges::all_of(groups_, [](const auto& group)
          {
              const auto& constraints = group->constraints();
              if (constraints.empty())
              {
                  return true;
              }

              std::vector<std::pair<unsigned, std::string>> constraintsWidth;
              constraintsWidth.reserve(constraints.size());
              for (const auto& c: constraints)
              {
                  std::string msg = "Constraint group: " + c->group() + " name: " + c->name();
                  constraintsWidth.emplace_back(c->RHSTimeSeries().width, msg);
              }
              return checkAllElementsIdenticalOrOne(constraintsWidth);
          });
          return allConsistent;
    }

    void BindingConstraintGroupRepository::resizeAllTimeseriesNumbers(unsigned int nb_years) {
        std::for_each(groups_.begin(), groups_.end(), [&](auto &group) {
            group->timeseriesNumbers.clear();
            group->timeseriesNumbers.reset(1, nb_years);
        });
    }

    BindingConstraintGroup* BindingConstraintGroupRepository::operator[](const std::string& name) const {
        if (auto it = groupsByName_.find(name); it != groupsByName_.end())
        {
            return it->second;
        }
        auto group = std::ranges::find_if(groups_, [&name](auto group_of_constraint) {
                                        return group_of_constraint->name() == name;
                                    });
        if (group != groups_.end())
        {
            groupsByName_[name] = *group;
            return *group;
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
        auto& new_group = owning_groups_.emplace_back(std::make_unique<BindingConstraintGroup>(name));
        return new_group.get();
    }

    void BindingConstraintGroupRepository::clear() {
        groupsByName_.clear();
        groups_.clear();
        owning_groups_.clear();
    }
} // Data
