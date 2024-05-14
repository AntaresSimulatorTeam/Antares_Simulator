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

#pragma once

#include <functional>
#include <memory>
#include <set>

#include "antares/study/binding_constraint/BindingConstraintGroup.h"
#include "antares/study/fwd.h"

#include "BindingConstraint.h"
#include "BindingConstraintSaver.h"

namespace Antares::Data
{

class BindingConstraintGroupRepository
{
public:
    [[nodiscard]] unsigned size() const;

    [[nodiscard]] bool buildFrom(const BindingConstraintsRepository& repository);

    void resizeAllTimeseriesNumbers(unsigned nb_years);

    BindingConstraintGroup* operator[](const std::string& name) const;

    using iterator = std::vector<std::unique_ptr<BindingConstraintGroup>>::iterator;
    using const_iterator = std::vector<std::unique_ptr<BindingConstraintGroup>>::const_iterator;

    [[nodiscard]] iterator begin();
    [[nodiscard]] const_iterator begin() const;

    [[nodiscard]] iterator end();
    [[nodiscard]] const_iterator end() const;

    BindingConstraintGroup* add(const std::string& name);
    void clear();

private:
    [[nodiscard]] bool timeSeriesWidthConsistentInGroups() const;

    std::vector<std::unique_ptr<BindingConstraintGroup>> groups_;
};

} // namespace Antares::Data
