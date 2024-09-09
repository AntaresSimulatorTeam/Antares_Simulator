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

#include "antares/study/binding_constraint/BindingConstraintGroup.h"

#include <algorithm>
#include <utility>

namespace Antares::Data
{

void BindingConstraintGroup::add(const std::shared_ptr<BindingConstraint>& constraint)
{
    constraints_.insert(constraint);
}

BindingConstraintGroup::BindingConstraintGroup(std::string name):
    name_(std::move(name))
{
}

std::set<std::shared_ptr<BindingConstraint>> BindingConstraintGroup::constraints() const
{
    return constraints_;
}

unsigned BindingConstraintGroup::numberOfTimeseries() const
{
    // Assume all BC in a group have the same width
    if (constraints_.empty())
    {
        return 0;
    }
    return (*constraints_.begin())->RHSTimeSeries().width;
}

} // namespace Antares::Data
