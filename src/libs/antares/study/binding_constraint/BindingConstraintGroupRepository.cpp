/*
** Copyright 2007-2024, RTE (https://www.rte-france.com)
** See AUTHORS.txt
** SPDX-License-Identifier: MPL-2.0
** This file is part of Antares-Simulator,
** Adequacy and Performance assessment for interconnected energy networks.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
*/
//
// Created by marechaljas on 28/06/23.
//

#include "antares/study/binding_constraint/BindingConstraintGroupRepository.h"

#include <algorithm>
#include <memory>
#include <numeric>

#include "antares/study/binding_constraint/BindingConstraintGroup.h"
#include "antares/study/binding_constraint/BindingConstraintsRepository.h"

namespace Antares::Data
{

unsigned BindingConstraintGroupRepository::size() const
{
    return groups_.size();
}

bool BindingConstraintGroupRepository::buildFrom(const BindingConstraintsRepository& repository)
{
    for (const auto& constraint: repository)
    {
        const auto group_found = operator[](constraint->group());
        BindingConstraintGroup* group;
        if (group_found)
        {
            group = group_found;
        }
        else
        {
            group = add(constraint->group());
        }
        group->add(constraint);
    }
    return timeSeriesWidthConsistentInGroups();
}

bool BindingConstraintGroupRepository::timeSeriesWidthConsistentInGroups() const
{
    bool allConsistent = !std::ranges::any_of(
      groups_,
      [](const auto& group)
      {
          const auto& constraints = group->constraints();
          if (constraints.empty())
          {
              return false;
          }
          auto width = (*constraints.begin())->RHSTimeSeries().width;
          bool isConsistent = std::ranges::all_of(
            constraints,
            [&width](const std::shared_ptr<BindingConstraint>& bc)
            {
                bool sameWidth = bc->RHSTimeSeries().width == width;
                if (!sameWidth)
                {
                    logs.error() << "Inconsistent time series width for constraint of the same "
                                    "group. Group at fault: "
                                 << bc->group() << " .Previous width was " << width
                                 << " new constraint " << bc->name() << " found with width of "
                                 << bc->RHSTimeSeries().width;
                }
                return sameWidth;
            });
          return !isConsistent;
      });
    return allConsistent;
}

void BindingConstraintGroupRepository::resizeAllTimeseriesNumbers(unsigned int nb_years)
{
    std::ranges::for_each(groups_,
                          [&nb_years](auto& group) { group->timeseriesNumbers.reset(nb_years); });
}

BindingConstraintGroup* BindingConstraintGroupRepository::operator[](const std::string& name) const
{
    if (auto group = std::ranges::find_if(groups_,
                                          [&name](auto& group_of_constraint)
                                          { return group_of_constraint->name() == name; });
        group != groups_.end())
    {
        return group->get();
    }
    return nullptr;
}

BindingConstraintGroupRepository::iterator BindingConstraintGroupRepository::begin()
{
    return groups_.begin();
}

BindingConstraintGroupRepository::const_iterator BindingConstraintGroupRepository::begin() const
{
    return groups_.begin();
}

BindingConstraintGroupRepository::iterator BindingConstraintGroupRepository::end()
{
    return groups_.end();
}

BindingConstraintGroupRepository::const_iterator BindingConstraintGroupRepository::end() const
{
    return groups_.end();
}

BindingConstraintGroup* BindingConstraintGroupRepository::add(const std::string& name)
{
    return groups_.emplace_back(std::make_unique<BindingConstraintGroup>(name)).get();
}

void BindingConstraintGroupRepository::clear()
{
    groups_.clear();
}
} // namespace Antares::Data
