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
// Created by marechaljas on 22/03/23.
//
/*
** Copyright 2007-2023 RTE
** Authors: Antares_Simulator Team
**
** This file is part of Antares_Simulator.
**
** Antares_Simulator is free software: you can redistribute it and/or modify
** it under the terms of the Mozilla Public Licence 2.0 as published by
** the Mozilla Foundation, either version 2 of the License, or
** (at your option) any later version.
**
** There are special exceptions to the terms and conditions of the
** license as they are applied to this software. View the full text of
** the exceptions in file COPYING.txt in the directory of this software
** distribution
**
** Antares_Simulator is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** Mozilla Public Licence 2.0 for more details.
**
** You should have received a copy of the Mozilla Public Licence 2.0
** along with Antares_Simulator. If not, see <https://opensource.org/license/mpl-2-0/>.
**
** SPDX-License-Identifier: MPL-2.0
*/

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

void BindingConstraintsTSNumberData::saveToINIFile(const Study&, Yuni::IO::File::Stream& file) const
{
    // Turning values into strings (precision 4)
    std::ostringstream value_into_string;
    value_into_string << std::setprecision(4);

    for (const auto& [group_name, ts_numbers]: rules_)
    {
        for (unsigned year = 0; year < ts_numbers.height; ++year)
        {
            auto value = ts_numbers[0][year];
            if (value != 0)
            {
                file << get_prefix() << group_name << "," << year << "=" << value << "\n";
            }
        }
    }
}

void BindingConstraintsTSNumberData::setTSnumber(const std::string& group_name,
                                                 const uint year,
                                                 uint value)
{
    auto& group_ts_numbers = rules_[group_name];
    group_ts_numbers[0][year] = value;
}

} // namespace Antares::Data::ScenarioBuilder
