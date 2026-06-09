// Copyright 2007-2026, RTE (https://www.rte-france.com)
// SPDX-License-Identifier: MPL-2.0

//
// Created by marechaljas on 22/03/23.
//
#include "antares/study/scenario-builder/BindingConstraintsTSNumbersData.h"

#include <iomanip>
#include <sstream>

#include "antares/study/scenario-builder/applyToMatrix.hxx"

namespace Antares::Data::ScenarioBuilder
{
bool BindingConstraintsTSNumberData::apply(Study& study)
{
    return std::all_of(
      rules_.begin(),
      rules_.end(),
      [&study, this](const auto& args)
      {
          const auto& [groupName, tsNumbers] = args;
          auto group = study.bindingConstraintsGroups[groupName];
          if (group == nullptr)
          {
              logs.error("Group with name '" + groupName + "' does not exists");
          }
          uint errors = 0;
          CString<512, false> logprefix;
          logprefix.clear() << "Binding constraints: group '" << groupName << "': ";
          return ApplyToMatrix(errors, logprefix, *group, tsNumbers[0], get_tsGenCount(study));
      });
}

bool BindingConstraintsTSNumberData::reset(const Study& study)
{
    const uint nbYears = study.parameters.nbYears;
    std::ranges::for_each(study.bindingConstraintsGroups,
                          [this, &nbYears](const auto& group)
                          {
                              auto& ts_numbers = rules_[group->name()];
                              ts_numbers.reset(1, nbYears);
                          });
    return true;
}

void BindingConstraintsTSNumberData::setTSnumber(const std::string& group_name,
                                                 const uint year,
                                                 uint value)
{
    auto& group_ts_numbers = rules_[group_name];
    if (year < group_ts_numbers.height)
    {
        group_ts_numbers[0][year] = value;
    }
}

} // namespace Antares::Data::ScenarioBuilder
